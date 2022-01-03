CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -pthread

all: BGSclient
	g++ -o bin/BGSclient bin/connectionHandler.o bin/Client.o bin/reader.o bin/writer.o $(LDFLAGS)

BGSclient: bin/connectionHandler.o bin/Client.o bin/reader.o bin/writer.o
	
bin/connectionHandler.o: src/connectionHandler.cpp
	g++ $(CFLAGS) -o bin/connectionHandler.o src/connectionHandler.cpp

bin/Client.o: src/Client.cpp
	g++ $(CFLAGS) -o bin/Client.o src/Client.cpp

bin/reader.o: src/reader.cpp
	g++ $(CFLAGS) -o bin/reader.o src/reader.cpp

bin/writer.o: src/writer.cpp
	g++ $(CFLAGS) -o bin/writer.o src/writer.cpp

.PHONY: clean
clean:
	rm -f bin/*
