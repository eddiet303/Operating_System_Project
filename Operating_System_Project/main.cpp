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
using namespace std;

static int min_arrival(const vector<Process>& P){
    if (P.empty()) return 0;
    int m = P[0].getArrivalTime();
    for (size_t i = 1; i < P.size(); ++i) m = min(m, P[i].getArrivalTime());
    return m;
}
static int max_end(const vector<ExecutionSlice>& g){
    int m = 0;
    for (auto& s : g) m = max(m, s.endTime);
    return m;
}

int main() {
    cout << "OS boot... done\n";

    string user, pass;
    int tries = 0;
    while (tries < 3) {
        cout << "Admin Login\n";
        cout << "Username: "; cin >> user;
        cout << "Password: "; cin >> pass;
        if (authenticateUser(user, pass)) break;
        ++tries;
        if (tries == 3) { cout << "Too many attempts. Exiting.\n"; return 1; }
        cout << "Invalid credentials. Try again.\n";
    }

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

    vector<Process> procs;
    procs.reserve(n);
    for (int i = 0; i < n; ++i) procs.emplace_back(pids[i], at[i], bt[i], 1, 0, false);

    cout << "\nChoose Scheduling Algorithm:\n"
            "  1) FCFS\n"
            "  2) SJF (non-preemptive)\n"
            "  3) SRTF (preemptive SJF)\n"
            "Select: ";
    int choice; cin >> choice;
    ScheduleType method = ScheduleType::FCFS;
    if (choice == 2) method = ScheduleType::SJF;
    else if (choice == 3) method = ScheduleType::SRTF;

    auto gantt = Scheduler::run(method, procs);

    cout << "\n=== Gantt Timeline ===\n";
    for (auto& s : gantt) {
        cout << "[t=" << setw(2) << s.startTime << "→" << setw(2) << s.endTime << "] P" << s.pid << "\n";
    }

    cout << "\n=== Per-Tick View ===\n";
    int startT = procs.empty() ? 0 : min_arrival(procs);
    int endT = max_end(gantt);
    for (int t = startT; t < endT; ++t) {
        int runningPid = -1;
        for (auto& s : gantt) if (t >= s.startTime && t < s.endTime) { runningPid = s.pid; break; }
        cout << "[Time " << t << "] RUN: ";
        if (runningPid == -1) cout << "(idle)\n"; else cout << "P" << runningPid << "\n";
    }

    double avgTAT = 0.0, avgWT = 0.0;
    cout << "\n=== Results ===\n";
    cout << "PID\tAT\tBT\tCT\tTAT\tWT\n";
    for (const auto& p : procs) {
        int pid = p.getPid();
        int arrival = p.getArrivalTime();
        int burst = p.getBurstTime();
        int completion = 0;
        for (auto& s : gantt) if (s.pid == pid) completion = max(completion, s.endTime);
        int tat = p.getTurnaroundTime();
        int wt  = p.getWaitingTime();
        avgTAT += tat; avgWT += wt;
        cout << pid << '\t' << arrival << '\t' << burst << '\t' << completion << '\t' << tat << '\t' << wt << '\n';
    }
    if (n > 0) { avgTAT /= n; avgWT /= n; }
    cout << fixed << setprecision(2);
    cout << "Average Turnaround Time: " << avgTAT << "\n";
    cout << "Average Waiting Time:    " << avgWT << "\n";
    return 0;
}
