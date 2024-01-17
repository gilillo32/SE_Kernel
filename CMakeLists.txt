cmake_minimum_required(VERSION 3.26)
project(Proiektua C)

set(CMAKE_C_STANDARD 11)

include_directories(headers)

file(GLOB SOURCES "src/*.c")

add_executable(Proiektua ${SOURCES}
        headers/runqueue_s.h
        headers/cpu_data_s.h
        headers/processgenerator.h
        headers/sched_prio_fifo.h
        headers/sched_simple_fifo.h
        headers/scheduler_data_interface.h
        headers/scheduler_s.h
        headers/memory_s.h
        headers/executor.h
        headers/loader.h
        src/executor_s.c
        src/loader_s.c
        src/memory_s.c
        src/cpu.c
        src/linkedlist.c
        src/pcb_s.c
        src/runqueues.c
        src/scheduler_priority_fifo.c
        src/scheduler_simple_fifo.c
        src/scheduler.c
        src/scheduler_data_interface.c)