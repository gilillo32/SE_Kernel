//
// Created by gilas on 1/12/23.
//

#ifndef PROIEKTUA_CPU_DATA_S_H
#define PROIEKTUA_CPU_DATA_S_H

#include <stdint.h>
#include "memory_s.h"

#define CPU_REGISTERS 16

struct cpu_s{
    int core_kop;
    struct core_s **core_arr;
};

typedef struct cpu_s cpu_s;

struct core_s{
    int thread_kop;
    struct thread_s **thread_arr;
};

struct thread_s{
    uint32_t PC;    // Program Counter
    uint32_t IR;    // Instruction Register
    uint32_t PTBR;  // Page Table Base Register

    int32_t R[CPU_REGISTERS];
    int32_t cc;
    struct mmu mem_manag_unit;
};

typedef struct thread_s thread_s;

int cpu_t_init(struct cpu_s *p_cpu, int core_num, int thread_num_per_core, struct physical_memory *ps);

int core_t_init(struct core_s *p_core, int hari_num, struct physical_memory *ps);

int thread_t_init(struct thread_s *p_hari, struct physical_memory *ps);

int get_hari_kop(struct cpu_s *p_cpu, int *hari_kop);

#endif //PROIEKTUA_CPU_DATA_S_H
