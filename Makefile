CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic-errors -std=c++23
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lfmt

OBJS = src/main.o

all: clean smile

smile: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

src/main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o src/main.o

clean:
	rm -f smile $(OBJS) && rm -rf ~/.smile

run: smile
		./smile