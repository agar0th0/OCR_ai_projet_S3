CC = gcc

CFLAGS = -Wall -Wextra

TARGETS = solver neural_network loader rotator detector
# we will add rotator and detector

all: $(TARGETS)

detector: detector.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

rotator: rotator.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

solver: solver.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

neural_network: neural_network.o
	$(CC) $(CLFAGS) -o $@ $^ -lm

loader: loader.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -lm

clean:
	rm -f *.o $(TARGETS)

.PHONY: all clean
