#include <stdlib.h>
#include "processgenerator.h"
#include "loader.h"
#include <err.h>
#include "executor.h"
#include <stdio.h>
#include <pthread.h>
#include "linkedlist_s.h"
#include "pcb_s.h"
#include "scheduler_s.h"
#include <assert.h>


#define SCHEDULER_NULL_PROCESS_RR_FREQ 4
#define SCHEDULER_BASE_PROCESS_RR_FREQ 10

void sched_add_to_process_queue(sched_basic_t *sched, pcb_t *new_pcb){
    pthread_mutex_lock(sched->sched_mutex_list);
    new_pcb->status = PR_READY;
    i_sched_insert(&sched->scheduler_queue_i_object, new_pcb);
    pthread_mutex_unlock(sched->sched_mutex_list);
}

void sched_execute(sched_basic_t *sched){
    for(int i = 0; i < sched->core_control_array_size; i++){
        struct sched_cpu_thread_control *core_control = &sched->core_control_array[i];
        __sched_update_running_pcb(core_control);
        if(SCHED_PRIO_RR == sched->current_policy || SCHED_RR == sched->current_policy){
            if(core_control->current_process_quantum <= 0){
                sched_dispatcher(sched, core_control);
            }
            else{
                core_control->current_process_quantum -= sched->timer->timer_tick_freq;
            }
        }
        else if(core_control->currrent_cpu_sched_status == NULL_PROCESS_RUN || core_control->current_process->status == PR_FINI){
            sched_dispatcher(sched, core_control);
        }
    }
}

void sched_dispatcher(sched_basic_t *sched, struct sched_cpu_thread_control *thread_control){
    pcb_t *next_process, *old_process;
    next_process = __sched_get_next_process_clean_queue(sched, thread_control);
    old_process = thread_control->current_process;
    if(next_process == NULL){
        if(old_process != &sched->null_process && old_process->status == PR_FINI){
            next_process = &sched->null_process;
            thread_control->currrent_cpu_sched_status = SCHEDULER_NULL_PROCESS_RR_FREQ;
        }
    }
    else{
        thread_control->current_process_quantum = SCHEDULER_BASE_PROCESS_RR_FREQ;
    }
    if(next_process != NULL) {
        thread_control->current_process = next_process;
        disp_dispatch(old_process, next_process, thread_control->thread_s);
        if(thread_control->currrent_cpu_sched_status != NULL_PROCESS_RUN){
            if(old_process->status == PR_FINI){
                disp_terminate(thread_control->thread_s, old_process);
            }
            else{
                sched_add_to_process_queue(sched, old_process);
                old_process->status = PR_READY;
            }
        }
    }
    if(next_process != NULL){
        if(next_process == &sched->null_process){
            thread_control->currrent_cpu_sched_status = NULL_PROCESS_RUN;
        }
        else{
            thread_control->currrent_cpu_sched_status = PROCESS_RUN;
        }
    }
}

int __sched_update_running_pcb(struct sched_cpu_thread_control *thread_control){
    pcb_t *pcb = thread_control->current_process;
    if(pcb != NULL){
        if(pcb->status == PR_FINI){
            thread_control->current_process = NULL;
            return 1;
        }
    }
    return 0;
}

pcb_t *__sched_get_next_process_clean_queue(sched_basic_t *sched, struct sched_cpu_thread_control * thread_control){
    pcb_t *next_process = NULL;
    if(thread_control->currrent_cpu_sched_status == NULL_PROCESS_RUN){
        next_process = &sched->null_process;
    }
    else{
        next_process = __sched_get_next_process(sched);
    }
    return next_process;
}

int sched_cpu_thread_control_init(struct sched_cpu_thread_control *thread_control, thread_s *thread_s, pcb_t *current_process, int current_process_quantum, int currrent_cpu_sched_status){
    thread_control->thread_s = thread_s;
    thread_control->current_process = current_process;
    thread_control->current_process_quantum = current_process_quantum;
    thread_control->currrent_cpu_sched_status = currrent_cpu_sched_status;
    return 0;
}

pcb_t *__sched_get_next_process(sched_basic_t *sched){
    pcb_t *next_process = NULL;
    if(sched->current_policy == SCHED_FIFO){
        next_process = i_sched_peek(&sched->scheduler_queue_i_object);
    }
    else if(sched->current_policy == SCHED_PRIO_FIFO){
        next_process = i_sched_peek(&sched->scheduler_queue_i_object);
    }
    else if(sched->current_policy == SCHED_RR){
        next_process = i_sched_peek(&sched->scheduler_queue_i_object);
    }
    else if(sched->current_policy == SCHED_PRIO_RR){
        next_process = i_sched_peek(&sched->scheduler_queue_i_object);
    }
    return next_process;
}

int sched_queue_is_empty(sched_basic_t *sched){
    return i_sched_peek(&sched->scheduler_queue_i_object) == NULL;
}

void disp_dispatch(pcb_t *old_pcb, pcb_t *new_pcb, struct thread_s *thread_s){
    if(old_pcb != NULL){
        __disp_unload(thread_s, old_pcb);
    }
    __disp_load(thread_s, new_pcb);
}

void disp_terminate(struct thread_s *thread_s, pcb_t *pcb){
    __disp_unload(thread_s, pcb);
    pcb_destroy(pcb);
}

void sched_init(sched_basic_t *sched, clk_timer_s *timer, struct cpu_s cpu_array[], int cpu_kop, int policy) {
    sched->sched_mutex_list = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(sched->sched_mutex_list, NULL);
    pcb_init(&sched->null_process, -1, 0);
    load_null_pcb(&cpu_array[0].core_arr[0]->thread_arr[0]->mem_manag_unit, &sched->null_process);
    sched->current_policy = policy;
    i_sched_configure(&sched->scheduler_queue_i_object, policy);
    sched->timer = timer;
    sched_cpu_thread_control_init((struct sched_cpu_thread_control *) &sched->core_control_array[0], cpu_array[0].core_arr[0]->thread_arr[0], NULL, 0, NULL_PROCESS_RUN);
}

void __disp_load(struct thread_s *thread_s, pcb_t *pcb){
    thread_s->PTBR = pcb->memory_map.pgb;
    thread_s->IR = pcb->cpu_snapshot.IR;
    thread_s->PC = pcb->cpu_snapshot.PC;
    thread_s->cc = pcb->cpu_snapshot.cc;
    for(int i = 0; i < CPU_REGISTERS; i++){
        thread_s->R[i] = pcb->cpu_snapshot.R[i];
    }
    pcb->status = PR_EXEC;
}

void __disp_unload(struct thread_s *thread_s, pcb_t *pcb){
    pcb->cpu_snapshot.IR = thread_s->IR;
    pcb->cpu_snapshot.PC = thread_s->PC;
    pcb->cpu_snapshot.cc = thread_s->cc;
    for(int i = 0; i < CPU_REGISTERS; i++){
        pcb->cpu_snapshot.R[i] = thread_s->R[i];
    }
    pcb->status = PR_FINI;
}

void __sched_thread_control_array_init(sched_basic_t *sched, struct cpu_s my_cpu_arr[], int cpu_kop){
    int cpu_thread_count = 0;
    int current_thread_count;
    for(int i = 0; i < cpu_kop; i++){
        get_hari_kop(&my_cpu_arr[i], &current_thread_count);
        cpu_thread_count += current_thread_count;
    }
    sched->core_control_array_size = cpu_thread_count;
    sched->core_control_array = malloc(sizeof(struct sched_cpu_thread_control) * cpu_thread_count);
    int next_thread_control_index = 0;
    for(int i = 0; i < cpu_kop; i++){
        for(int j = 0; j < my_cpu_arr[i].core_kop; j++){
            for(int k = 0; k < my_cpu_arr[i].core_arr[j]->thread_kop; k++){
                sched_cpu_thread_control_init(
                        (struct sched_cpu_thread_control *) &sched->core_control_array[next_thread_control_index], my_cpu_arr[i].core_arr[j]->thread_arr[k], NULL, 0, NULL_PROCESS_RUN);
                next_thread_control_index++;
            }
        }
    }
}

int __sched_thread_control_array_destroy(sched_basic_t *sched){
    free(sched->core_control_array);
    return 0;
}

