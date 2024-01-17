#include "pcb_s.h"
#include "processgenerator.h"
#include <stdio.h>
#include <stdlib.h>

void process_generator_init(pr_generator_s * prgen, sched_basic_t * sched, int MAX_PID, int MIN_PID, int MAX_PRIO){
    prgen -> scheduler = sched;
    prgen -> MAX_PID = MAX_PID;
    prgen -> MIN_PID = MIN_PID;
    prgen -> max_prio = MAX_PRIO;
    prgen -> current_counter = 0;
}

int prgen_get_valid_pid(pr_generator_s * pr_gen){
    int new_pid;
    new_pid = pr_gen -> current_counter++;
    if(pr_gen -> current_counter < 0)
        pr_gen -> current_counter = 1;
    return new_pid;
}


int prgen_get_valid_priority(pr_generator_s * pr_gen){
    return rand() % pr_gen -> max_prio;
}

void prgenerator_generate_process(pr_generator_s * pr_gen){
    int new_pid = prgen_get_valid_pid(pr_gen);
    int new_prio = prgen_get_valid_priority(pr_gen);
    pcb_t * new_pcb = malloc(sizeof(pcb_t));
    if(new_pcb == NULL){
        printf("Error: malloc failed in prgenerator_generate_process\n");
        exit(1);
    }
    pcb_init(new_pcb, new_pid, new_prio);
    sched_add_to_process_queue(pr_gen -> scheduler, new_pcb);
}
