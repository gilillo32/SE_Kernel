#ifndef PROIEKTUA_SCHEDULER_DATA_INTERFACE_H
#define PROIEKTUA_SCHEDULER_DATA_INTERFACE_H

#include "pcb_s.h"
#include <string.h>

#define SCHED_PRIO_RR 1
#define SCHED_PRIO_FIFO 2
#define SCHED_FIFO 3
#define SCHED_RR 4

struct s_i_sched{
    //Interface scheduler
    void *sched_data;
    void (*insert)(void *sched_data, struct pcb_s *pcb);
    void (*init)(void *sched_data);
    pcb_t *(*peek)(void *sched_data);
    pcb_t *(*pop)(void *sched_data);
    char *(*print)(void *sched_data);
};

typedef struct s_i_sched s_i_sched;

void i_sched_init(s_i_sched *sched);
pcb_t *i_sched_peek(s_i_sched *sched);
pcb_t *i_sched_pop(s_i_sched *sched);
void i_sched_insert(s_i_sched *sched, pcb_t *pcb);
char *i_sched_print(s_i_sched *sched);

void i_sched_implement(
        s_i_sched *sched,
        void *sched_data,
        void (*insert)(void *sched_data, struct pcb_s *pcb),
        void (*init)(void *sched_data),
        pcb_t *(*peek)(void *sched_data),
        pcb_t *(*pop)(void *sched_data),
        char *(*print)(void *sched_data)
        );

void i_sched_configure(s_i_sched *sched, int policy);

#endif //PROIEKTUA_SCHEDULER_DATA_INTERFACE_H
