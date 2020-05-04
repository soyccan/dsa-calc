CXX := clang++
CXXFLAGS := -Wall -Wextra -Wconversion -std=gnu++17
LDFLAGS :=

IWYU := /usr/local/bin/include-what-you-use # this will check correct includes
IWYU-FLAGS := --check_also # flags for include-what-you-see

GIT_HOOKS := .git/hooks/applied

# header files and pre-compiled headers files
HDRS := expression.hpp parser.hpp operatorlist.hpp node.hpp
PRECOMP-HDRS := $(HDRS:%.hpp=%.hpp.gch)
TARGETS := hw3_1 hw3_2
OBJS := hw3_1.o hw3_2.o
deps := $(OBJS:%.o=.%.o.d)

ifeq ($(IWYU), 1)
	# include-what-you-see
	CXX := $(IWYU)
	CXXFLAGS += $(IWYU-FLAGS)
endif

ifdef STACK
	CXXFLAGS += -DSHOW_STACK
endif

ifndef DEBUG
	DEBUG := 1
endif
ifeq ($(DEBUG), 1)
	# C++ boost
	CXXFLAGS += -I/usr/local/include/

	CXXFLAGS += -g
else
	CXXFLAGS += -DNDEBUG -O2
endif


.PHONY: all clean
all:
ifeq ($(shell hostname),soyccanmac.local)
	$(MAKE) upload
else
	$(MAKE) $(TARGETS)
endif

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

$(TARGETS): %: %.o
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ -MMD -MF .$@.d $<

$(PRECOMP-HDRS): %.hpp.gch: %.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ -MMD -MF .$@.d $<

upload:
	scp -P 9453 -r *.hpp *.h *.cpp test/ Makefile soyccan@bravo.nctu.me:/home/soyccan/Documents/dsa-hw3/

clean:
	rm -rf $(OBJS) $(deps) $(TARGETS)

-include $(deps)
