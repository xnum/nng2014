#
# From http://www.borngeek.com/2010/05/06/automatic-dependency-generation/
#

SHELL = /bin/bash
TARGET = main
 
ifndef BC
    BC=debug
endif
 
# gnu gcov
Coverage = --coverage -lgcov

# gperf
Gperf = -ltcmalloc_and_profiler

CC = g++
CFLAGS = -Wall -Wextra -std=c++11 -m64 -march=native -msse4.2 -DFP2
LDFLAGS = -lm  #$(Gperf)
 
ifeq ($(BC),debug)
CFLAGS += -g3
else
CFLAGS += -Ofast
endif
 
DEPDIR=deps
OBJDIR=$(BC)/objs
SRCDIR=src
 
OTMP = $(patsubst $(SRCDIR)/%.cpp,%.o,$(wildcard $(SRCDIR)/*.cpp))
OBJS = $(patsubst %,$(OBJDIR)/%,$(OTMP))
DEPS = $(patsubst %.o,$(DEPDIR)/%.d,$(OTMP))
 
all: init $(TARGET)

tag:
	ctags -R src/

init:
	@mkdir -p $(DEPDIR)
	@mkdir -p $(OBJDIR)

main: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)
 
-include $(DEPS)
 
$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
	$(CC) -MM -MT $(OBJDIR)/$*.o $(CFLAGS) $(SRCDIR)/$*.cpp > $(DEPDIR)/$*.d
 
clean:
	rm -fr debug/*
	rm -fr release/*
