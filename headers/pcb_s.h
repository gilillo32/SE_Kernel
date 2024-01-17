#ifndef MY_PCB_H
#define MY_PCB_H

#define PR_UNDEFINED 0
#define PR_EXEC 1
#define PR_FINI 2
#define PR_READY 3

#include <stdint.h>
#include <executor.h>

struct memory_map{
    uint32_t data; // Virtual data address
    uint32_t code; // Virtual code
    uint32_t pgb; // Page table base
};

struct cpu_snapshot{
    uint32_t PC;
    uint32_t IR; // Instruction register
    uint32_t R[CPU_REGISTERS]; // Registers
    uint32_t cc; // Condition code
};

struct pcb_t {
    int pid;
    int current_priority;
    int status;
    struct memory_map memory_map;
    struct cpu_snapshot cpu_snapshot;
};

typedef struct pcb_t pcb_t;

int memory_map_init(struct memory_map * map, uint32_t data, uint32_t code, uint32_t pgb);

void pcb_init(pcb_t * pcb, int pid, int priority);

void pcb_destroy(pcb_t * pcb);

void cpu_snapshot_init(struct cpu_snapshot * snapshot, uint32_t PC);

#endif