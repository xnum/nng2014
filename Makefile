
.PHONY: all clean dep-clean

# use mirror method
USE_MIRROR_REDUCE=#-DMIRROR

# enable fp2 or fp1
USE_FP2=-DUSE_FP2

DEF=$(USE_FP2) $(USE_MIRROR_REDUCE)


COVERAGE_OPT=--coverage -lgcov
#===================================

OBJ_DIR=obj
DEP_DIR=deps

CXX=g++
INCFLAGS=
LDFLAGS=-lm 
OPTFLAGS=
CPPFLAGS=-std=c++11 -m64 -msse4.2 -march=native $(DEF) -Wall -Wextra $(INCFLAGS) -DNDEBUG -Ofast # $(COVERAGE_OPT)

SRCS=$(shell ls *.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))
DEPS=$(patsubst %.cpp,$(DEP_DIR)/%.d,$(SRCS))

all: main tag
main: $(OBJS)
	$(CXX)  $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

tag:
	ctags -R *


$(DEP_DIR)/%.d: %.cpp
	@rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@ 

-include $(DEPS)

clean:
	@rm -rf main main_release main_prof main_debug *.o *.gcno *.gcda *.gcov

dep-clean:
	@rm -rf main main_release main_prof main_debug *.o $(DEP_DIR)/*.d*
