CXX = g++
CXXFLAGS = -Wall -ggdb -std=c++11

treeplanner: main.o Item.o UniqueId.o
	$(CXX) $(CXXFLAGS) -o treeplanner main.o Item.o UniqueId.o -lncurses

main.o: main.cpp
	$(CXX) -c main.cpp

Item.o: Item.cpp
	$(CXX) -c Item.cpp

UniqueId.o: UniqueId.cpp
	$(CXX) -c UniqueId.cpp

clean:
	rm main.o Item.o UniqueId.o
