all: bankserver bankclient

bankserver: bankserver.o queue.o hashtable.o transfers.o names.o
	g++ bankserver.o queue.o hashtable.o transfers.o names.o -o bankserver -pthread

bankclient: bankclient.o queue.o hashtable.o transfers.o
	g++ bankclient.o queue.o hashtable.o transfers.o -o bankclient

hashtable.o: hashtable.cpp
	g++ -c hashtable.cpp

transfers.o: transfers.cpp
	g++ -c transfers.cpp

bankserver.o: bankserver.cpp
	g++ -c bankserver.cpp -pthread

bankclient.o: bankclient.cpp
	g++ -c bankclient.cpp

queue.o: queue.cpp
	g++ -c queue.cpp

names.o: names.cpp
	g++ -c names.cpp

clean:
	rm *o bankserver bankclient