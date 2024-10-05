CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -pedantic-errors -std=c++23 -I/usr/include
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lfmt -lboost_system -lboost_filesystem -lboost_program_options -lSQLiteCpp -lsqlite3

OBJS = src/main.o
TARGET = ./dist/

all: clean smile dist

smile: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

src/main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o src/main.o

clean:
	rm -f smile $(OBJS) && rm -rf dist
#	rm -f smile $(OBJS) && rm -rf ~/.smile

test: smile
	./smile --v --i ech

dist: smile
	@mkdir dist
	@cp -r ./initializer $(TARGET)
	@cp ./smile $(TARGET)

install: all
	@sudo mv $(TARGET) /usr/bin/smile
	@echo "Installation complete. Smile is available in /usr/bin"
