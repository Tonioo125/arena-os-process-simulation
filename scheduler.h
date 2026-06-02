#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "fighter.h"
#include <signal.h>
#include <sys/time.h>

#define QUANTUM_MS 1500   // ms per tick (gives player time to react)

extern volatile sig_atomic_t tick;
extern int current_idx;

void setup_signals(void);
void setup_timer(void);
int  next_fighter(void);    // weighted round-robin
void sigalrm_handler(int sig);
void sigusr1_handler(int sig);  // boost player priority
void sigusr2_handler(int sig);  // debuff random enemy

#endif