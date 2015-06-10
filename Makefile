
.PHONY: all clean dep-clean

# enable to use zhash(need third-part lib) or c++ stl map
USE_ZHASH=#-DZHASH
CZMQ_DIR=#.

# lineSolving optimize option
USE_CUT_BY_SIZE=-DCUT_BY_SIZE
USE_CUT_BY_CACHE=-DCUT_BY_CACHE

# use mirror method
USE_MIRROR_REDUCE=#-DMIRROR

# enable fp2 or fp1
USE_FP2=-DUSE_FP2

LINESOLVE_OPT=$(USE_CUT_BY_SIZE) $(USE_CUT_BY_CACHE)
DEF=$(USE_ZHASH) $(LINESOLVE_OPT) $(USE_FP2) $(USE_MIRROR_REDUCE)

#===================================

OBJ_DIR=obj
DEP_DIR=deps

CXX=g++
INCFLAGS=
LDFLAGS=-lm 
CPPFLAGS=-std=c++11 -m64 -msse4.2 -march=native $(DEF) -Wall -Wextra $(INCFLAGS) -DNDEBUG -Ofast

SRCS=$(shell ls *.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))
DEPS=$(patsubst %.cpp,$(DEP_DIR)/%.d,$(SRCS))

all: main
main: $(OBJS)
	$(CXX)  $(CPPFLAGS) -o $@ $^ $(LDFLAGS)


$(DEP_DIR)/%.d: %.cpp
	@rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@ 

-include $(DEPS)

clean:
	@rm -rf main main_release main_prof main_debug *.o

dep-clean:
	@rm -rf main main_release main_prof main_debug *.o $(DEP_DIR)/*.d*
