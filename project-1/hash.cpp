#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "graph.h"
#include "hash.h"

using namespace std; 

Bucket::Bucket(){
	front = rear = NULL;
	count = 0;
}

Bucket::~Bucket(){
	Bnode *temp;
	while(front!=NULL){
		temp = front;
		front = front->next;
		delete temp->item;
		delete temp;
	}
}

void Bucket::add(Node &node){ //Adds a member to the doubly linked list
	Bnode *add = new Bnode();
	add->item = &node;
	if(front==NULL){
		front = add;
	}
	else{
		rear->next = add;
		add->previous = rear;
	}
	rear = add;
	count++;
}

Node * Bucket::Remove(Bnode *torem){ //Removes a member from the doubly linked list
	Bnode *temp = front;
	for(int i=0; i<count ; i++){
		if(torem==temp){
			Node *node = temp->item;
			if (node->getlinksf()->isempty() && node->getlinkst()->isempty()){
				if(temp->previous==NULL && temp->next==NULL){
					rear=front=NULL;
				}
				else if(temp->previous!=NULL && temp->next!=NULL){
					temp->previous->next = temp->next;
					temp->next->previous = temp->previous;
				}
				else if(temp->previous==NULL){
					front = front->next;
					front->previous = NULL;
				}
				else{
					rear = temp->previous;
					rear->next = NULL;
				}
				count--;
				delete temp;
				return node;
			}
			else
				return NULL;
		}
		temp = temp->next;
	}
	return NULL;
}

Bnode * Bucket::Search(unsigned long int id,bool flag){ //Searches for a node in the bucket
	if(this->isempty()) return NULL; 
	Bnode* temp = front;
	for(int i=0 ; i<count ; i++){
		if(temp->item->getid()==id){
			if(flag == true){ //If flag==true returns the node if it is not visited and then mark it as visited
				if(temp->visited == false){ 
					temp->visited = true;
					return temp;
				}
				else{ //If flag==false just returns the node
					return NULL;
				}
			}
			else{
				return temp;
			}
		}
		temp = temp->next;
	}
	return NULL;
} 

bool Bucket::isempty(){ return count==0; }

void Bucket::print(){ //Prints all the nodes in the bucket
	if(this->isempty()) return ;
	Bnode* temp = front;
	for(int i=0 ; i<count ; i++){
		temp->item->print();
		temp = temp->next;
		cout << endl;
	}
}

void Bucket::reset(){ 
	if(this->isempty()) return ;
	Bnode* temp = front;
	for(int i=0 ; i<count ; i++){ //For all the nodes in the bucket
		temp->visited = false; //Marks as not visited
		temp = temp->next;
	}
}

void Bucket::unmark(unsigned long id){
	if(this->isempty()) return ;
	Bnode *node = Search(id,false); //Finds a specific node in the bucket and marks it as not visited
	if(node->visited == true)
		node->visited = false;
}

Node* Bucket::getnode(int pos){ 
	if(pos >= count) return NULL;
	Bnode* temp = front;
	for(int i=0 ; i<pos ; i++){ //Gets a node with a specific position in the bucket
		temp = temp->next;
	}
	return temp->item;
}

int Bucket::getcount(){ return count; }

Hashtable::Hashtable(int size_){
	size = size_;
	array = new Bucket*[size];
	for(int i=0; i<size_ ; i++){
		array[i] = new Bucket();
	}
}

Hashtable::~Hashtable(){
	for(int i=0 ; i<size ; i++){
		delete array[i]; 
	}
	delete[] array;
}

int Hashtable::getsize(){ return size; }

void Hashtable::addNode(Node* node){
	unsigned long int id =  node->getid();
	int bucket = id%size; //Applies hash function
	array[bucket]->add(*node); //Adds node to the proper bucket
}

Node * Hashtable::Search(unsigned long int id,bool flag){
	Bnode *node;
	int bucket = id%size; //Applies hash function
	node = array[bucket]->Search(id,flag);//Searches for the node in the proper bucket
	if(node!=NULL)
		return node->item;
	else
		return NULL;
}

Node * Hashtable::Remove(unsigned long int id){
	Bnode *node;
	Node *rem;
	int bucket = id%size; //Applies hash function 
	node = array[bucket]->Search(id,false);  
	this->reset();
	if(node!=NULL){
		rem = array[bucket]->Remove(node); //Removes the node from the proper bucket
		return rem;
	}
	else
		return NULL;
}

void Hashtable::print(){ //Prints all buckets
	for(int i=0 ; i<size; i++){
		array[i]->print();
	}
}

void Hashtable::reset(){ //Marks all nodes as not visited
	for(int i=0 ; i<size ; i++){
		array[i]->reset();
	}
}

void Hashtable::unmark(unsigned long id){ //Marks a specific node as not visited
	int bucket = id%size;
	array[bucket]->unmark(id);
}

Node *Hashtable::getnode(int i,int pos){ //Returns a pointer to a specific node
	Node* node = array[i]->getnode(pos);
	return node;
}

bool Hashtable::isempty(){ //Checks if all buckets are emoty
	for(int i=0; i<size ; i++){
		if(array[i]->isempty()==false)
			return false;
	}
	return true;
}

int Hashtable::getcount(int i){ return array[i]->getcount(); } //Returns the size of a specific bucket
