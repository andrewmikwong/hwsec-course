include ../cpu.mk

TARGETS=receiver sender test_unit
UTILS=util.o lib_covert.o

all: $(TARGETS)

# Compile library
lib_covert.o: lib_covert.c lib_covert.h
	$(CC) $(CFLAGS) -c lib_covert.c

# Compile legacy util
util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

# Compile Targets
sender: sender.o $(UTILS)
	$(CC) $(CFLAGS) sender.o $(UTILS) -o sender

receiver: receiver.o $(UTILS)
	$(CC) $(CFLAGS) receiver.o $(UTILS) -o receiver

test_unit: test_unit.o $(UTILS)
	$(CC) $(CFLAGS) test_unit.o $(UTILS) -o test_unit

# Object files
%.o: %.c
	$(CC) $(CFLAGS) -c $<

run_sender: sender 
	taskset -c $(SENDER_CPU) ./sender

run_receiver: receiver
	taskset -c $(RECEIVER_CPU) ./receiver

run_tests: test_unit
	./test_unit

.PHONY:	clean
clean:
	$(RM) *.o $(TARGETS)
