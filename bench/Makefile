# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

TARGETS = xtbench xtbench_moving
CFLAGS = -O3 -I../include -I.
LDFLAGS =
LIBS =

OS := $(shell uname)
ifeq ($(OS), Darwin)
    LIBS += -framework Accelerate
endif

LIBS += -lm

all: $(TARGETS)

xtbench: bench_scalar.o ../src/libxtract.a
	$(CC) $(LDFLAGS) -o $@ bench_scalar.o ../src/libxtract.a $(LIBS)

xtbench_moving: bench_moving.o ../src/libxtract.a
	$(CC) $(LDFLAGS) -o $@ bench_moving.o ../src/libxtract.a $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

bench: $(TARGETS)
	@./xtbench
	@./xtbench_moving

clean:
	@$(RM) $(TARGETS) *.o

.PHONY: all bench clean
