CC = clang

CFLAGS  = -g -Wall -std=c99 -D_GNU_SOURCE -pthread

# the build target executable:
MUMSH = mumsh
MUMSHMC_FLAGS= -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=integer
MUMSHMC = mumsh_memory_check
MUMSH_SRC = execute.c input.c split.c main.c
all: $(MUMSH) ${MUMSHMC}

# assume the C code for TARGET is TARGET.

$(MUMSH): $(MUMSH_SRC)
	$(CC) $(CFLAGS) -o $(MUMSH) $(MUMSH_SRC)

$(MUMSHMC): $(MUMSH_SRC)
	$(CC) $(CFLAGS) $(MUMSHMC_FLAGS) -o $(MUMSHMC) $(MUMSH_SRC)

clean:
	$(RM) *.o *.a *~ $(MUMSH) $(MUMSHMC) 
