TARGET = xtbench
SRC = bench_scalar.c
OBJ = $(SRC:.c=.o)
CFLAGS = -O3 -I../include -I.
LDFLAGS =
LIBS =

OS := $(shell uname)
ifeq ($(OS), Darwin)
    LIBS += -framework Accelerate
endif

LIBS += -lm

$(TARGET): $(OBJ) ../src/libxtract.a
	$(CC) $(LDFLAGS) -o $@ $(OBJ) ../src/libxtract.a $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

bench: $(TARGET)
	@./$(TARGET)

clean:
	@$(RM) $(TARGET) $(OBJ)

.PHONY: bench clean
