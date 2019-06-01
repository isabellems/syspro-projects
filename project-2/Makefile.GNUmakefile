all: thing 

thing: hashtable.o structures.o lolz.o
	g++ hashtable.o structures.o lolz.o -o thing

hashtable.o: hashtable.cpp
	g++ -c hashtable.cpp

structures.o: structures.cpp
	g++ -c structures.cpp

lolz.o: lolz.cpp
	g++ -c lolz.cpp

clean:
	rm *o thing