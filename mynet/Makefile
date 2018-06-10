
SRC = ./*.cpp *.cc *c

CXXFLAGS = -g -O0 -Wall -Wextra -Werror \
	   -Wconversion -Wno-unused-parameter \
	   -Wold-style-cast -Woverloaded-virtual \
	   -Wpointer-arith -Wshadow -Wwrite-strings \
	   -march=native -rdynamic -std=c++17\

LDFLAGS = -lpthread -lrt

all: obj
clean:
	rm -f *.o
	rm obj

obj: $(SRC)
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: all clean
