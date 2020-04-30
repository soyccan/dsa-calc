CC := clang
CXX := clang++
CFLAGS := -Wall -Wextra -Wconversion -std=gnu17
CXXFLAGS := -Wall -Wextra -Wconversion -std=gnu++17
LDFLAGS :=

GIT_HOOKS := .git/hooks/applied

# header files and pre-compiled headers files
HDRS := expression.hpp parser.hpp operatorlist.hpp
PRECOMP-HDRS := $(HDRS:%.hpp=%.hpp.gch)
OBJS := hw3_1.o
deps := $(OBJS:%.o=.%.o.d)

ifndef DEBUG
	DEBUG := 1
endif
ifeq ($(DEBUG), 1)
	CXXFLAGS += -g
else
	CXXFLAGS += -DNDEBUG
endif


.PHONY: all clean
all: $(GIT_HOOKS) hw3_1 hw3_2

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

hw3_1: hw3_1.o
	$(CXX) $(LDFLAGS) -o $@ $^

hw3_2:
	@true

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ -MMD -MF .$@.d $<

$(PRECOMP-HDRS): %.hpp.gch: %.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ -MMD -MF .$@.d $<

run: hw3_1
	./hw3_1 < in.txt

clean:
	rm -rf $(OBJS) $(deps) hw3_1 hw3_2

-include $(deps)
