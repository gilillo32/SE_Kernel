#include "runqueue_s.h"

void prio_array_init(rt_prio_array * pri_array){
    pri_array->nr_active = 0;
    for(int i = 0; i < MAX_PRIO; i++){
        linkedlist_init(&(pri_array->queue[i]));
    }
}

void runqueue_init(rt_runqueue * runq){
    runq->nr_running = 0;
    runq->active = &(runq->array[0]);
    runq->expired = &(runq->array[1]);
    prio_array_init(runq->active);
    prio_array_init(runq->expired);
}

void simple_runqueue_init(rt_simple_runqueue * runq){
    runq->nr_running = 0;
    linkedlist_init(&(runq->active));
}