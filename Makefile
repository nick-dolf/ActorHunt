all: main

CXX = clang++
override CXXFLAGS += -g -Wno-everything

SRCS = $(shell find . -name '.ccls-cache' -type d -prune -o -type f -name '*.cpp' -print | sed -e 's/ /\\ /g')
LIB = -lncurses

main: $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(LIB) -o "$@"

main-debug: $(SRCS)
	$(CXX) $(CXXFLAGS) -O0 $(SRCS) $(LIB) -o "$@"

clean:
	rm -f main main-debug