#include "scheduler_data_interface.h"
#include "sched_simple_fifo.h"
#include "sched_prio_fifo.h"

void i_sched_implement(
        s_i_sched *i_sched,
        void *sched_data,
        void (*in)(void *, struct pcb_s *),
        void (*init)(void *),
        pcb_t *(*peek)(void *),
        pcb_t *(*out)(void *),
        char *(*print)(void *)
        ){
    i_sched->sched_data = sched_data;
    i_sched->insert = in;
    i_sched->init = init;
    i_sched->peek = peek;
    i_sched->pop = out;
    i_sched->print = print;
}

void i_sched_init(s_i_sched *i_sched){
    i_sched->init(i_sched->sched_data);
}

pcb_t *i_sched_peek(s_i_sched *i_sched){
    return i_sched->peek(i_sched->sched_data);
}

pcb_t *i_sched_pop(s_i_sched *i_sched){
    return i_sched->pop(i_sched->sched_data);
}

void i_sched_insert(s_i_sched *i_sched, pcb_t *new_pcb){
    i_sched->insert(i_sched->sched_data, (struct pcb_s *) new_pcb);
}

char *i_sched_print(s_i_sched *i_sched){
    return i_sched->print(i_sched->sched_data);
}

void i_sched_configure(s_i_sched *ifaceo, int policy){
    switch (policy) {
        case SCHED_PRIO_RR:
        case SCHED_PRIO_FIFO:
            rq_prio_fifo_i_sched_inplement(ifaceo);
            break;
        case SCHED_RR:
        case SCHED_FIFO:
        default:
            rq_simple_fifo_i_sched_inplement(ifaceo);
            break;
    }
    i_sched_init(ifaceo);
}