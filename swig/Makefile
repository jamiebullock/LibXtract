NAME = xtract

OS := $(shell uname)

# Assume that since we're building the python bindings, we have a python installed!
INCLUDEPY = $(shell python -m sysconfig | grep -w INCLUDEPY | awk '{print $$3}')

ifeq ($(OS), Darwin)
    CFLAGS=-g -c
    LDFLAGS=-bundle -flat_namespace -undefined suppress
    LIBS=-framework Accelerate
    JAVA_LIB_SUFFIX=dylib
else
    CFLAGS=-g -c -fPIC
    LDFLAGS=-shared
    LIBS=
    JAVA_LIB_SUFFIX=so
endif

JAVA_HOME ?= $(shell java -XshowSettings:properties -version 2>&1 | grep 'java.home' | awk '{print $$3}')
JAVA_INCLUDE = $(JAVA_HOME)/include
JAVA_INCLUDE_OS = $(wildcard $(JAVA_INCLUDE)/darwin $(JAVA_INCLUDE)/linux $(JAVA_INCLUDE)/win32)

.PHONY: python java

python:
	@swig -I../include -python $(NAME).i
	@$(CC) $(CFLAGS) $(NAME)_wrap.c -o $(NAME)_wrap.o -I$(INCLUDEPY) -I../include
	@$(CC) $(LDFLAGS) ../src/lib$(NAME).a $(NAME)_wrap.o -o _$(NAME).so $(LIBS)

java:
	@mkdir -p java
	@swig -I../include -java -package libxtract -outdir java $(NAME).i
	@$(CC) $(CFLAGS) $(NAME)_wrap.c -o $(NAME)_wrap.o -I$(JAVA_INCLUDE) -I$(JAVA_INCLUDE_OS) -I../include
	@$(CC) $(LDFLAGS) ../src/lib$(NAME).a $(NAME)_wrap.o -o lib$(NAME)_java.$(JAVA_LIB_SUFFIX) $(LIBS)
	@javac java/*.java

install: python
	@SITE_DIR=$$(python -c "import site; print(site.getsitepackages()[0])") && \
	install -d "$$SITE_DIR" && \
	install _$(NAME).so "$$SITE_DIR" && \
	install -m 644 $(NAME).py "$$SITE_DIR"

clean:
	@$(RM) *.o
	@$(RM) *.pyc
	@$(RM) *.so
	@$(RM) *.dylib
	@$(RM) $(NAME)_wrap.c
	@$(RM) $(NAME).py
	@$(RM) -r java

