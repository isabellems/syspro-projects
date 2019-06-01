#include <iostream>
#include <cstdlib>
#include <cstring>
#include "hashtable.h"

using namespace std;

//Account functions

Account::Account(long amount,char* name_){
	transfers = new List();
	balance = amount;
	strcpy(name,name_);
}

Account::~Account(){
	delete transfers;
}

bool Account::add_transfer(char* name,long amount){
	transfers->add_trans(name,amount); //adds transfer to the list of transfers
	balance+=amount; //increases the balance
	return true;
}

long Account::get_balance(){ return balance; }

void Account::get_name(char* name_){
	strcpy(name_,name);
};

List* Account::getlist(){ return transfers; }

void Account::print_balance(){
	cout << "Name: " << name << " Balance: " << balance << " : " ;
	transfers->print();
}

void Account::red_balance(long amount){ //reduces the balance
	balance-=amount;
}

//Bucket functions

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

void Bucket::add_account(Account* acc){
	Bnode *add = new Bnode();
	add->item = acc;
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

bool Bucket::is_empty(){ return count==0; }

Bnode* Bucket::search(char* name){
	if(this->is_empty()) return NULL; 
	Bnode* temp=front;
	char name_[50];
	for(int i=0 ; i<count ; i++){
		temp->item->get_name(name_);
		if(!strcmp(name_,name)) return temp;
		temp = temp->next;
	}
	return NULL;
}

//Hashtable functions

Hashtable::Hashtable(int size_){
	size = size_;
	count = 0;
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

int Hashtable::hash(char* str){ //returns a hash value for a string
	unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c 
    return (unsigned int)hash%this->size;
}

bool Hashtable::add_account(char* name,long init_bal){
	if(count!=0)
		if(search(name)!=NULL) return false; //if an account with the same exists returns false
	Account* acc = new Account(init_bal,name); //create a new account
	int hash = this->hash(name); // get hash value
	array[hash]->add_account(acc); //add account to the right bucket
	count++;
	return true;
}

Account* Hashtable::search(char* acc){ 
	int hash = this->hash(acc);
	Bnode* bn = array[hash]->search(acc);
	if(bn==NULL) return NULL;
	return bn->item;
}

bool Hashtable::print_balance(char* name_,long &bal){
	Account *acc = search(name_);
	if(acc == NULL){ //if account doesn't exist
		return false;
	}
	else{
		bal = acc->get_balance(); //returns balance
		return true;
	}
}

int Hashtable::get_size(){ return size; }

bool Hashtable::add_transfer(char* receiver,char* sender,long amount){	
	int hash = this->hash(sender); //get hash value
	Bnode* send = array[hash]->search(sender); //checks if source exists
	if(!strcmp(receiver,sender)) return false;
	if(send == NULL){
		return false;
	}
	if(send->item->get_balance() < amount){ //if balance if not sufficient
		return false;
	}
	hash = this->hash(receiver);
	Bnode* rec = array[hash]->search(receiver); //checks if destination exists
	if(rec == NULL){
		return false;
	}
	if(rec->item->add_transfer(sender,amount)){ //if transfer was successful
		send->item->red_balance(amount);	//reduces sender's balance
		return true;
	}
	return false;
}

bool Hashtable::balance_suff(char* sender,long amount){ 
	Account* acc = this->search(sender); //checks if sender exists
	if(acc == NULL) return false;
	if(acc->get_balance() < amount) return false; //if balance is not sufficient return false
	else return true;
}
