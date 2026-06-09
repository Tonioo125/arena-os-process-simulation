#include "fighter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

Fighter fighters[MAX_FIGHTERS];
int     alive_count = 0;
char    battle_log[MAX_LOG][LOG_LEN];

void push_log(const char *msg) {
    // Shift log up
    for (int i = 0; i < MAX_LOG - 1; i++)
        strncpy(battle_log[i], battle_log[i+1], LOG_LEN);
    strncpy(battle_log[MAX_LOG-1], msg, LOG_LEN - 1);
    battle_log[MAX_LOG-1][LOG_LEN-1] = '\0';
}

void init_fighters(void) {
    srand(time(NULL));

    struct { char name[16]; int hp; int atk; int def; int pri; int is_player; } data[] = {
        {"[YOU] Justin",   120, 18, 5, 3, 1},
        {"Golem",        150, 12, 8, 2, 0},
        {"Rogue",         90, 22, 3, 3, 0},
        {"Warlord",      110, 16, 6, 2, 0},
    };

    alive_count = MAX_FIGHTERS;
    for (int i = 0; i < MAX_FIGHTERS; i++) {
        fighters[i].id        = i;
        strncpy(fighters[i].name, data[i].name, 15);
        fighters[i].hp        = data[i].hp;
        fighters[i].max_hp    = data[i].hp;
        fighters[i].attack    = data[i].atk;
        fighters[i].defense   = data[i].def;
        fighters[i].priority  = data[i].pri;
        fighters[i].slice_debt= 0;
        fighters[i].state     = READY;
        fighters[i].is_player = data[i].is_player;
    }

    for (int i = 0; i < MAX_LOG; i++)
        battle_log[i][0] = '\0';

    push_log("⚔  Battle begins! Type a/d/b during your turn.");
}

void apply_damage(int atk_idx, int tgt_idx) {
    Fighter *a = &fighters[atk_idx];
    Fighter *t = &fighters[tgt_idx];
    int dmg = a->attack - t->defense / 2;
    if (dmg < 1) dmg = 1;
    // Small random variance
    dmg += (rand() % 5) - 2;
    if (dmg < 1) dmg = 1;
    t->hp -= dmg;
    if (t->hp <= 0) {
        t->hp    = 0;
        t->state = DEFEATED;
        alive_count--;
    }

    char msg[LOG_LEN];
    snprintf(msg, LOG_LEN, "  %s hits %s for %d dmg!", a->name, t->name, dmg);
    push_log(msg);
}

// Pick a random living opponent
static int random_opponent(int self) {
    int candidates[MAX_FIGHTERS];
    int n = 0;
    for (int i = 0; i < MAX_FIGHTERS; i++)
        if (i != self && fighters[i].state != DEFEATED)
            candidates[n++] = i;
    if (n == 0) return -1;
    return candidates[rand() % n];
}

void ai_take_turn(int idx) {
    int target = random_opponent(idx);
    if (target == -1) return;
    apply_damage(idx, target);
}

void player_take_turn(int idx) {
    // Non-blocking read — player has 2 seconds to act
    // Set stdin to non-blocking via select()
    #include <sys/select.h>
    fd_set fds;
    struct timeval tv = {2, 0};   // 2 second window
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    push_log("  >> YOUR TURN! [a=attack  d=defend  b=boost]");

    char c = 0;
    if (select(1, &fds, NULL, NULL, &tv) > 0)
        read(STDIN_FILENO, &c, 1);

    if (c == 'a' || c == 'A') {
        int target = random_opponent(idx);
        if (target != -1) apply_damage(idx, target);
    } else if (c == 'd' || c == 'D') {
        fighters[idx].defense += 3;
        push_log("  >> Justin braces! +3 defense this round.");
    } else if (c == 'b' || c == 'B') {
        if (fighters[idx].priority < 5) {
            fighters[idx].priority++;
            push_log("  >> Justin focuses! Priority boosted.");
        } else {
            push_log("  >> Priority already maxed!");
        }
    } else {
        push_log("  >> Justin hesitates... (no input)");
    }
}