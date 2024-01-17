#ifndef PROIEKTUA_SCHED_PRIO_FIFO_H
#define PROIEKTUA_SCHED_PRIO_FIFO_H

#include "scheduler_s.h"
#include "runqueue_s.h"

void rq_prio_fifo_in(rt_runqueue *rq, pcb_t *new_pc);
void rq_prio_fifo_init(rt_runqueue *rq);
pcb_t *rq_prio_fifo_out(rt_runqueue *rq);
pcb_t *rq_prio_fifo_peek(rt_runqueue *rq);
char *rq_prio_fifo_print(rt_runqueue *rq);
int _rq_prio_fifo_update_sched(rt_runqueue *rq);
void rq_prio_fifo_i_sched_inplement(s_i_sched *i_sched);
void rq_prio_fifo_i_sched_init(s_i_sched *i_sched);
void rq_prio_fifo_i_sched_adpt_in(rt_runqueue *rq, pcb_t *new_pcb);
pcb_t *rq_prio_fifo_i_sched_adpt_out(rt_runqueue *rq);
pcb_t *rq_prio_fifo_i_sched_adpt_peek(rt_runqueue *rq);
char *rq_prio_fifo_i_sched_adpt_print(rt_runqueue *rq);

#endif //PROIEKTUA_SCHED_PRIO_FIFO_H
