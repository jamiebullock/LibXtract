TARGETS = xtfuzz_scalar xtfuzz_delta xtfuzz_vector

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

all: $(TARGETS)

xtfuzz_%: fuzz_%.c ../src/libxtract.a
	$(FUZZ_CC) $(CFLAGS) -o $@ $< ../src/libxtract.a $(LIBS)

clean:
	@$(RM) $(TARGETS)
	@$(RM) -r $(addsuffix .dSYM,$(TARGETS))

.PHONY: all clean
