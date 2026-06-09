#include "display.h"
#include "scheduler.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"

static void hp_bar(int hp, int max_hp) {
    int width  = 20;
    int filled = (hp * width) / max_hp;
    if (filled < 0) filled = 0;
    printf("[");
    for (int i = 0; i < width; i++) {
        if (i < filled)
            printf(hp > max_hp / 2 ? GREEN "█" RESET : RED "█" RESET);
        else
            printf(DIM "░" RESET);
    }
    printf("]");
}

void display_arena(int active_idx) {
    printf("\033[2J\033[H");   // clear screen and cursor home
    printf(BOLD CYAN
        "╔══════════════════════════════════════════════════════════╗\n"
        "║                 JUSTIN  BATTLE  ARENA                    ║\n"
        "║      Scheduler: Weighted Round-Robin | SIGALRM ticks     ║\n"
        "╚══════════════════════════════════════════════════════════╝\n"
        RESET);

    printf(BOLD "  %-14s %-4s %-4s %-4s %-4s  %-22s  %s\n" RESET,
           "Fighter", "PRI", "ATK", "DEF", "HP", "Health Bar", "Status");
    printf("  %-14s %-4s %-4s %-4s %-4s  %-22s  %s\n",
           "──────────────","────","────","────","────",
           "──────────────────────","──────────");

    for (int i = 0; i < MAX_FIGHTERS; i++) {
        Fighter *f = &fighters[i];
        const char *color  = f->is_player ? MAGENTA : CYAN;
        const char *status;

        if      (f->state == DEFEATED)       { color = DIM;    status = "💀 DEFEATED"; }
        else if (i == active_idx)             { color = YELLOW; status = "▶ ACTIVE";   }
        else if (f->state == READY)           status = "  READY";
        else                                  status = "  READY";

        printf("  %s%-14s%s %-4d %-4d %-4d %-4d  ",
               color, f->name, RESET,
               f->priority, f->attack, f->defense, f->hp);
        if (f->state == DEFEATED)
            printf("[" DIM "────────────────────" RESET "]");
        else
            hp_bar(f->hp, f->max_hp);
        printf("  %s%s%s\n", color, status, RESET);
    }

    // Battle log
    printf(BOLD "\n  ────────────────────────────────────── Battle Log ──────────────────────────────────────\n" RESET);
    for (int i = 0; i < MAX_LOG; i++) {
        if (battle_log[i][0])
            printf("  %s\n", battle_log[i]);
    }
    printf(BOLD "\n  ────────────────────────────────────────────────────────────────────────────────────────\n" RESET);
    printf(BOLD "\n  Signals: " RESET
           "kill -USR1 %d " CYAN "(boost you)" RESET
           "  |  kill -USR2 %d " RED "(throttle enemy)" RESET "\n",
           (int)getpid(), (int)getpid());
}

void display_victory(void) {
    printf("\n" BOLD);
    for (int i = 0; i < MAX_FIGHTERS; i++) {
        if (fighters[i].state != DEFEATED) {
            if (fighters[i].is_player)
                printf(GREEN "  🏆 YOU WIN! Hero stands victorious!\n" RESET);
            else
                printf(RED "  💀 YOU LOST! %s wins the arena!\n" RESET, fighters[i].name);
        }
    }
    printf(RESET "\n");
}