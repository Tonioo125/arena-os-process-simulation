CC     = gcc
CFLAGS = -Wall -Wextra -g
TARGET = arena
SRCS   = main.c scheduler.c fighter.c display.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)