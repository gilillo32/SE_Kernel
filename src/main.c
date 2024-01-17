#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include "pcb_s.h"
#include "loader.h"
#include "scheduler_s.h"
#include "linkedlist_s.h"
#include "processgenerator.h"
#include "clock_s.h"
#include "executor.h"
#include "memory_s.h"


#define TIMER_KOP 3
#define SCHEDULER_FREQ 2
#define PROCESS_GENERATOR_FREQ 4
#define EGOERA_PRINT_FREQ 1
#define PRGEN_MAX_PID 1
#define PRGEN_MIN_PID 1000
#define CPU_KOP 2
#define CORE_PER_CPU 1
#define THREAD_PER_CORE 1
#define CORE_TOTAL CPU_KOP * CORE_PER_CPU
#define THREAD_TOTAL CORE_TOTAL * THREAD_PER_CORE

struct clk_clock_s  clk_main;

int main(int argc, char *argv[]){
    clk_timer_s timer_scheduler;
    clk_timer_s timer_loader;
    clk_timer_s debug_timer;
    clk_timer_s timer_executor;
    pthread_t thread_clock, thread_scheduler, thread_loader, thread_debug, thread_executor;

    loader_s loader;
    sched_basic_t scheduler;
    struct physical_memory physical_memory;
    int policy = 0;
    if(argc >= 2 && strspn(argv[1], "0123456789") == strlen(argv[1])){
        sscanf(argv[1], "%d", &policy);
    }
    printf("Scheduler policy:\n");
    switch(policy){
        case SCHED_FIFO:
            printf("FIFO\n");
            break;
        case SCHED_RR:
            printf("RR\n");
            break;
        case SCHED_PRIO_FIFO:
            printf("PRIO FIFO\n");
            break;
        case SCHED_PRIO_RR:
            printf("PRIO RR\n");
            break;
        default:
            printf("FIFO\n");
            policy = SCHED_FIFO;
            break;
    }
    loader_init(&loader, &physical_memory, &scheduler, MAX_PRIO);
    pmemo_init(&physical_memory);
    struct cpu_s cpu_array[CPU_KOP];
    for(int i = 0; i < CPU_KOP; i++){
        cpu_t_init(&cpu_array[i], CORE_PER_CPU, THREAD_PER_CORE, *physical_memory);
    }
    executor_t executor;
    executor_init(&executor, cpu_array, CPU_KOP);
    clk_clock_init(&clk_main);
    clk_timer_init(&timer_scheduler, &clk_main, SCHEDULER_FREQ, &sched_execute, &scheduler, "Sched");
    clk_timer_init(&timer_loader, &clk_main, PROCESS_GENERATOR_FREQ, (DoWhenTick) &loader_load_next_process, &loader, "Loader");
    clk_timer_init(&timer_executor, &clk_main, 1, (DoWhenTick) &executor_exec, &executor, "Executor");
    clk_timer_init(&debug_timer, &clk_main, EGOERA_PRINT_FREQ, (DoWhenTick) &sched_print_sched_status, &loader, "Egoera");
    sched_init(&scheduler, &timer_scheduler, cpu_array, CPU_KOP, policy);

    if(pthread_create(&thread_scheduler, NULL, clk_timer_start, (void *) &timer_scheduler)){
        printf("Error creating thread scheduler\n");
        exit(1);
    }
    if(pthread_create(&thread_loader, NULL, clk_timer_start, (void *) &timer_loader)){
        printf("Error creating thread loader\n");
        exit(1);
    }
    if(pthread_create(&thread_executor, NULL, clk_timer_start, (void *) &timer_executor)){
        printf("Error creating thread executor\n");
        exit(1);
    }
    if(pthread_create(&thread_debug, NULL, clk_timer_start, (void *) &debug_timer)){
        printf("Error creating thread debug\n");
        exit(1);
    }
    if(pthread_create(&thread_clock, NULL, clk_clock_start, (void *) &clk_main)){
        printf("Error creating thread clock\n");
        exit(1);
    }

    pthread_join(thread_clock, NULL);
    exit(0);
}