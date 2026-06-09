#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>

volatile sig_atomic_t tick = 0;
int current_idx = 0;

void sigalrm_handler(int sig) { (void)sig; tick = 1; }

void sigusr1_handler(int sig) {
    (void)sig;
    // Boost player priority
    for (int i = 0; i < MAX_FIGHTERS; i++) {
        if (fighters[i].is_player && fighters[i].state != DEFEATED) {
            if (fighters[i].priority < 5) fighters[i].priority++;
            push_log("  [SIGNAL] SIGUSR1: Justin priority boosted!");
        }
    }
}

void sigusr2_handler(int sig) {
    (void)sig;
    // Throttle a random living enemy
    for (int i = MAX_FIGHTERS - 1; i >= 0; i--) {
        if (!fighters[i].is_player && fighters[i].state != DEFEATED) {
            if (fighters[i].priority > 1) fighters[i].priority--;
            char msg[64];
            snprintf(msg, 64, "  [SIGNAL] SIGUSR2: %s throttled!", fighters[i].name);
            push_log(msg);
            break;
        }
    }
}

void setup_signals(void) {
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    sa.sa_handler = sigalrm_handler;
    sigaction(SIGALRM, &sa, NULL);

    sa.sa_handler = sigusr1_handler;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_handler = sigusr2_handler;
    sigaction(SIGUSR2, &sa, NULL);
}

void setup_timer(void) {
    struct itimerval t;
    t.it_value.tv_sec     = QUANTUM_MS / 1000;
    t.it_value.tv_usec    = (QUANTUM_MS % 1000) * 1000;
    t.it_interval         = t.it_value;
    setitimer(ITIMER_REAL, &t, NULL);
}

// Weighted round-robin: priority = number of slices before skipping
int next_fighter(void) {
    for (int tries = 0; tries < MAX_FIGHTERS * 5; tries++) {
        current_idx = (current_idx + 1) % MAX_FIGHTERS;
        Fighter *f = &fighters[current_idx];
        if (f->state == DEFEATED) continue;

        f->slice_debt++;
        if (f->slice_debt >= (6 - f->priority)) {
            f->slice_debt = 0;
            return current_idx;
        }
    }
    // fallback: pick any alive fighter
    for (int i = 0; i < MAX_FIGHTERS; i++)
        if (fighters[i].state != DEFEATED) return i;
    return -1;
}