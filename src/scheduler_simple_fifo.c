#include "sched_simple_fifo.h"
#include <stdio.h>
#include <stdlib.h>

void rq_simple_fifo_in(rt_runqueue *rq, pcb_t *new_pcb){
    linkedlist_push((linkedlist_structure *) &rq->active, (void *) new_pcb);
    rq->nr_running++;
}

void rq_simple_fifo_init(rt_runqueue *rq){
    linkedlist_init((linkedlist_structure *) &rq->active);
    rq->nr_running = 0;
}

pcb_t *rq_simple_fifo_out(rt_runqueue *rq){
    if(rq->nr_running){
        rq->nr_running--;
        return (pcb_t *) linkedlist_pop((linkedlist_structure *) &rq->active);
    }
    else{
        return NULL;
    }
}

pcb_t *rq_simple_fifo_peek(rt_runqueue *rq){
    if(rq->nr_running){
        return (pcb_t *) linkedlist_get_first((linkedlist_structure *) &rq->active);
    }
    else{
        return NULL;
    }
}

char *rq_simple_fifo_print(rt_simple_runqueue *rq){
    printf("SCHEDULER: Simple FIFO\n");
    printf("Process total list length: %d\n", rq->nr_running);
    printf("Process pids: ");
    struct linkedlist_node *process_node;
    for(process_node = rq->active.first; process_node != NULL; process_node = process_node->next){
        pcb_t *process = (pcb_t *) process_node->data;
        printf("%d ", process->pid);
    }
    printf("\n");
    return "";
}

void rq_simple_fifo_i_sched_implement(s_i_sched *i_sched){
    rt_simple_runqueue *sched_data = (rt_simple_runqueue *) malloc(sizeof(rt_simple_runqueue));
    i_sched_implement(
            i_sched,
            sched_data,
            (void (*)(void *, struct pcb_s *)) &rq_simple_fifo_i_sched_adpt_in,
            (void (*)(void *)) &rq_simple_fifo_i_sched_init,
            (pcb_t *(*)(void *)) &rq_simple_fifo_i_sched_adpt_peek,
            (pcb_t *(*)(void *)) &rq_simple_fifo_i_sched_adpt_out,
            (char *(*)(void *)) &rq_simple_fifo_i_sched_adpt_print
    );
}

void rq_simple_fifo_i_sched_adpt_in(rt_runqueue *rq, pcb_t *new_pcb){
    rq_simple_fifo_in(rq, new_pcb);
}

void rq_simple_fifo_i_sched_init(s_i_sched *i_sched){
    rt_simple_runqueue *sched_data = (rt_simple_runqueue *) i_sched->sched_data;
    rq_simple_fifo_init((rt_runqueue *) sched_data);
}

pcb_t *rq_simple_fifo_i_sched_adpt_out(rt_runqueue *rq){
    return rq_simple_fifo_out(rq);
}

pcb_t *rq_simple_fifo_i_sched_adpt_peek(rt_runqueue *rq){
    return rq_simple_fifo_peek(rq);
}

char *rq_simple_fifo_i_sched_adpt_print(rt_runqueue *rq){
    return rq_simple_fifo_print((rt_simple_runqueue *) rq);
}
