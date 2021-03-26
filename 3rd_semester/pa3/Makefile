
CXX = g++
CXX_FLAGS = -std=c++11 -Wall -O0

FILES = HashUtils.o Graph.o main.cpp

all:
	$(CXX) $(CXX_FLAGS) -c HashUtils.cpp -o HashUtils.o
	$(CXX) $(CXX_FLAGS) -c Graph.cpp -o Graph.o
	$(CXX) $(CXX_FLAGS) $(FILES) -o Test


clean:
	rm -f Test
	rm -f *.o

g++ -std=c++11 -Wall -O0 -c HashUtils.cpp -o HashUtils.o
g++ -std=c++11 -Wall -O0 -c Graph.cpp -o Graph.o
g++ -std=c++11 -Wall -O0 -c main.cpp -o main.o
g++ HashUtils.o Graph.o main.o -o testing