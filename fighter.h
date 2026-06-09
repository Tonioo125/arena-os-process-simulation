#ifndef FIGHTER_H
#define FIGHTER_H

#define MAX_FIGHTERS  4
#define MAX_LOG       6     
#define LOG_LEN       64

typedef enum {
    READY,
    RUNNING,
    DEFEATED
} FighterState;

typedef struct {
    int          id;
    char         name[16];
    int          hp;
    int          max_hp;
    int          attack;
    int          defense;
    int          priority;    // 1–5; higher = more slices
    int          slice_debt;  // used for weighted round-robin
    FighterState state;
    int          is_player;
} Fighter;

extern Fighter fighters[MAX_FIGHTERS];
extern int     alive_count;
extern char    battle_log[MAX_LOG][LOG_LEN];

void init_fighters(void);
void ai_take_turn(int idx);
void player_take_turn(int idx);
void apply_damage(int attacker, int target);
void push_log(const char *msg);

#endif