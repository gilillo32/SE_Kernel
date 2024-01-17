#include "cpu_data_s.h"
#include <stdlib.h>

#define TLB_MAX_SPACE 20

int cpu_t_init(struct cpu_s *p_cpu, int core_num, int thread_num_per_core, struct physical_memory *ps){
    if(core_num <= 0 || thread_num_per_core <= 0 || ps == NULL){
        return -1;
    }
    p_cpu->core_kop = core_num;
    struct core_s **my_cores = malloc(sizeof(struct core_s *) * core_num);
    struct core_s *curr_core;
    for(int i = 0; i < core_num; i++){
        curr_core = (struct core_s *)malloc(sizeof(struct core_s));
        core_t_init(curr_core, thread_num_per_core, ps);
        my_cores[i] = curr_core;
    }
    p_cpu->core_arr = my_cores;
    return 0;
}

int core_t_init(struct core_s *p_core, int thread_num, struct physical_memory *ps){
    if(thread_num <= 0 || ps == NULL){
        return -1;
    }
    struct thread_s **my_threads = malloc(sizeof(struct thread_s *) * thread_num);
    struct thread_s *curr_thread;
    my_threads = (struct thread_s **) malloc(thread_num * sizeof(struct thread_s *));
    for(int i = 0; i < thread_num; i++){
        curr_thread = (struct thread_s *)malloc(sizeof(struct thread_s));
        thread_t_init(curr_thread, ps);
        my_threads[i] = curr_thread;
    }
    p_core->thread_kop = thread_num;
    p_core->thread_arr = my_threads;
    return 0;
}

int thread_t_init(struct thread_s *p_hari, struct physical_memory *ps){
    if(ps == NULL){
        return -1;
    }
    p_hari->PC = 0;
    p_hari->IR = 0;
    p_hari->PTBR = 0;
    p_hari->cc = 0;
    p_hari->R[0] = 0;
    p_hari->R[1] = 0;
    p_hari->R[2] = 0;
    p_hari->R[3] = 0;
    p_hari->R[4] = 0;
    p_hari->R[5] = 0;
    p_hari->R[6] = 0;
    p_hari->R[7] = 0;
    p_hari->R[8] = 0;
    p_hari->R[9] = 0;
    p_hari->R[10] = 0;
    p_hari->R[11] = 0;
    p_hari->R[12] = 0;
    p_hari->R[13] = 0;
    p_hari->R[14] = 0;
    p_hari->R[15] = 0;
    struct mmu *my_mmu = malloc(sizeof(struct mmu));
    my_mmu->ps = ps;
    my_mmu->tlb_max_space = TLB_MAX_SPACE;
    my_mmu->tlb_hash_arr = malloc(sizeof(struct pte) * TLB_MAX_SPACE);
    for(int i = 0; i < TLB_MAX_SPACE; i++){
        my_mmu->tlb_hash_arr[i].active = 0;
    }
    p_hari->mem_manag_unit = *my_mmu;
    return 0;
}

int get_hari_kop(struct cpu_s *p_cpu, int *hari_kop){
    if(p_cpu == NULL || hari_kop == NULL){
        return -1;
    }
    int kop = 0;
    for(int i = 0; i < p_cpu->core_kop; i++){
        kop += p_cpu->core_arr[i]->thread_kop;
    }
    *hari_kop = kop;
    return 0;
}