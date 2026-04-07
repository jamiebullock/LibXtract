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

.PHONY: python java

python:
	@swig -I../include -python $(NAME).i
	@$(CC) $(CFLAGS) $(NAME)_wrap.c -o $(NAME)_wrap.o -I$(INCLUDEPY) -I../include
	@$(CC) $(LDFLAGS) ../src/lib$(NAME).a $(NAME)_wrap.o -o _$(NAME).so $(LIBS)

java:
ifndef JAVA_HOME
	$(error JAVA_HOME is not set. Set it to your JDK installation directory)
endif
	@mkdir -p java
	@swig -I../include -java -package $(NAME) -outdir java $(NAME).i
	@$(CC) $(CFLAGS) $(NAME)_wrap.c -o $(NAME)_wrap.o -I$(JAVA_HOME)/include -I$(wildcard $(JAVA_HOME)/include/darwin $(JAVA_HOME)/include/linux $(JAVA_HOME)/include/win32) -I../include
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

