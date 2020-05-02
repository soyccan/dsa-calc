CXX := clang++
CXXFLAGS := -Wall -Wextra -Wconversion -std=gnu++17
LDFLAGS :=

IWYU := /usr/local/bin/include-what-you-use # this will check correct includes
IWYU-FLAGS := --check_also # flags for include-what-you-see

GIT_HOOKS := .git/hooks/applied

# header files and pre-compiled headers files
HDRS := expression.hpp parser.hpp operatorlist.hpp
PRECOMP-HDRS := $(HDRS:%.hpp=%.hpp.gch)
TARGETS := hw3_1 hw3_2
OBJS := hw3_1.o hw3_2.o
deps := $(OBJS:%.o=.%.o.d)

ifeq ($(IWYU), 1)
	# include-what-you-see
	CXX := $(IWYU)
	CXXFLAGS += $(IWYU-FLAGS)
endif

ifndef DEBUG
	DEBUG := 1
endif
ifeq ($(DEBUG), 1)
	# C++ boost
	CXXFLAGS += -I/usr/local/include/

	CXXFLAGS += -g
else
	CXXFLAGS += -DNDEBUG
endif


.PHONY: all clean
all: $(GIT_HOOKS) hw3_1 hw3_2

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

$(TARGETS): %: %.o
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ -MMD -MF .$@.d $<

$(PRECOMP-HDRS): %.hpp.gch: %.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ -MMD -MF .$@.d $<

run: hw3_1
	./hw3_1 < in.txt

clean:
	rm -rf $(OBJS) $(deps) $(TARGETS)

-include $(deps)
