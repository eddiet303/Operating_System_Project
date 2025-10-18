# Operating Process System Simulator

## Project Overview

* This project is a simplified operating system simulator with focus on the process state written in C++ that focuses on simulating a boot-up process, and implemeting a authentication system. Also is include a class (Process) that represent a class process.
* CSCE4326 - Operating Systems  
* Instructor: Dr. Elvira Teran 
(Fall 2025)

# Team Members 
* Eduardo Torres
* Marlon Chavez
* Jose Santos

## Code Explanation
The simulator is written in C++ and is divided into three main files: 
- `main.cpp` contains the entry point of the program. It displays a boot message, asks the user for a username and password, and allows up to three login attempts.
- `auth.h` declares the authentication function so it can be used in other files.
- `auth.cpp` implements the authentication logic, comparing the input username and password against hardcoded valid credentials. If they match, access is granted; otherwise, the user is prompted to try again. This simple structure demonstrates how an operating system might perform a basic boot-up sequence and user login process.
- `Process.h` Here is where the declaration of the Process class It defines an enum called ProcessState to represent the different states a process can be in NEW, READY, RUNNING, WAITING, and TERMINATED. stores process attributes; ID, arrival time, CPU burst time, priority, remaining execution time, memory requirements, and whether it performs I/O operations. declares a set of getters and setters to safely access and modify these attributes. decrementRemainingTime() and incrementWaitingTime() simulate how a process uses CPU time or waits in the ready queue and the display() function provides a clear text-based summary of the process’s current state and timing information.
- `Process.cpp ' implements the Process class used in a CPU scheduling simulation. It includes the constructor, getter, and setter methods, and utility functions that manage key attributes such as process ID, arrival time, burst time, priority, memory requirements, and execution state. It also handles timing metrics, such as remaining time, waiting time, and turnaround time, as well as determining whether a process requires I/O operations. It also displays all relevant process information in a structured and readable format.

## Default Credentials
- Username: `Administrator`
- Password: `Hello123#`
