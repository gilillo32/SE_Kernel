#ifndef PROIEKTUA_SCHEDULER_S_H
#define PROIEKTUA_SCHEDULER_S_H

#include <pthread.h>
#include "linkedlist_s.h"
#include "pcb_s.h"
#include "runqueue_s.h"
#include "cpu_data_s.h"
#include "clock_s.h"
#include "scheduler_data_interface.h"

#define NULL_PROCESS_RUN 1
#define PROCESS_RUN 2

typedef struct sched_struct sched_basic_t;

struct sched_struct{
    pthread_mutex_t *sched_mutex_list;
    int current_policy;
    s_i_sched scheduler_queue_i_object;
    pcb_t null_process;
    clk_timer_s *timer;
    struct sched_cpu_thread_control **core_control_array;
    int core_control_array_size;
};

struct sched_cpu_thread_control{
    struct thread_s *thread_s;
    pcb_t *current_process;
    int current_process_quantum;
    int currrent_cpu_sched_status;
};

void sched_add_to_process_queue(sched_basic_t *sched, pcb_t *new_pcb);
void sched_execute(sched_basic_t *sched);
void sched_dispatcher(sched_basic_t *sched, struct sched_cpu_thread_control *thread_control);
pcb_t *__sched_get_next_process(sched_basic_t *sched);
void sched_init(sched_basic_t *sched, clk_timer_s *timer, struct cpu_s cpu_array[], int cpu_kop, int policy);
void __disp_load(struct thread_s *thread_s, pcb_t *pcb);
void __disp_unload(struct thread_s *thread_s, pcb_t *pcb);
void disp_dispatch(pcb_t *old_pcb, pcb_t *new_pcb, struct thread_s *thread_s);
void disp_terminate(struct thread_s *thread_s, pcb_t *pcb);
pcb_t *__sched_get_next_process_clean_queue(sched_basic_t *sched, struct sched_cpu_thread_control * thread_control);
int sched_cpu_thread_control_init(struct sched_cpu_thread_control *thread_control, thread_s *thread_s, pcb_t *current_process, int current_process_quantum, int currrent_cpu_sched_status);

int __sched_update_running_pcb(struct sched_cpu_thread_control *thread_control);

#endif //PROIEKTUA_SCHEDULER_S_H
