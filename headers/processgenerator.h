#ifndef PROIEKTUA_PROCESSGENERATOR_H
#define PROIEKTUA_PROCESSGENERATOR_H

#include <scheduler_s.h>

struct pr_generator_s{
    int MAX_PID;
    int MIN_PID;
    int max_prio;
    int current_counter;
    sched_basic_t *scheduler;
};

typedef struct pr_generator_s pr_generator_s;

void _prgenrator_init(pr_generator_s *prgen, sched_basic_t *sched, int min_pid, int max_pid, int max_prio);
int _prgenerator_get_valid_pid(pr_generator_s *prgen);
void prgenerator_generate_process(pr_generator_s *prgen);
void sched_print_sched_status(sched_basic_t *sched);

#endif //PROIEKTUA_PROCESSGENERATOR_H
