#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include "scheduler.h"
#include "fighter.h"
#include "display.h"

// Set terminal to raw mode
static struct termios orig_term;

void enable_raw(void) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_term);
    raw = orig_term;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void disable_raw(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);
}

void sigint_handler(int sig) {
    (void)sig;
    disable_raw();
    printf("\033[2J\033[H");
    printf("\nGame interrupted. Goodbye!\n");
    exit(0);
}

int main(void) {
    printf("\033[2J\033[H");   // clear screen
    printf("⚔  Process Battle Arena starting...\n");
    sleep(1);

    enable_raw();

    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);

    init_fighters();
    setup_signals();
    setup_timer();

    display_arena(-1);

    while (alive_count > 1) {
        if (!tick) continue;
        tick = 0;

        int idx = next_fighter();
        if (idx == -1) break;

        fighters[idx].state = RUNNING;

        if (fighters[idx].is_player)
            player_take_turn(idx);
        else
            ai_take_turn(idx);

        fighters[idx].state = (fighters[idx].state == DEFEATED) ? DEFEATED : READY;

        display_arena(idx);
    }

    // Stop timer
    struct itimerval stop = {0};
    setitimer(ITIMER_REAL, &stop, NULL);

    display_victory();
    disable_raw();
    return 0;
}