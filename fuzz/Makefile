TARGET = xtfuzz
SRC = fuzz_scalar.c

# A clang with libFuzzer. Apple clang lacks the runtime; on macOS install LLVM
# via Homebrew and pass FUZZ_CC=/opt/homebrew/opt/llvm/bin/clang.
FUZZ_CC ?= clang
FUZZ_SAN ?= address,undefined
SANITIZE = -fsanitize=fuzzer,$(FUZZ_SAN) -fno-sanitize-recover=all -g -O1
CFLAGS = -I../include $(SANITIZE)

LIBS =
OS := $(shell uname)
ifeq ($(OS), Darwin)
    LIBS += -framework Accelerate
endif
LIBS += -lm

$(TARGET): $(SRC) ../src/libxtract.a
	$(FUZZ_CC) $(CFLAGS) -o $@ $(SRC) ../src/libxtract.a $(LIBS)

clean:
	@$(RM) $(TARGET)
	@$(RM) -r $(TARGET).dSYM

.PHONY: clean
