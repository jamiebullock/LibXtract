NAME = xtract

OS := $(shell uname)

# Assume that since we're building the python bindings, we have a python installed!
INCLUDEPY = $(shell python -m sysconfig | grep -w INCLUDEPY | awk '{print $$3}')

ifeq ($(OS), Darwin)
    CFLAGS=-g -c
    LD=gcc
    LDFLAGS=-bundle -flat_namespace -undefined suppress
else
    CFLAGS=-g -c -fPIC
    LD=ld
    LDFLAGS=-shared
endif

.PHONY: python

python:
	@swig -I../include -python $(NAME).i
	@$(CC) $(CFLAGS) $(NAME)_wrap.c -o $(NAME)_wrap.o -I$(INCLUDEPY) -I../include
	@$(CC) $(LDFLAGS) ../src/lib$(NAME).a $(NAME)_wrap.o -o _$(NAME).so -framework Accelerate

clean:
	@$(RM) *.o
	@$(RM) *.pyc
	@$(RM) *.so
	@$(RM) $(NAME)_wrap.c
	@$(RM) $(NAME).py

