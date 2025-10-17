//Put the class declaration in a header file (Process.h) 
//and implementation / usage in .cpp (e.g., main.cpp).
// ==================== Process.h ====================
#ifndef PROCESS_H
#define PROCESS_H
#include <string>
#include <vector>

enum ProcessState { //same as powerpoint
    NEW,
    RUNNING,
    WAITING,
    READY,
    TERMINATED
};

class Process {
private: //Variants to stay
    int pid; //pid (Process ID) – A unique identifier for the process.
    int arrival_time; //arrival_time – The time at which the process enters the system.
    int burst_time; //burst_time – The total CPU execution time required by the process.
    int priority; //priority – (If applicable) The priority level of the process
    ProcessState state; //enum ProcessState
    int remaining_time; //remaining_time – The amount of CPU time left for completion
    int waiting_time; //waiting_time – The total time the process has spent waiting in the ready queue.
    int turnaround_time; // – The total time from arrival to completion.
    int memory_required; //– The amount of memory required by the process
    bool io_operations; // A list or flag indicating if the process has I/O operations

public:
// Constructor valid variant
    Process(int p_id, int arr_time, int b_time, int prio = 0, int mem = 0, bool io = false);

    // Getters
    int getPid() const;
    int getArrivalTime() const;
    int getBurstTime() const;
    int getPriority() const;
    ProcessState getState() const;
    int getRemainingTime() const;
    int getWaitingTime() const;
    int getTurnaroundTime() const;
    int getMemoryRequired() const;
    bool hasIoOperations() const;

    // Setters
    void setState(ProcessState s);
    void setRemainingTime(int t);
    void setWaitingTime(int t);
    void setTurnaroundTime(int t);

    // Utility methods
    void decrementRemainingTime(int time = 1);
    void incrementWaitingTime(int time = 1);
    bool isComplete() const;
    
    // Display
    void display() const;
    std::string getStateName() const;
};

#endif // PROCESS_H
