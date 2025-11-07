// Scheduler.cpp
#include "scheduler.h"
#include <algorithm>
#include <queue>
#include <vector>
#include <unordered_map>

using namespace std;

// merge contiguous slices of same pid
static vector<ExecutionSlice> mergeSlices(const vector<ExecutionSlice>& s) {
    if (s.empty()) return {};
    vector<ExecutionSlice> out;
    out.push_back(s[0]);
    for (size_t i = 1; i < s.size(); ++i) {
        const auto &cur = s[i];
        auto &last = out.back();
        if (last.pid == cur.pid && last.endTime == cur.startTime) last.endTime = cur.endTime;
        else out.push_back(cur);
    }
    return out;
}

// fill turnaround and waiting from gantt (last end time)
static void fillMetrics(vector<Process>& procs, const vector<ExecutionSlice>& gantt) {
    unordered_map<int,int> lastEnd;
    for (const auto &slice : gantt) lastEnd[slice.pid] = slice.endTime;
    for (auto &p : procs) {
        int pid = p.getPid();
        if (lastEnd.find(pid) != lastEnd.end()) {
            int completion = lastEnd[pid];
            int turnaround = completion - p.getArrivalTime();
            if (turnaround < 0) turnaround = 0;
            int waiting = turnaround - p.getBurstTime();
            if (waiting < 0) waiting = 0;
            p.setTurnaroundTime(turnaround);
            p.setWaitingTime(waiting);
            p.setRemainingTime(0);
        } else {
            p.setTurnaroundTime(0);
            p.setWaitingTime(0);
        }
    }
}

// FCFS
static vector<ExecutionSlice> runFCFS(vector<Process>& procs) {
    vector<Process> list = procs;
    sort(list.begin(), list.end(), [](const Process& a, const Process& b) {
        if (a.getArrivalTime() != b.getArrivalTime()) return a.getArrivalTime() < b.getArrivalTime();
        return a.getPid() < b.getPid();
    });

    vector<ExecutionSlice> slices;
    int time = 0;
    for (const auto &p : list) {
        int start = max(time, p.getArrivalTime());
        int end = start + p.getBurstTime();
        slices.push_back({p.getPid(), start, end});
        time = end;
    }

    auto gantt = mergeSlices(slices);
    fillMetrics(procs, gantt);
    return gantt;
}

// SJF (non-preemptive)
static vector<ExecutionSlice> runSJF(vector<Process>& procs) {
    vector<Process> list = procs;
    sort(list.begin(), list.end(), [](const Process& a, const Process& b){
        if (a.getArrivalTime() != b.getArrivalTime()) return a.getArrivalTime() < b.getArrivalTime();
        return a.getPid() < b.getPid();
    });

    auto cmp = [](const Process& a, const Process& b){
        if (a.getBurstTime() != b.getBurstTime()) return a.getBurstTime() > b.getBurstTime();
        if (a.getArrivalTime() != b.getArrivalTime()) return a.getArrivalTime() > b.getArrivalTime();
        return a.getPid() > b.getPid();
    };
    priority_queue<Process, vector<Process>, decltype(cmp)> pq(cmp);

    vector<ExecutionSlice> slices;
    int time = 0;
    size_t idx = 0;
    while (idx < list.size() || !pq.empty()) {
        if (pq.empty()) {
            time = max(time, list[idx].getArrivalTime());
            while (idx < list.size() && list[idx].getArrivalTime() <= time) {
                pq.push(list[idx]); ++idx;
            }
        }
        if (!pq.empty()) {
            Process p = pq.top(); pq.pop();
            int start = max(time, p.getArrivalTime());
            int end = start + p.getBurstTime();
            slices.push_back({p.getPid(), start, end});
            time = end;
            while (idx < list.size() && list[idx].getArrivalTime() <= time) {
                pq.push(list[idx]); ++idx;
            }
        }
    }

    auto gantt = mergeSlices(slices);
    fillMetrics(procs, gantt);
    return gantt;
}

// SRTF (preemptive, unit-time simulation)
static vector<ExecutionSlice> runSRTF(vector<Process>& procs) {
    vector<Process> list = procs;
    sort(list.begin(), list.end(), [](const Process& a, const Process& b){
        if (a.getArrivalTime() != b.getArrivalTime()) return a.getArrivalTime() < b.getArrivalTime();
        return a.getPid() < b.getPid();
    });

    unordered_map<int,int> remaining;
    unordered_map<int,int> arrival;
    for (const auto &p : list) { remaining[p.getPid()] = p.getBurstTime(); arrival[p.getPid()] = p.getArrivalTime(); }

    struct Item { int rem; int art; int pid; };
    auto cmp = [](const Item& a, const Item& b){
        if (a.rem != b.rem) return a.rem > b.rem;
        if (a.art != b.art) return a.art > b.art;
        return a.pid > b.pid;
    };
    priority_queue<Item, vector<Item>, decltype(cmp)> pq(cmp);

    vector<ExecutionSlice> raw;
    int time = 0;
    size_t idx = 0;

    while (idx < list.size() || !pq.empty()) {
        if (pq.empty()) {
            time = max(time, list[idx].getArrivalTime());
            while (idx < list.size() && list[idx].getArrivalTime() <= time) {
                int pid = list[idx].getPid();
                pq.push({ remaining[pid], list[idx].getArrivalTime(), pid });
                ++idx;
            }
        }
        if (!pq.empty()) {
            Item cur = pq.top(); pq.pop();
            int pid = cur.pid;
            int start = time;
            int end = start + 1;
            raw.push_back({pid, start, end});
            time = end;
            remaining[pid] -= 1;
            while (idx < list.size() && list[idx].getArrivalTime() <= time) {
                int npid = list[idx].getPid();
                pq.push({ remaining[npid], list[idx].getArrivalTime(), npid });
                ++idx;
            }
            if (remaining[pid] > 0) pq.push({ remaining[pid], arrival[pid], pid });
        }
    }

    auto gantt = mergeSlices(raw);
    fillMetrics(procs, gantt);
    return gantt;
}

// public dispatcher
vector<ExecutionSlice> Scheduler::run(ScheduleType method, vector<Process>& processes) {
    for (auto &p : processes) p.setRemainingTime(p.getBurstTime()), p.setTurnaroundTime(0), p.setWaitingTime(0);

    if (method == ScheduleType::FCFS) return runFCFS(processes);
    if (method == ScheduleType::SJF)  return runSJF(processes);
    return runSRTF(processes);
}
