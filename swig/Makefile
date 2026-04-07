NAME = xtract

OS := $(shell uname)

PYTHON ?= python3
INCLUDEPY = $(shell $(PYTHON) -m sysconfig | grep -w INCLUDEPY | awk '{print $$3}')
PYTHON_LDFLAGS = $(shell $(PYTHON)-config --ldflags --embed 2>/dev/null || echo "")

PYTHON_EXT_SUFFIX = $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX') or '.so')" 2>/dev/null)

ifeq ($(OS), Darwin)
    CFLAGS=-g -c
    LDFLAGS=-bundle -flat_namespace -undefined suppress
    LIBS=-framework Accelerate
    JAVA_LIB_SUFFIX=dylib
    LIBXTRACT=../src/lib$(NAME).a
    LINK_LIB=$(LIBXTRACT)
else
    CFLAGS=-g -c -fPIC
    LDFLAGS=-shared
    LIBS=-lm
    JAVA_LIB_SUFFIX=so
    LIBXTRACT=$(wildcard ../src/lib$(NAME).a ../src/lib$(NAME).lib)
    LINK_LIB=-Wl,--whole-archive $(LIBXTRACT) -Wl,--no-whole-archive
endif

NODE_INCLUDE = $(shell node -e "\
  var fs = require('fs'), p = process.config.variables.node_prefix + '/include/node'; \
  if (fs.existsSync(p)) { console.log(p); } \
  else { var h = require('os').homedir() + '/.node-gyp/' + process.versions.node + '/include/node'; \
  if (fs.existsSync(h)) { console.log(h); } \
  else { console.log(p); } }" 2>/dev/null)
NAPI_INCLUDE = $(shell node -e "console.log(require('node-addon-api').include)" 2>/dev/null | tr -d '"')

.PHONY: python java node check check-python check-java check-node

python:
	@swig -I../include -python $(NAME).i
	@$(CC) $(CFLAGS) $(NAME)_wrap.c -o $(NAME)_wrap.o -I$(INCLUDEPY) -I../include
	@$(CC) $(LDFLAGS) $(LINK_LIB) $(NAME)_wrap.o -o _$(NAME)$(PYTHON_EXT_SUFFIX) $(LIBS) $(PYTHON_LDFLAGS)

java:
ifndef JAVA_HOME
	$(error JAVA_HOME is not set. Set it to your JDK installation directory)
endif
	@mkdir -p java
	@swig -I../include -java -package $(NAME) -outdir java $(NAME).i
	@$(CC) $(CFLAGS) $(NAME)_wrap.c -o $(NAME)_wrap.o -I$(JAVA_HOME)/include -I$(wildcard $(JAVA_HOME)/include/darwin $(JAVA_HOME)/include/linux $(JAVA_HOME)/include/win32) -I../include
	@$(CC) $(LDFLAGS) $(LINK_LIB) $(NAME)_wrap.o -o libj$(NAME).$(JAVA_LIB_SUFFIX) $(LIBS)
	@javac java/*.java

node:
	@swig -I../include -javascript -napi $(NAME).i
	@$(CXX) -std=c++17 $(CFLAGS) $(NAME)_wrap.c -o $(NAME)_wrap.o -I$(NODE_INCLUDE) -I$(NAPI_INCLUDE) -I../include
	@$(CXX) $(LDFLAGS) $(LINK_LIB) $(NAME)_wrap.o -o $(NAME).node $(LIBS)

check: check-python check-java check-node

check-python: python
	@echo "Running Python bindings test..."
	@PYTHONPATH=. $(PYTHON) test.py

check-java: java
	@echo "Running Java bindings test..."
	@mkdir -p $(NAME)
	@cp java/*.class $(NAME)/
	@javac -cp . test.java
	@java -Djava.library.path=. -cp . test
	@$(RM) -r $(NAME) test.class

check-node: node
	@echo "Running Node.js bindings test..."
	@node test.js

install: python
	@SITE_DIR=$$($(PYTHON) -c "import site; print(site.getsitepackages()[0])") && \
	install -d "$$SITE_DIR" && \
	install _$(NAME)$(PYTHON_EXT_SUFFIX) "$$SITE_DIR" && \
	install -m 644 $(NAME).py "$$SITE_DIR"

clean:
	@$(RM) *.o
	@$(RM) *.pyc
	@$(RM) *.so
	@$(RM) *.dylib
	@$(RM) *.node
	@$(RM) $(NAME)_wrap.c
	@$(RM) $(NAME).py
	@$(RM) -r java

