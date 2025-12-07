# Operating Process System Simulator

## Project Overview

* This project is a simplified operating system simulator with focus on the process state written in C++ that focuses on simulating a boot-up process, and implemeting a authentication system. Also is include a class (Process) that represent a class process. lastly the project is simulate the behavior of a virtual memory responsible for translating the "virtual" addresses used by a program into "physical" addresses in RAM, handling the illusion of large memory availability through paging.
* CSCE4326 - Operating Systems  
* Instructor: Dr. Elvira Teran 
(Fall 2025)

# Team Members 
* Eduardo Torres
* Marlon Chavez
* Jose Santos

## Code Explanation
The simulator is written in C++ and is divided into three main files: 
- `main.cpp` contains the entry point of the program. It displays a boot message, asks the user for a username and password, and allows up to three login attempts. It displays a simulated process with PIDs and their different states, as well as the different timings.
- `auth.h` declares the authentication function so it can be used in other files.
- `auth.cpp` implements the authentication logic, comparing the input username and password against hardcoded valid credentials. If they match, access is granted; otherwise, the user is prompted to try again. This simple structure demonstrates how an operating system might perform a basic boot-up sequence and user login process.
- `Process.h` Here is where the declaration of the Process class It defines an enum called ProcessState to represent the different states a process can be in NEW, READY, RUNNING, WAITING, and TERMINATED. stores process attributes; ID, arrival time, CPU burst time, priority, remaining execution time, memory requirements, and whether it performs I/O operations. declares a set of getters and setters to safely access and modify these attributes. decrementRemainingTime() and incrementWaitingTime() simulate how a process uses CPU time or waits in the ready queue and the display() function provides a clear text-based summary of the process’s current state and timing information.
- `Process.cpp` implements the Process class used in a CPU scheduling simulation. It includes the constructor, getter, and setter methods, and utility functions that manage key attributes such as process ID, arrival time, burst time, priority, memory requirements, and execution state. It also handles timing metrics, such as remaining time, waiting time, and turnaround time, as well as determining whether a process requires I/O operations. It also displays all relevant process information in a structured and readable format.
- `scheduler.cpp` The Scheduler.cpp file contains the actual implementation of the scheduling algorithms declared in the header. FCFS schedules based on the order of process arrival, SJF selects the process with the shortest CPU burst time, and SRTF preempts the currently running process if the remaining time of another process is less than the currently scheduled process. The run() function simulates time passing, changes process states, and keeps record of execution slices that show how long each process runs.. This file focuses only on scheduling behavior, keeping the logic separated from user login, process creation, and display functions, making the code easier to read and maintain.
- `scheduler.h` The Scheduler.h file defines the scheduling system used in the simulator. It declares an enumeration for the supported algorithms: FCFS, SJF, and SRTF. It also defines the ExecutionSlice structure, which stores when and which process is running, allowing the final output to show the CPU timeline. The main Scheduler class is declared here, along with the run() function that receives a list of processes and applies the selected scheduling algorithm. Keeping this logic in a separate header organizes the project, makes the main program cleaner, and allows additional scheduling methods to be added more easily in the future.
- `vm.cpp` The header file says what the core logic of the virtual memory system is, and the vm.cpp file does that. It takes care of the complicated mechanics of address translation, simulating hardware actions like TLB lookups and page faults when data isn't in physical RAM. This file has the rules for giving out free frames and moving old pages to the backing store when memory is full, following the chosen replacement policy exactly. It also has utility functions that let users see the memory state and get performance statistics like hit/miss ratios, so they can see how well the memory management simulation is working in real time.
- `vm.h` The vm.h header file has the VirtualMemory class, which acts like a computer's Memory Management Unit (MMU). It lists the basic data structures that paging needs, such as a Page Table for mapping virtual page numbers (VPNs) to physical frames, a Translation Lookaside Buffer (TLB) for storing recent translations, and metadata structures for keeping track of frame usage. The class interface lets you read and write memory, change addresses, and manage the lifecycle of memory pages. It also lets you set system parameters like page size and replacement policies like LRU (Least Recently Used) or CLOCK.
## Default Credentials
- Username: `Administrator`
- Password: `Hello123#`
