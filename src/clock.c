#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "linkedlist_s.h"
#include "clock_s.h"

void *clk_clock_start(void *clk_mutex_s){
	clk_clock_s * clk;
	clk = (clk_clock_s *) clk_mutex_s;
	pthread_mutex_lock(clk -> mutex_clk);

	while(1){
		printf("\nCLK: tick %d = %d\n", clk -> timer_tick_consumed_cont);
		while(clk -> timer_tick_consumed_cont < clk -> timer_total_cont || clk -> timer_total_cont == 0){
			pthread_cond_wait(clk -> cond_timer_attention, clk -> mutex_clk);
		}
		pthread_cond_broadcast(clk -> cond_clk_cycle);
		clk -> timer_tick_consumed_cont = 0;
	}
	pthread_mutex_unlock(clk -> mutex_clk);
}

void *clk_timer_start(void *p_timer_s){
	clk_timer_s * tmr;
	clk_clock_s * t_clk;
	tmr = (clk_timer_s *) p_timer_s;
	t_clk = tmr -> linked_clk;
	tmr -> tick_count = tmr -> timer_tick_freq;
	pthread_mutex_lock(t_clk -> mutex_clk);
	t_clk -> timer_total_cont++;
	while(tmr -> timer_status != CLOCK_STATUS_TERMINATE){
		tmr -> tick_count--;
		printf("\nTMR - %s: freq %d; count %d \n", tmr -> name, tmr -> timer_tick_freq, tmr -> tick_count);

		if(tmr -> tick_count <= 0){
			printf("\nFIRE\n");
			tmr -> TickAction(tmr -> tickActionParams);
			tmr -> tick_count = tmr -> timer_tick_freq;
		}
		else{
			printf("\nWAIT\n");
		}
		t_clk -> timer_tick_consumed_cont++;
		pthread_cond_signal(t_clk -> cond_timer_attention);
		pthread_cond_wait(t_clk -> cond_clk_cycle, t_clk -> mutex_clk);
	}
	t_clk -> timer_total_cont--;
	pthread_mutex_unlock(t_clk -> mutex_clk);
}

void clk_clock_init(clk_clock_s * clk_struct) {
    pthread_mutex_t *g_mutex_clk = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_cond_t *g_cond_clk_cycle = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_t *g_cond_timer_attention = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));

    pthread_mutex_init(g_mutex_clk, NULL);
    pthread_cond_init(g_cond_clk_cycle, NULL);
    pthread_cond_init(g_cond_timer_attention, NULL);

    clk_struct->mutex_clk = g_mutex_clk;
    clk_struct->cond_clk_cycle = g_cond_clk_cycle;
    clk_struct->cond_timer_attention = g_cond_timer_attention;
    clk_struct->timer_tick_consumed_cont = 0;
    clk_struct->timer_total_cont = 0;
    clk_struct->clk_status = CLOCK_STATUS_RUNNING;
}

void clk_timer_init(clk_timer_s * timer_struct, clk_clock_s * clk_struct, int timer_tick_freq, DoWhenTick tick_action, void * tick_action_params, char * name) {
    timer_struct->linked_clk = clk_struct;
    timer_struct->timer_tick_freq = timer_tick_freq;
    timer_struct->TickAction = tick_action;
    timer_struct->tickActionParams = tick_action_params;
    timer_struct->timer_status = CLOCK_STATUS_RUNNING;
    timer_struct->tick_count = 0;
    timer_struct->name = name;
}

void clk_stop_timer(clk_timer_s * timer_struct) {
    timer_struct->timer_status = CLOCK_STATUS_TERMINATE;
}

void clk_change_timer_tickFreq(clk_timer_s * timer_struct, int new_tick_freq, int restart) {
    if (restart) timer_struct->tick_count = timer_struct->timer_tick_freq;
    else{
        int consumed_ticks = timer_struct -> timer_tick_freq - timer_struct -> tick_count;
        timer_struct->tick_count = new_tick_freq - consumed_ticks;
        if(timer_struct -> tick_count <= 0) timer_struct->tick_count = 1;
    }
    timer_struct -> timer_tick_freq = new_tick_freq;
}

