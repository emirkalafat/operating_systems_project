CC = gcc
CFLAGS = -Wall -Wextra -g
TARGETS = calculator addition subtraction multiplication division saver

all: $(TARGETS)

calculator: calculator.o
	$(CC) $(CFLAGS) -o $@ $^

addition: addition.o
	$(CC) $(CFLAGS) -o $@ $^

subtraction: subtraction.o
	$(CC) $(CFLAGS) -o $@ $^

multiplication: multiplication.o
	$(CC) $(CFLAGS) -o $@ $^

division: division.o
	$(CC) $(CFLAGS) -o $@ $^

saver: saver.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGETS)