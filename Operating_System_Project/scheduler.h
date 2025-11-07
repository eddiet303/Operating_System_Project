#pragma once
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include "Process.h"

/*
 Which scheduling algorithm to run.
 - FCFS : First-Come First-Served (non-preemptive)
 - SJF  : Shortest Job First (non-preemptive)
 - SRTF : Shortest Remaining Time First (preemptive SJF)
 */
enum class ScheduleType {
    FCFS,
    SJF,
    SRTF
};

/*
 A single contiguous execution slice used to build a Gantt-like timeline.
 - pid      : process id
 - startTime: inclusive start time of this slice
 - endTime  : exclusive end time (slice runs on times [startTime, endTime))
 */
struct ExecutionSlice {
    int pid;
    int startTime;
    int endTime;
};

/*
 Scheduler

 */
class Scheduler {
public:
    // Run the chosen scheduling method on 'processes'. Returns Gantt slices.
    static std::vector<ExecutionSlice> run(ScheduleType method, std::vector<Process>& processes);
};

#endif // SCHEDULER_H
