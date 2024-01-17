#ifndef CLOCK_H

#define CLOCK_H
#include <pthread.h>

#define CLOCK_STATUS_RUNNING 1
#define CLOCK_STATUS_TERMINATE 2

typedef void (*DoWhenTick)();
struct clk_timer_s {
	struct clk_clock_s * linked_clk;
	int timer_tick_freq;
	DoWhenTick TickAction;
	void * tickActionParams;
	int timer_status;
	int tick_count;
	char * name;
};

typedef struct clk_timer_s clk_timer_s;

struct clk_clock_s {
	pthread_mutex_t * mutex_clk;
	pthread_cond_t * cond_clk_cycle;
	pthread_cond_t * cond_timer_attention;
	int timer_tick_consumed_cont;
	int timer_total_cont;
	int clk_status;
};

typedef struct clk_clock_s clk_clock_s;

void *clk_timer_start(void *timer_struct);
void *clk_clock_start(void *clk_struct);

void clk_clock_init(clk_clock_s *clk_struct);

void clk_timer_init(clk_timer_s *timer_struct, clk_clock_s *clk_struct, int timer_tick_freq, DoWhenTick tick_action, void *tick_action_params, char * name);

void clk_stop_timer(clk_timer_s *timer_struct);

void clk_change_timer_tick_freq(clk_timer_s *timer_struct, int new_tick_freq, int restart);

#endif

