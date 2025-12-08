#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <sstream>
#include <algorithm>
#include "auth.h"
#include "Process.h"
#include "scheduler.h"
#include "vm.h"
using namespace std;

static int min_arrival(const vector<Process>& P){
    if (P.empty()) return 0;
    int m = P[0].getArrivalTime();
    for (size_t i = 1; i < P.size(); ++i) m = min(m, P[i].getArrivalTime());
    return m;
}
static int max_end(const vector<ExecutionSlice>& g){
    int m = 0; for (size_t i=0;i<g.size();++i) m = max(m, g[i].endTime); return m;
}

static void run_scheduler_flow() {
    int n;
    cout << "\nEnter number of processes: ";
    cin >> n;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<int> pids(n), at(n), bt(n);
    for (int i = 0; i < n; ++i) {
        while (true) {
            cout << "P" << (i + 1) << " -> PID Arrival Burst: ";
            string line; getline(cin, line);
            istringstream iss(line);
            if (iss >> pids[i] >> at[i] >> bt[i]) break;
            cout << "Please enter three integers: <PID> <Arrival> <Burst>\n";
        }
    }

    vector<Process> procs; procs.reserve(n);
    for (int i = 0; i < n; ++i) procs.emplace_back(pids[i], at[i], bt[i], 1, 0, false);

    cout << "\nChoose Scheduling Algorithm:\n"
            "  1) FCFS\n"
            "  2) SJF (non-preemptive)\n"
            "  3) SRTF (preemptive SJF)\n"
            "Select: ";
    int choice; cin >> choice;
    ScheduleType method = ScheduleType::FCFS;
    if (choice == 2) method = ScheduleType::SJF; else if (choice == 3) method = ScheduleType::SRTF;

    vector<ExecutionSlice> gantt = Scheduler::run(method, procs);

    cout << "\n=== Gantt Timeline ===\n";
    for (size_t i=0;i<gantt.size();++i) cout << "[t=" << setw(2) << gantt[i].startTime << "->" << setw(2) << gantt[i].endTime << "] P" << gantt[i].pid << "\n";

    cout << "\n=== Per-Tick View ===\n";
    int startT = procs.empty() ? 0 : min_arrival(procs);
    int endT = max_end(gantt);
    for (int t = startT; t < endT; ++t) {
        int runningPid = -1;
        for (size_t i=0;i<gantt.size();++i) if (t >= gantt[i].startTime && t < gantt[i].endTime) { runningPid = gantt[i].pid; break; }
        cout << "[Time " << t << "] RUN: " << (runningPid==-1? string("(idle)") : string("P")+to_string(runningPid)) << "\n";
    }

    double avgTAT = 0.0, avgWT = 0.0;
    cout << "\n=== Results ===\n";
    cout << "PID\tAT\tBT\tCT\tTAT\tWT\n";
    for (size_t i=0;i<procs.size();++i) {
        int pid=procs[i].getPid(), arrival=procs[i].getArrivalTime(), burst=procs[i].getBurstTime();
        int completion=0; for (size_t j=0;j<gantt.size();++j) if (gantt[j].pid==pid) completion = max(completion, gantt[j].endTime);
        int tat=procs[i].getTurnaroundTime(), wt=procs[i].getWaitingTime();
        avgTAT += tat; avgWT += wt;
        cout << pid << '\t' << arrival << '\t' << burst << '\t' << completion << '\t' << tat << '\t' << wt << '\n';
    }
    if (!procs.empty()) { avgTAT/=procs.size(); avgWT/=procs.size(); }
    cout << fixed << setprecision(2);
    cout << "Average Turnaround Time: " << avgTAT << "\n";
    cout << "Average Waiting Time:    " << avgWT << "\n";
}

static void run_vm_shell() {
    VirtualMemory vm(4096, 64, 8, 8, "CLOCK");
    cout << "\nVM shell. cmds: init ps vp fr tlb [CLOCK|LRU] | read a | write a b | fill a len b | seq start ops [w] | rand ops max [seed] [w] | dumpvpn v | dumpf f | stats | policy P | reset | back\n";
    string cmd;
    while (true) {
        cout << "vm> ";
        if (!(cin >> cmd)) return;
        if (cmd=="back") return;
        if (cmd=="init") { size_t ps; int vp,fr,tlb; string pol="CLOCK"; cin>>ps>>vp>>fr>>tlb; if (cin.peek()==' ') cin>>pol; vm.init(ps,vp,fr,tlb,pol); cout<<"ok\n"; }
        else if (cmd=="read") { uint64_t a; cin>>a; try{ cout<<(int)vm.read(a)<<"\n"; }catch(std::exception&){ cout<<"err\n"; } }
        else if (cmd=="write") { uint64_t a; int b; cin>>a>>b; try{ vm.write(a,(uint8_t)(b&0xFF)); cout<<"ok\n"; }catch(std::exception&){ cout<<"err\n"; } }
        else if (cmd=="fill") { uint64_t a; size_t len; int b; cin>>a>>len>>b; try{ vm.fill(a,len,(uint8_t)(b&0xFF)); cout<<"ok\n"; }catch(std::exception&){ cout<<"err\n"; } }
        else if (cmd=="seq") { uint64_t s; int ops; string w="r"; cin>>s>>ops; if (cin.peek()==' ') cin>>w; vm.seq(s,ops,(w=="w"||w=="W")); cout<<"ok\n"; }
        else if (cmd=="rand") { int ops; uint64_t mx; uint32_t seed=12345; string w="r"; cin>>ops>>mx; if (cin.peek()==' ') cin>>seed; if (cin.peek()==' ') cin>>w; vm.rnd(ops,mx,seed,(w=="w"||w=="W")); cout<<"ok\n"; }
        else if (cmd=="dumpvpn") { int v; cin>>v; vm.dumpVpn(v); }
        else if (cmd=="dumpf") { int f; cin>>f; vm.dumpFrame(f); }
        else if (cmd=="stats") { vm.stats(); }
        else if (cmd=="policy") { string p; cin>>p; vm.setPolicy(p); cout<<"ok\n"; }
        else if (cmd=="reset") { vm.reset(); cout<<"ok\n"; }
        else { cout<<"unknown\n"; }
    }
}

int main() {
    cout << "OS boot... done\n";
    string user, pass; int tries = 0;
    while (tries < 3) {
        cout << "Admin Login\n";
        cout << "Username: "; cin >> user;
        cout << "Password: "; cin >> pass;
        if (authenticateUser(user, pass)) break;
        ++tries; if (tries==3) { cout << "Too many attempts. Exiting.\n"; return 1; }
        cout << "Invalid credentials. Try again.\n";
    }

    while (true) {
        cout << "\nMain Menu\n1) CPU Scheduling Simulator\n2) Virtual Memory Shell\n3) Exit\nSelect: ";
        int opt; if (!(cin>>opt)) return 0;
        if (opt==1) run_scheduler_flow();
        else if (opt==2) run_vm_shell();
        else if (opt==3) return 0;
        else cout<<"Invalid\n";
    }
}
