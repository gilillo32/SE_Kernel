//
// Created by gilas on 1/12/23.
//

#ifndef PROIEKTUA_LOADER_H
#define PROIEKTUA_LOADER_H

#include "linkedlist_s.h"
#include "pcb_s.h"
#include "memory_s.h"
#include "scheduler_s.h"

#define LOAD_DIR "./data/programlist/"

typedef struct loader_s{
    linkedlist_structure unloaded_elf_list;
    int prid_counter;
    int max_prio;
    struct mmu mmu;
    sched_basic_t *scheduler;
} loader_s;

int __loader_get_valid_priority(loader_s *loader);
int __loader_get_valid_pid(loader_s *loader);
int loader_init(loader_s *loader, struct physical_memory *phys_mem, sched_basic_t *scheduler, int max_prio);
int loader_load_next_process(loader_s *loader);

int __load_file(struct mmu *p_mmu, char *d_name, struct pcb_t *pcb);
int load_null_pcb(struct mmu *p_mmu, struct pcb_t *pcb);
int __hex_string_to_int32(char *hex_string, uint32_t *result);
int __hex_char_to_int4(char hex_char, uint8_t *result);

#endif //PROIEKTUA_LOADER_H

