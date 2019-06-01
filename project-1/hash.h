#ifndef GRAPH_H
#define GRAPH_H

struct Bnode //Bucket's node
{
	Node *item;
	Bnode *previous;
	Bnode *next;
	bool visited;
	Bnode(){
		item = NULL;
		previous = NULL;
		next = NULL;
		visited = false;
	}
};

class Bucket{
protected:
	Bnode* front;
	Bnode* rear;
	int count;
public:
	Bucket();
	~Bucket();
	void add(Node &);
	Node * Remove(Bnode *);
	Bnode * Search(unsigned long,bool); 
	//Searches a node and returns a pointer to it.If the second argument is true, marks node as visited
	bool isempty();
	void print();
	//Prints all the nodes in the bucket
	void reset();
	//Sets all nodes in the bucket as not visited
	void unmark(unsigned long);
	//Sets a node as not visited
	Node* getnode(int);
	//Returns a node from a specific position in the bucket
	int getcount();
	//Returns bucket's size
};


class Hashtable{
protected:
	int size; //Size given from the user
	Bucket** array; //Dynamic array of buckets
public:
	Hashtable(int);
	~Hashtable();
	int getsize();
	//Returns hashtable's size
	void addNode(Node*);
	Node * Search(unsigned long,bool);
	//Searches a node and returns a pointer to it.If the second argument is true, marks node as visited
	Node * Remove(unsigned long);
	void print();
	//Prints all buckets
	void reset();
	//Sets all nodes as not visited
	void unmark(unsigned long);
	//Sets a node as not visited
	Node * getnode(int,int);
	//Returns a node from a specific position in its bucket
	bool isempty();
	//Checks if hashtable is empty
	int getcount(int);
	//Returns a specific bucket's size
};

#endif