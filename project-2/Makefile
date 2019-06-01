all: board boardpost

board: board.o channel.o
	g++ board.o channel.o -o board

board.o: board.cpp
	g++ -c board.cpp

channel.o: channel.cpp
	g++ -c channel.cpp

boardpost: boardpost.o
	g++ boardpost.o -o boardpost

boardpost.o: boardpost.cpp
	g++ -c boardpost.cpp

clean:
	rm *o board boardpost
