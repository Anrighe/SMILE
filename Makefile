CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic-errors -std=c++23 -I/usr/include
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lfmt -lboost_system -lboost_filesystem -lboost_program_options -lSQLiteCpp -lsqlite3

OBJS = src/main.o

all: clean smile

smile: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

src/main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o src/main.o

clean:
	rm -f smile $(OBJS)
#	rm -f smile $(OBJS) && rm -rf ~/.smile

run: smile
		./smile