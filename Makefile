CXX = g++
CXXFLAGS = -std=c++11 -pthread

all: server client

server: server.cpp game.cpp
	$(CXX) $(CXXFLAGS) -o server server.cpp game.cpp

client: client.cpp game.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp game.cpp

clean:
	rm -f server client
