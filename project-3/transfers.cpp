#include <iostream>
#include <cstdlib>
#include <cstring>
#include "transfers.h"


using namespace std;

//Transfer functions

Transfer::Transfer(long amount_,char* name_){
	amount = amount_; 
	strcpy(name,name_);
}

void Transfer::get_name(char* name_){
	strcpy(name_,name);
}

long Transfer::get_amount(){
	return amount;
}

void Transfer::print(){
	cout << "(" << name << "," << amount << ")" ;
}

void Transfer::add_amount(long am){ //adds an amount to an existing transfer
	amount+=am;
}

// List functions

List::List(){
	front = rear = NULL;
	count = 0;
}

List::~List(){
	Lnode *temp;
	while(front!=NULL){
		temp = front;
		front = front->next;
		delete temp->item;
		delete temp;
	}
}

Lnode* List::search(char* name){ //searches for a transfer in the list
	if(this->is_empty()) return NULL;
	Lnode* temp=front;
	char name_[50];
	for(int i=0 ; i<count ; i++){
		temp->item->get_name(name_);
		if(!strcmp(name_,name)) return temp;
		temp = temp->next;
	}
	return NULL;
}

void List::add_trans(char* name,long amount){
	Lnode *ser = NULL;
	if((ser=this->search(name))!=NULL){ //checks if a transfer already exists
		ser->item->add_amount(amount); //adds the amount to the existing transfer
		return;
	}
	Lnode* add = new Lnode();
	Transfer* tran = new Transfer(amount,name);
	add->item = tran;
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

void List::print(){
	Lnode* temp = front;
	for(int i=0 ; i<count ; i++){
		temp->item->print();
		cout << "," << endl;
	}
	cout << endl;
}

bool List::is_empty(){ return count==0; }
