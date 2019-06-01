#ifndef HASH_H
#define HASH_H

#include "stack.h"
#define MAX 10


class Link{
protected:
	unsigned long head;
	unsigned long tail;
	double amount;
public:
	Link(unsigned long,unsigned long,double); 
	unsigned long gethead();
	//Returns the head of a link
	unsigned long gettail();
	//Returns the tail of a link
	double getamount();
	void addamount(double); 
	//Adds an amount to a existing link
	void print();
};

struct Lnode{ // Link-list's node
	Link *item;
	Lnode *previous;
	Lnode *next;
	Lnode(){
		item = NULL;
		next = NULL;
		previous = NULL;
	}
};

class Linklist // List of graph's links
{
private:
	Lnode* front;
	Lnode* rear;
	int count;
public:
	Linklist();
	~Linklist();
	void Add(Link &);
	void Remove(Lnode *);
	Lnode * Search(unsigned long,unsigned long); 
	bool isempty();
	void print();
	double sum();
	//Sums the amount of money of every link in the list
	Lnode* getfront();
	//Returns first link of the list
	int getcount();
	Lnode* getlnode(int);
	//Returns a list's node from a specific position in the list given by the user
};

class Node
{
protected:
	unsigned long id;
	Linklist* linksf; //links from the node
 	Linklist* linkst; // links to the node
public:
	Node(unsigned long id);
	~Node();
	unsigned long getid();
	Linklist* getlinksf();
	Linklist* getlinkst();
	void addlinkf(unsigned long,double);
	//Adds a link to linksf list 
	void addlinkt(unsigned long,double);
	//Adds a link to linkst list
	void removelinkf(unsigned long);
	//Removes a link from linksf list 
	void removelinkt(unsigned long);
	//Removes a link from linkst list 
	double sumin();
	//Returns the amount of money that the node gets
	double sumout();
	//Returns the amount of money that the node gives
	void print();
	//Prints node's id and the links heading from it
};

class Hashtable;

class Graph 
{
protected:
	int size;
	Hashtable* hash;
public:
	Graph(int);
	~Graph();
	int getsize();
	void createnode(unsigned long);
	//Creates a node and puts it in the hashtable
	void delnode(unsigned long);
	//Deletes a node if there are no links from or to it
	bool addtran(unsigned long,unsigned long,double); 
	//Adds a link between two nodes
	bool deltran(unsigned long,unsigned long); 
	//Deletes a link between two nodes
	void lookup(int,unsigned long); 
	//Calculates the amount of money that the node gets/gives/both 
	void searchtr(Stack *,unsigned long,double,bool &); 
	//Searches graph recursively for triangles including the node
	void searchconn(Stack *,unsigned long,unsigned long,bool &); 
	//Searches graph recursively for connections between the nodes
	void searchcycl(Stack *,unsigned long,bool &); 
	//Searches graph recursively for cycles including the node
	void dfs(Stack *,StackD*,unsigned long,int,bool &); 
	//Dfs search
	void triangle(unsigned long,double); 
	//Calls recursive function for triangle search
	void conn(unsigned long,unsigned long);
	//Calls recursive function for connection search
	void allcycles(unsigned long);
	//Calls recursuve function for cycles search
	void traceflow(unsigned long,int);
	//Calls dfs search function for a specific depth
	void print();
	//Prints graph
	void dump(char*);
	//Copies graph to file
};

#endif