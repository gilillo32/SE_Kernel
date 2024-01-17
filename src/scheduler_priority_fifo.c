#include "sched_prio_fifo.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

int get_first_not_empty_list_index(linkedlist_structure arr[]);

void rq_prio_fifo_in(rt_runqueue *sched, pcb_t *new_pcb){
    int pcb_priority = new_pcb->current_priority;
    rt_prio_array  *prioArray = sched->expired;
    if(MAX_PRIO <= pcb_priority){
        pcb_priority = MAX_PRIO - 1;
    }
    linkedlist_structure *equal_prio_list = &prioArray->queue[pcb_priority];
    linkedlist_push(equal_prio_list, (void *) new_pcb);
    prioArray->nr_active++;
    sched->nr_running;
}

void rq_prio_fifo_init(rt_runqueue *sched){
    sched->expired = malloc(sizeof(rt_prio_array));
    if(sched->expired == NULL){
        err(1, "malloc failed in rq_prio_fifo_init");
    }
    rt_prio_array *prioArray = sched->expired;
    for(int i = 0; i < MAX_PRIO; i++){
        linkedlist_init(&prioArray->queue[i]);
    }
    prioArray->nr_active = 0;
    sched->nr_running = 0;
}

pcb_t * rq_prio_fifo_peek(rt_runqueue *sched){
    _rq_prio_fifo_update_sched(sched);
    rt_prio_array *prioArray = sched->active;
    if(prioArray->nr_active){
        int next_prio = get_first_not_empty_list_index(prioArray->queue);
        return (pcb_t *) linkedlist_get_first(&prioArray->queue[next_prio]);
    }
    else{
        return NULL;
    }
}

pcb_t * rq_prio_fifo_out(rt_runqueue *sched){
    _rq_prio_fifo_update_sched(sched);
    rt_prio_array *prioArray = sched->active;
    if(prioArray->nr_active){
        prioArray->nr_active--;
        sched->nr_running--;
        int next_prio = get_first_not_empty_list_index(prioArray->queue);
        return (pcb_t *) linkedlist_pop(&prioArray->queue[next_prio]);
    }
    else{
        return NULL;
    }
}

char * rq_prio_fifo_print(rt_runqueue *sched){
    printf("Process total list length: %d\n", sched->nr_running);
    printf("Process active: %d\n", sched->active->nr_active);
    printf("Process expired: %d\n", sched->expired->nr_active);
    printf("Process active list:\n");
    for(int i = 0; i < MAX_PRIO; i++){
        if(sched->active->queue[i].length != 0){
            printf("Priority %d: ", i);
            struct linkedlist_node *pr;
            for(pr = sched->active->queue[i].first; pr != NULL; pr = pr->next){
                printf("%d ", ((pcb_t *) pr->data)->pid);
            }
        }
    }
    printf("\n");
    printf("Process expired list:\n");
    for(int i = 0; i < MAX_PRIO; i++){
        if(sched->expired->queue[i].length != 0){
            printf("Priority %d: ", i);
            struct linkedlist_node *pr;
            for(pr = sched->expired->queue[i].first; pr != NULL; pr = pr->next){
                printf("%d ", ((pcb_t *) pr->data)->pid);
            }
        }
    }
    printf("\n");
    return NULL;
}

int _rq_prio_fifo_update_sched(rt_runqueue *sched){
    if(sched->expired->nr_active == 0){
        return 0;
    }
    rt_prio_array *prioArray = sched->active;
    rt_prio_array *expired = sched->expired;
    if(prioArray->nr_active == 0){
        rt_prio_array *tmp = prioArray;
        prioArray = expired;
        expired = tmp;
        sched->active = prioArray;
        sched->expired = expired;
    }
    else{
        int next_prio = get_first_not_empty_list_index(expired->queue);
        linkedlist_structure *next_prio_list = &expired->queue[next_prio];
        pcb_t *next_pcb = (pcb_t *) linkedlist_pop(next_prio_list);
        linkedlist_push(&prioArray->queue[next_prio], (void *) next_pcb);
        expired->nr_active--;
        prioArray->nr_active++;
    }
    return 1;
}

int get_first_not_empty_list_index(linkedlist_structure arr[]){
    for(int i = 0; i < MAX_PRIO; i++){
        if(arr[i].length != 0){
            return i;
        }
    }
    return -1;
}

void rq_prio_fifo_i_sched_inplement(s_i_sched *i_sched){
    rt_runqueue *rq = (rt_runqueue *) malloc(sizeof(rt_runqueue));
    i_sched_implement(
            i_sched,
            (void *) rq,
            (void (*)(void *, struct pcb_s *)) (void (*)(void *, pcb_t *)) rq_prio_fifo_in,
            (void (*)(void *)) rq_prio_fifo_init,
            (pcb_t *(*)(void *)) rq_prio_fifo_out,
            (pcb_t *(*)(void *)) rq_prio_fifo_peek,
            (char *(*)(void *)) rq_prio_fifo_print
            );
}

void rq_prio_fifo_i_sched_adpt_in(rt_runqueue *rq, pcb_t *new_pcb){
    rq_prio_fifo_in(rq, new_pcb);
}

void rq_prio_fifo_i_sched_adpt_init(s_i_sched *i_sched){
    rq_prio_fifo_init((rt_runqueue *) i_sched->sched_data);
}

pcb_t *rq_prio_fifo_i_sched_adpt_out(rt_runqueue *rq){
    return rq_prio_fifo_out(rq);
}

pcb_t *rq_prio_fifo_i_sched_adpt_peek(rt_runqueue *rq){
    return rq_prio_fifo_peek(rq);
}

char *rq_prio_fifo_i_sched_adpt_print(rt_runqueue *rq){
    return rq_prio_fifo_print(rq);
}