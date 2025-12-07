#pragma once
#include <vector>
#include <string>
#include <cstdint>

class VirtualMemory {
public:
    VirtualMemory(std::size_t pageSize=4096, int virtPages=64, int frames=8, int tlbSize=8, const std::string& policy="CLOCK");
    void init(std::size_t pageSize, int virtPages, int frames, int tlbSize, const std::string& policy);
    uint8_t read(uint64_t vaddr);
    void write(uint64_t vaddr, uint8_t value);
    void fill(uint64_t vaddr, std::size_t len, uint8_t value);
    void seq(uint64_t start, int ops, bool writes=false);
    void rnd(int ops, uint64_t maxAddr, uint32_t seed=12345, bool writes=true);
    void dumpVpn(int vpn) const;
    void dumpFrame(int f) const;
    void stats() const;
    void setPolicy(const std::string& p);
    void reset();

    std::size_t pageSize() const;
    int vpnCount() const;
    int frameCount() const;

private:
    struct PTE { bool valid=false, dirty=false, ref=false; int frame=-1; };
    struct TLB { int vpn=-1, frame=-1; uint64_t age=0; };
    struct FMeta { int vpn=-1; bool ref=false; bool dirty=false; };

    std::size_t PAGE_SIZE;
    int VPN_COUNT, FRAME_COUNT, TLB_SIZE;
    std::string policy;

    std::vector<std::vector<uint8_t>> backing;
    std::vector<std::vector<uint8_t>> phys;
    std::vector<PTE> ptab;
    std::vector<TLB> tlb;
    std::vector<FMeta> fmeta;
    std::vector<int> freeList;
    int clockHand=0;

    uint64_t tlbHits=0, tlbMisses=0, faults=0, evictions=0, writebacks=0, reads=0, writes=0, ticks=0;

    std::pair<int,int> split(uint64_t vaddr) const;
    int translate(int vpn, bool isWrite);
    int tlbLookup(int vpn);
    void tlbInsert(int vpn, int frame);
    void pageFault(int vpn);
    int allocFrame();
    int evictClock();
    int evictLRU();
    void tlbInvalidate(int vpn);
    void touch();
};
