# Systemy-operacyjne---Zadania
============================================================================
Author: Natan Wojnowski
Location: Kraków, Poland
============================================================================

* Project Overview
==================

This repository contains a collection of operating systems and systems programming
exercises implemented in C. The projects cover core UNIX/Linux concepts such as:

- Process creation and management (fork, exec)
- Signal handling
- Inter-process communication (pipes, FIFOs, message queues)
- Shared memory
- Semaphores and synchronization
- Multithreading (pthreads, mutexes)
- Dynamic and static libraries
- Client-server communication models

Each folder corresponds to a separate exercise and is self-contained.

---

* Repository Structure
======================

Each exercise is located in its own directory:

Zad_1/
Zad_2/
...
Zad_10/

Each folder typically contains:
- source code (.c files)
- Makefile
- local README describing the task and implementation

---

* How to build and run
======================

Each exercise is independent and uses its own Makefile.

General workflow:

1. Enter a selected directory:
   cd Zad_X

2. Compile the program:
   make

3. Run the program:
   make run
   (or a task-specific command such as make run_a, make run_b, etc.)

4. Clean build files:
   make clean

Some exercises may include additional targets such as:
- make check
- make run_static
- make run_dynamic
- make run_a / make run_b

Refer to the local README inside each folder for exact commands.

---

* Topics covered
=================

1. Processes
   - fork(), exec(), wait()
   - process hierarchies
   - orphan and zombie processes
   - process groups

2. Signals
   - signal handling (signal, sigaction)
   - signal masking
   - inter-process communication via signals

3. Pipes and FIFOs
   - unnamed pipes
   - named pipes (FIFO)
   - producer-consumer patterns

4. Message Queues
   - POSIX message queues
   - client-server communication

5. Shared Memory
   - mmap-based shared memory
   - synchronization with semaphores

6. Synchronization
   - POSIX semaphores
   - mutexes (pthread)
   - race condition prevention

7. Threads
   - pthread creation and joining
   - critical sections
   - mutex synchronization

8. Libraries
   - static libraries
   - shared libraries
   - RPATH vs LD_LIBRARY_PATH

---

* Notes
=======

- Each program is designed for Linux/UNIX environments.
- Many exercises require POSIX-compatible system calls.
- Timing (sleep) is used in some tasks for demonstration purposes.
- Proper synchronization is required to ensure correct behavior.

---

* Purpose of the project
========================

The goal of this repository is to demonstrate understanding of fundamental
operating systems concepts through practical implementation in C, including:

- process control
- concurrency
- synchronization
- inter-process communication
- system-level programming patterns

============================================================================
