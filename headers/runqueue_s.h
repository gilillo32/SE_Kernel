#ifndef RUNQUEUES_H
#define RUNQUEUES_H

#define MAX_PRIO 20

#include <linkedlist_s.h>

struct rt_prio_array{
    int nr_active;
    linkedlist_structure queue[MAX_PRIO];
};

typedef struct rt_prio_array rt_prio_array;

struct rt_runqueue{
    int nr_running;
    rt_prio_array *active;
    rt_prio_array *expired;
    rt_prio_array array[2];
};

typedef struct rt_runqueue rt_runqueue;

struct rt_simple_runqueue{
    int nr_running;
    linkedlist_structure active;
};

typedef struct rt_simple_runqueue rt_simple_runqueue;

void prio_array_init(rt_prio_array * pri_array);
void runqueue_init(rt_runqueue * runq);
void simple_runqueue_init(rt_simple_runqueue * runq);

linkedlist_structure *rt_prio_array_get_queue(rt_prio_array pri_array, int prio);
int rt_prio_array_length(rt_prio_array rt_prio_array, int prio);

#endif
