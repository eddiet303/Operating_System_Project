#pragma once
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include "Process.h"

/*
 * Which scheduling algorithm to run.
 * - FCFS : First-Come First-Served (non-preemptive)
 * - SJF  : Shortest Job First (non-preemptive)
 * - SRTF : Shortest Remaining Time First (preemptive SJF)
 */
enum class ScheduleType {
    FCFS,
    SJF,
    SRTF
};

/*
 * A single contiguous execution slice used to build a Gantt-like timeline.
 * - pid      : process id
 * - startTime: inclusive start time of this slice
 * - endTime  : exclusive end time (slice runs on times [startTime, endTime))
 */
struct ExecutionSlice {
    int pid;
    int startTime;
    int endTime;
};

/*
 * Scheduler
 * ---------
 * Single-class static API to run different scheduling algorithms.
 *
 * Usage:
 *   std::vector<Process> procs = ...;   // your processes
 *   auto gantt = Scheduler::run(ScheduleType::FCFS, procs);
 *
 * Notes:
 * - The function will reset each Process' remaining time to its burst time,
 *   then run the chosen algorithm and update each Process' metrics via
 *   Process::setTurnaroundTime(...) and Process::setWaitingTime(...).
 * - The processes vector is passed by reference and WILL be modified (remaining
 *   time and metric setters). If you want to keep the original, pass a copy.
 * - The function returns an ordered vector of ExecutionSlice describing the
 *   timeline (useful for printing a Gantt chart or exporting CSV).
 */
class Scheduler {
public:
    // Run the chosen scheduling method on 'processes'. Returns Gantt slices.
    static std::vector<ExecutionSlice> run(ScheduleType method, std::vector<Process>& processes);
};

#endif // SCHEDULER_H
