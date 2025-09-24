# Operating System Simulator

## Project Overview

* This project is a simplified operating system simulator written in C++ that focuses on simulating a boot-up process, and implemeting a authentication system.
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

## Default Credentials
- Username: `Administrator`
- Password: `Hello123#`
