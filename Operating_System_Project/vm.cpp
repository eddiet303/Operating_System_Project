#include "vm.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <random>

using std::vector; using std::uint8_t; using std::size_t; using std::string;

VirtualMemory::VirtualMemory(size_t ps, int vp, int fr, int tlbSz, const string& pol) { init(ps, vp, fr, tlbSz, pol); }

void VirtualMemory::init(size_t ps, int vp, int fr, int tlbSz, const string& pol) {
    PAGE_SIZE = ps; VPN_COUNT = vp; FRAME_COUNT = fr; TLB_SIZE = tlbSz; policy = pol;
    backing.assign(VPN_COUNT, vector<uint8_t>(PAGE_SIZE, 0));
    phys.assign(FRAME_COUNT, vector<uint8_t>(PAGE_SIZE, 0));
    ptab.assign(VPN_COUNT, {}); tlb.assign(TLB_SIZE, {}); fmeta.assign(FRAME_COUNT, {});
    freeList.clear(); for (int f=0; f<FRAME_COUNT; ++f) freeList.push_back(f);
    clockHand = 0; tlbHits=tlbMisses=faults=evictions=writebacks=reads=writes=ticks=0;
}

uint8_t VirtualMemory::read(uint64_t vaddr) {
    touch(); auto [vpn, off] = split(vaddr); int f = translate(vpn, false);
    fmeta[f].ref = true; ptab[vpn].ref = true; ++reads; return phys[f][off];
}

void VirtualMemory::write(uint64_t vaddr, uint8_t val) {
    touch(); auto [vpn, off] = split(vaddr); int f = translate(vpn, true);
    fmeta[f].ref = true; fmeta[f].dirty = true; ptab[vpn].ref = true; ptab[vpn].dirty = true; ++writes; phys[f][off] = val;
}

void VirtualMemory::fill(uint64_t vaddr, size_t len, uint8_t val) { for (size_t i=0;i<len;i++) write(vaddr+i, val); }

void VirtualMemory::seq(uint64_t start, int ops, bool wr) {
    for (int i=0;i<ops;i++) { uint64_t a=(start+i)%(uint64_t(VPN_COUNT)*PAGE_SIZE); if (wr) write(a, uint8_t(a&0xFF)); else (void)read(a); }
}

void VirtualMemory::rnd(int ops, uint64_t maxAddr, uint32_t seed, bool wr) {
    std::mt19937 rng(seed); std::uniform_int_distribution<uint64_t> A(0, maxAddr);
    std::uniform_int_distribution<int> B(0,255);
    for (int i=0;i<ops;i++) { uint64_t a=A(rng)%(uint64_t(VPN_COUNT)*PAGE_SIZE); if (wr && (rng()&1)) write(a, uint8_t(B(rng))); else (void)read(a); }
}

void VirtualMemory::dumpVpn(int vpn) const {
    if (vpn<0 || vpn>=VPN_COUNT) { std::cout<<"bad vpn\n"; return; }
    std::cout<<"PTE[v"<<vpn<<"] valid="<<ptab[vpn].valid<<" frame="<<ptab[vpn].frame<<" dirty="<<ptab[vpn].dirty<<" ref="<<ptab[vpn].ref<<"\n";
    if (!ptab[vpn].valid) {
        for (size_t i=0;i<std::min<size_t>(32,PAGE_SIZE);++i){ if(i&&i%16==0) std::cout<<"\n"; std::cout<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)backing[vpn][i]<<" "; }
        std::cout<<std::dec<<"\n";
    } else {
        int f=ptab[vpn].frame;
        for (size_t i=0;i<std::min<size_t>(32,PAGE_SIZE);++i){ if(i&&i%16==0) std::cout<<"\n"; std::cout<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)phys[f][i]<<" "; }
        std::cout<<std::dec<<"\n";
    }
}

void VirtualMemory::dumpFrame(int f) const {
    if (f<0 || f>=FRAME_COUNT) { std::cout<<"bad frame\n"; return; }
    std::cout<<"F"<<f<<" vpn="<<fmeta[f].vpn<<" ref="<<fmeta[f].ref<<" dirty="<<fmeta[f].dirty<<"\n";
    for (size_t i=0;i<std::min<size_t>(32,PAGE_SIZE);++i){ if(i&&i%16==0) std::cout<<"\n"; std::cout<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)phys[f][i]<<" "; }
    std::cout<<std::dec<<"\n";
}

void VirtualMemory::stats() const {
    double hit = (tlbHits+tlbMisses)? 100.0*tlbHits/(tlbHits+tlbMisses) : 0.0;
    double fr  = (reads+writes)? 100.0*faults/(reads+writes) : 0.0;
    std::cout<<"Reads "<<reads<<" Writes "<<writes<<"\n";
    std::cout<<"TLB hits "<<tlbHits<<" misses "<<tlbMisses<<" hit% "<<std::fixed<<std::setprecision(2)<<hit<<"\n";
    std::cout<<"Faults "<<faults<<" Evictions "<<evictions<<" WriteBacks "<<writebacks<<" Fault% "<<fr<<"\n";
}

void VirtualMemory::setPolicy(const string& p) { policy = p; }
void VirtualMemory::reset() { init(PAGE_SIZE, VPN_COUNT, FRAME_COUNT, TLB_SIZE, policy); }

size_t VirtualMemory::pageSize() const { return PAGE_SIZE; }
int VirtualMemory::vpnCount() const { return VPN_COUNT; }
int VirtualMemory::frameCount() const { return FRAME_COUNT; }

std::pair<int,int> VirtualMemory::split(uint64_t vaddr) const {
    int vpn = int(vaddr / PAGE_SIZE), off = int(vaddr % PAGE_SIZE);
    if (vpn<0 || vpn>=VPN_COUNT) throw std::runtime_error("address out of range");
    return {vpn, off};
}

int VirtualMemory::translate(int vpn, bool) {
    int f = tlbLookup(vpn);
    if (f!=-1) return f;
    if (!ptab[vpn].valid) pageFault(vpn);
    tlbInsert(vpn, ptab[vpn].frame);
    return ptab[vpn].frame;
}

int VirtualMemory::tlbLookup(int vpn) {
    for (auto& e : tlb) if (e.vpn==vpn) { ++tlbHits; e.age=0; return e.frame; }
    ++tlbMisses; return -1;
}

void VirtualMemory::tlbInsert(int vpn, int frame) {
    int idx=-1; for (int i=0;i<TLB_SIZE;i++) if (tlb[i].vpn==-1) { idx=i; break; }
    if (idx==-1) { uint64_t best=0; int bi=0; for (int i=0;i<TLB_SIZE;i++) if (tlb[i].age>=best) { best=tlb[i].age; bi=i; } idx=bi; }
    tlb[idx] = {vpn, frame, 0};
}

void VirtualMemory::pageFault(int vpn) {
    ++faults;
    int f = allocFrame();
    std::memcpy(&phys[f][0], &backing[vpn][0], PAGE_SIZE);
    ptab[vpn].valid=true; ptab[vpn].frame=f; ptab[vpn].ref=true; ptab[vpn].dirty=false;
    fmeta[f].vpn=vpn; fmeta[f].ref=true; fmeta[f].dirty=false;
}

int VirtualMemory::allocFrame() {
    if (!freeList.empty()) { int f=freeList.back(); freeList.pop_back(); return f; }
    ++evictions; return policy=="LRU" ? evictLRU() : evictClock();
}

int VirtualMemory::evictClock() {
    while (true) {
        int f = clockHand; int v = fmeta[f].vpn;
        if (!ptab[v].ref) {
            if (fmeta[f].dirty || ptab[v].dirty) { ++writebacks; std::memcpy(&backing[v][0], &phys[f][0], PAGE_SIZE); }
            ptab[v] = {}; tlbInvalidate(v); fmeta[f] = {}; clockHand = (clockHand+1)%FRAME_COUNT; return f;
        }
        ptab[v].ref=false; fmeta[f].ref=false; clockHand = (clockHand+1)%FRAME_COUNT;
    }
}

int VirtualMemory::evictLRU() {
    vector<uint64_t> age(FRAME_COUNT, 0);
    for (auto& e: tlb) if (e.vpn!=-1) age[e.frame] = std::max(age[e.frame], e.age);
    int best=0; uint64_t bestAge=0;
    for (int f=0; f<FRAME_COUNT; ++f) if (age[f]>=bestAge) { bestAge=age[f]; best=f; }
    int v = fmeta[best].vpn;
    if (fmeta[best].dirty || ptab[v].dirty) { ++writebacks; std::memcpy(&backing[v][0], &phys[best][0], PAGE_SIZE); }
    ptab[v] = {}; tlbInvalidate(v); fmeta[best] = {}; return best;
}

void VirtualMemory::tlbInvalidate(int vpn) { for (auto& e: tlb) if (e.vpn==vpn) e = {}; }

void VirtualMemory::touch() { ++ticks; for (auto& e: tlb) if (e.vpn!=-1) ++e.age; }
