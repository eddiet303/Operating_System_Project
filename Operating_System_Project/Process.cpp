//#include <iostream>
//#include "Process.h"

// Constructor: initialize all process attributes
#include "Process.h"
#include <iostream>
#include <iomanip>

// Constructor
Process::Process(int p_id, int arr_time, int b_time, int prio, int mem, bool io)
    : pid(p_id), arrival_time(arr_time), burst_time(b_time), priority(prio),
      state(NEW), remaining_time(b_time), waiting_time(0), turnaround_time(0),
      memory_required(mem), io_operations(io) {
}

// Getters
int Process::getPid() const {
    return pid;
}

int Process::getArrivalTime() const {
    return arrival_time;
}

int Process::getBurstTime() const {
    return burst_time;
}

int Process::getPriority() const {
    return priority;
}

ProcessState Process::getState() const {
    return state;
}

int Process::getRemainingTime() const {
    return remaining_time;
}

int Process::getWaitingTime() const {
    return waiting_time;
}

int Process::getTurnaroundTime() const {
    return turnaround_time;
}

int Process::getMemoryRequired() const {
    return memory_required;
}

bool Process::hasIoOperations() const {
    return io_operations;
}

// Setters
void Process::setState(ProcessState s) {
    state = s;
}

void Process::setRemainingTime(int t) {
    remaining_time = t;
}

void Process::setWaitingTime(int t) {
    waiting_time = t;
}

void Process::setTurnaroundTime(int t) {
    turnaround_time = t;
}

// Utility methods
void Process::decrementRemainingTime(int time) {
    remaining_time -= time;
    if (remaining_time < 0) {
        remaining_time = 0;
    }
}

void Process::incrementWaitingTime(int time) {
    waiting_time += time;
}

bool Process::isComplete() const {
    return remaining_time <= 0;
}

// Display methods
std::string Process::getStateName() const {
    switch (state) {
        case NEW: return "NEW";
        case RUNNING: return "RUNNING";
        case WAITING: return "WAITING";
        case READY: return "READY";
        case TERMINATED: return "TERMINATED";
        default: return "UNKNOWN";
    }
}

void Process::display() const {
    std::cout << "Process ID: " << pid << std::endl;
    std::cout << "Arrival Time: " << arrival_time << std::endl;
    std::cout << "Burst Time: " << burst_time << std::endl;
    std::cout << "Priority: " << priority << std::endl;
    std::cout << "State: " << getStateName() << std::endl;
    std::cout << "Remaining Time: " << remaining_time << std::endl;
    std::cout << "Waiting Time: " << waiting_time << std::endl;
    std::cout << "Turnaround Time: " << turnaround_time << std::endl;
    std::cout << "Memory Required: " << memory_required << std::endl;
    std::cout << "Has I/O Operations: " << (io_operations ? "Yes" : "No") << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}
