#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "auth.h"
#include "Process.h"
using namespace std;

int main() {
    cout << "OS boot… done\n";

    string username, password;
    int attempts = 0;
    while (attempts < 3) {
        cout << "Admin Login\n";
        cout << "Username: ";
        cin >> username;                  // visible input
        cout << "Password: ";
        cin >> password;                  // visible input
        if (authenticateUser(username, password)) break;
        attempts++;
        if (attempts == 3) {
            cout << "Too many attempts. Exiting.\n";
            return 1;
        }
        cout << "Invalid. Try again.\n";
    }

    // Demo workload: pid, arrival, burst, priority, mem, io_flag
    vector<Process> procs;
    vector<int> arrival = {0, 1, 2};
    procs.emplace_back(1, arrival[0], 5, 1, 64, false);
    procs.emplace_back(2, arrival[1], 3, 1, 32, true);
    procs.emplace_back(3, arrival[2], 4, 2, 16, false);

    const int n = (int)procs.size();

    enum VState { V_NEW, V_READY, V_RUNNING, V_WAITING, V_TERMINATED };
    vector<VState> vstate(n, V_NEW);
    queue<int> ready;

    auto allDone = [&]() {
        for (int i = 0; i < n; ++i) if (!procs[i].isComplete()) return false;
        return true;
    };

    int t = 0;
    cout << "\n=== Simulation start ===\n";
    for (int i = 0; i < n; ++i) if (arrival[i] == 0) { ready.push(i); vstate[i] = V_READY; }

    while (!allDone()) {
        for (int i = 0; i < n; ++i) {
            if (vstate[i] == V_NEW && arrival[i] <= t) {
                ready.push(i);
                vstate[i] = V_READY;
            }
        }

        int running = -1;
        if (!ready.empty()) {
            running = ready.front();
            ready.pop();
            vstate[running] = V_RUNNING;
        }

        cout << "\n[Time " << t << "]\n";
        if (running != -1) cout << "RUN:    P" << procs[running].getPid() << "\n";
        else               cout << "RUN:    (idle)\n";

        vector<int> readySnap;
        {
            queue<int> tmp = ready;
            while (!tmp.empty()) { readySnap.push_back(tmp.front()); tmp.pop(); }
        }
        cout << "READY:  ";
        if (readySnap.empty()) cout << "{}\n";
        else {
            cout << "{";
            for (size_t k = 0; k < readySnap.size(); ++k) {
                cout << "P" << procs[readySnap[k]].getPid();
                if (k + 1 < readySnap.size()) cout << ",";
            }
            cout << "}\n";
        }

        cout << "STATE:  ";
        for (int i = 0; i < n; ++i) {
            const char* label =
                vstate[i] == V_NEW        ? "NEW" :
                vstate[i] == V_READY      ? "READY" :
                vstate[i] == V_RUNNING    ? "RUNNING" :
                vstate[i] == V_WAITING    ? "WAITING" : "TERMINATED";
            cout << "P" << procs[i].getPid() << "=" << label << (i + 1 < n ? "  " : "\n");
        }

        if (running != -1) {
            procs[running].decrementRemainingTime(1);
            for (int i = 0; i < n; ++i) {
                if (i == running) continue;
                if (!procs[i].isComplete() && vstate[i] != V_NEW && vstate[i] != V_TERMINATED) {
                    procs[i].incrementWaitingTime(1);
                }
            }
        }

        t += 1;

        if (running != -1) {
            if (procs[running].isComplete()) {
                vstate[running] = V_TERMINATED;
                procs[running].setTurnaroundTime(t - arrival[running]);
            } else {
                bool goIO = false;
                if ((procs[running].getPid() % 2 == 0) && (t % 3 == 0)) goIO = true;
                if (goIO) {
                    vstate[running] = V_WAITING;
                } else {
                    vstate[running] = V_READY;
                    ready.push(running);
                }
            }
        }

        for (int i = 0; i < n; ++i) {
            if (vstate[i] == V_WAITING && !procs[i].isComplete()) {
                vstate[i] = V_READY;
                ready.push(i);
            }
        }
    }

    cout << "\n=== All processes finished at time " << t << " ===\n\n";
    cout << "PID\tState\n";
    for (int i = 0; i < n; ++i) {
        cout << procs[i].getPid() << "\t" << procs[i].getStateName() << "\n";
    }
    cout << "\nDetails:\n";
    for (const auto& p : procs) p.display();

    return 0;
}
