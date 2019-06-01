
all: elegxos

elegxos: main.o graph.o hash.o linklist.o stack.o
	g++ main.o graph.o hash.o linklist.o stack.o -o elegxos

main.o: main.cpp
	g++ -c main.cpp

graph.o: graph.cpp
	g++ -c graph.cpp

hash.o: hash.cpp
	g++ -c hash.cpp

linklist.o: linklist.cpp
	g++ -c linklist.cpp

stack.o: stack.cpp
	g++ -c stack.cpp

clean:
	rm *o elegxos