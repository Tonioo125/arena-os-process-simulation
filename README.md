# Process Battle Arena

A time-sharing system simulation built in C, where fighters compete for CPU time slices managed by a real POSIX signal-driven scheduler.

---

## Description

Process Battle Arena simulates how an operating system shares CPU time between multiple processes. Each fighter represents a process in a process table. A weighted round-robin scheduler, driven by `SIGALRM` and `setitimer()`, gives each fighter a time slice to act. Higher priority fighters get more slices, just like real OS scheduling.

The player controls one fighter directly, while the rest are AI-controlled. Signals (`SIGUSR1`, `SIGUSR2`) can be sent from another terminal to boost or throttle fighters mid-battle.

---

## Concepts Demonstrated

| OS Concept           | Implementation                                              |
| -------------------- | ----------------------------------------------------------- |
| Time-sharing         | `SIGALRM` fires every quantum via `setitimer()`             |
| Process table        | Array of `Fighter` structs with state tracking              |
| Weighted Round-Robin | `slice_debt` + `priority` controls turn frequency           |
| Signal handling      | `sigaction()` for `SIGALRM`, `SIGUSR1`, `SIGUSR2`, `SIGINT` |
| Process states       | `READY`, `RUNNING`, `DEFEATED`                              |
| Preemption           | Active fighter is switched out every tick automatically     |

---

## Project Structure

```
arena/
├── main.c        ← Entry point, terminal raw mode, SIGINT handler
├── scheduler.c   ← Weighted round-robin logic, signal handlers
├── scheduler.h
├── fighter.c     ← Fighter init, AI logic, player turn, damage
├── fighter.h
├── display.c     ← Terminal UI, HP bars, battle log
├── display.h
└── Makefile
```

---

## Requirements

- Linux or WSL (Ubuntu recommended)
- GCC compiler
- Terminal with ANSI color support

---

## Build & Run

```bash
# Clone or navigate to the project folder
cd ~/arena

# Compile
make

# Run
./arena
```

---

## Controls

During **your turn** (Justin is ACTIVE), you have **2 seconds** to press:

| Key         | Action                              |
| ----------- | ----------------------------------- |
| `a`         | Attack a random enemy               |
| `d`         | Defend — gain +3 defense this round |
| `b`         | Boost your own priority (max 5)     |
| _(nothing)_ | Justin hesitates, turn is skipped   |

---

## Sending Signals

Open a **second terminal** while the game is running:

```bash
# Get the PID shown at the bottom of the game screen, then:

kill -USR1 <pid>   # Boost Justin's priority (+1, max 5)
kill -USR2 <pid>   # Throttle a random enemy's priority (-1)
kill -INT  <pid>   # Quit the game cleanly (same as Ctrl+C)
```

---

## Fighters

| Fighter | HP  | Attack | Defense | Priority | Controlled by |
| ------- | --- | ------ | ------- | -------- | ------------- |
| Justin  | 120 | 18     | 5       | 3        | Player        |
| Golem   | 150 | 12     | 8       | 2        | AI            |
| Rogue   | 90  | 22     | 3       | 3        | AI            |
| Warlord | 110 | 16     | 6       | 2        | AI            |

---

## How the Scheduler Works

```
SIGALRM fires every 1500ms
        │
        ▼
tick flag set in sigalrm_handler()
        │
        ▼
main loop detects tick → calls next_fighter()
        │
        ▼
weighted round-robin picks next READY fighter
(higher priority = slice_debt fills faster = more turns)
        │
        ▼
if player → player_take_turn()   (waits up to 2s for input)
if AI     → ai_take_turn()       (attacks random opponent)
        │
        ▼
damage applied → HP updated → check for DEFEATED
        │
        ▼
display_arena() redraws terminal
        │
        ▼
repeat until alive_count == 1
```

---

## Damage Formula

```
damage = attacker.attack - (target.defense / 2) + random(-2, +2)
minimum damage = 1
```

---

## Additional Note

This project uses only Standard C and raw POSIX system calls. No threads, no external libraries, no process simulation frameworks used in this project.

| Header         | Purpose                                  |
| -------------- | ---------------------------------------- |
| `<signal.h>`   | `sigaction()`, signal constants          |
| `<sys/time.h>` | `setitimer()`                            |
| `<unistd.h>`   | `read()`, `getpid()`, `sleep()`          |
| `<termios.h>`  | Raw terminal mode for instant keypresses |
| `<stdio.h>`    | Output                                   |
| `<stdlib.h>`   | `exit()`, `rand()`                       |
| `<string.h>`   | `strncpy()`, `snprintf()`                |

---
