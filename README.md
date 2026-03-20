# 🐧 Operating Systems (Systemy Operacyjne) - AGH University

> Laboratory projects and assignments for the **Operating Systems** (Systemy Operacyjne) course at AGH University. 

This repository contains low-level system programming exercises written in **C**. It covers the fundamentals of the POSIX API, process and thread management, Inter-Process Communication (IPC), synchronization mechanisms, and network programming.

## 👨‍💻 Author
* **Kajetan Frątczak**

---

## 🚀 Technologies & Concepts
* **Language:** C
* **Environment:** Linux / POSIX
* **Build Tools:** Make (Makefiles), CMake
* **Key Concepts:** * Process Control (`fork`, `exec`, `wait`)
  * Signals
  * Inter-Process Communication (Pipes, FIFOs, Message Queues)
  * Shared Memory & Semaphores
  * Multithreading (`pthreads`)
  * Network Sockets (TCP/UDP)

---

## 📁 Repository Structure

### 🔹 `lab1/` & `lab2/` - File IO & Process Basics
* Introduction to standard C library and POSIX system calls for file manipulation.
* Implementations of basic mathematical programs (e.g., Collatz conjecture).

### 🔹 `lab3/` - Process Control
* Creating and managing child processes using `fork()` and `exec()`.
* File/Image manipulation using multiple processes (`flipper.c`).

### 🔹 `lab4/` & `lab5/` - Signals & Pipes
* Handling POSIX signals (`sigaction`, `kill`).
* Implementing sender-catcher architectures.
* Communication between processes using anonymous pipes.

### 🔹 `lab6/` & `lab7/` - Advanced IPC (Message Queues)
* Inter-Process Communication using System V and POSIX Message Queues.
* Implementing Client-Server architectures communicating locally.

### 🔹 `lab8/` - Synchronization (Semaphores & Shared Memory)
* Solving classic synchronization problems (e.g., the Printer Spooler problem).
* Utilizing shared memory and semaphores to prevent race conditions between `user.c` and `printer.c`.

### 🔹 `lab9/` & `lab10/` - POSIX Threads (pthreads)
* Multithreaded programming in C.
* Thread creation, joining, and synchronization using mutexes and condition variables.

### 🔹 `lab11/` & `lab12/` - Network Sockets
* Network programming using POSIX sockets.
* Implementation of local and remote Client-Server communication (TCP / UDP).

---

## ⚙️ How to build and run

Each laboratory folder contains its own `Makefile`. To compile the programs for a specific lab, navigate to its directory and run `make`.

```bash
cd lab8
make
./user  # (or the respective executable name)