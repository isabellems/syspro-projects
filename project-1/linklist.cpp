#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "graph.h"

using namespace std;

Linklist::Linklist(){
	front = rear = NULL;
	count = 0;
}

Linklist::~Linklist(){
	Lnode *temp;
	while(front!=NULL){
		temp = front;
		front = front->next;
		delete temp->item;
		delete temp;
	}
}

void Linklist::Add(Link &data){ //Adds Link to the doubly linked list
	Lnode *add = new Lnode();
	add->item = &data;
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
	
void Linklist::Remove(Lnode *torem){ //Removes link from the doubly linked list
	Lnode *temp = front;
	for(int i=0; i<count ; i++){
		if(torem==temp){
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
			delete temp->item;
			delete temp;
		}
		temp = temp->next;
	}
}

Lnode * Linklist::Search(unsigned long int head, unsigned long int tail){ //Searches for a link in the list
	if(this->isempty()) return NULL;
	Lnode* temp = front;
	for(int i=0 ; i<count ; i++){
		if(temp->item->gethead()==head && temp->item->gettail()==tail)
			return temp;
		temp = temp->next;
	}
	return NULL;
}

bool Linklist::isempty(){ return count==0; }

void Linklist::print(){ //Prints all links in the list
	if(this->isempty()) return ;
	Lnode* temp = front;
	for(int i=0 ; i<count ; i++){
		temp->item->print();
		if(i!=count-1)
			cout << "," ;
		temp = temp->next;
	}
}

double Linklist::sum(){ //Sums the amounts of all the links in the list
	if(this->isempty()) return 0;
	Lnode* temp = front;
	Link* link;
	double sum=0,amount;
	for(int i=0; i<count ; i++){
		link = temp->item;
		amount = link->getamount();
		sum = sum + amount;
		temp = temp->next;
	}
	return sum;
}

Lnode* Linklist::getfront(){ return front; }

int Linklist::getcount(){ return count; }

Lnode* Linklist::getlnode(int pos){ //Returns a pointer to a specific link in the list
	if(pos >= count) return NULL;
	Lnode* temp = front;
	for(int i=0 ; i<pos ; i++){
		temp = temp->next;
	}
	return temp;
}
