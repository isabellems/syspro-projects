#include <iostream>
#include "stack.h"

using namespace std; 

Stack::Stack(){
	count = 0;
	front = NULL;
	rear = NULL;
}

Stack::~Stack(){
	Snode *temp;
	while(front!=NULL){
		temp = front;
		front = front->next;
		delete temp;
	}
}

void Stack::push(unsigned long id_){
	Snode *push = new Snode();
	push->id = id_;
	if(front==NULL){
		front = push;
		rear = front;
	}
	else{
		rear->next = push;
		push->previous = rear;
		push->next = NULL;
		rear = push;
	}
	count++;
}

void Stack::pop(){
	if(front==NULL) return ;
	Snode *pop;
	pop = rear;
	count--;
	if(front->next != NULL){
		rear = rear->previous;
		rear->next = NULL;
	}
	else{
		front = rear = NULL;
	}
	delete pop;
}

int Stack::getcount(){ return count; }

unsigned long int Stack::getfront(){ return front->id; }

unsigned long int Stack::getrear(){ return rear->id; }

bool Stack::isempty(){ return front==NULL; }

void Stack::print(bool flag){ //Prints stack from front to rear
	if(this->isempty()) return;
	Snode *temp;
	temp = front;
	cout << "(" ;
	for(int i=0 ; i<count ; i++){
		cout << temp->id;
		if(i<count-1) 
			cout << "," ;
		temp = temp->next;
	}
	if(flag == true)
		cout << ")" << endl;
	else
		cout << ",";
}


StackD::StackD(){
	count = 0;
	front = NULL;
	rear = NULL;
}

StackD::~StackD(){
	SnodeD *temp;
	while(front!=NULL){
		temp = front;
		front = front->next;
		delete temp;
	}
}

void StackD::push(double amount_){
	SnodeD *push = new SnodeD();
	push->amount = amount_;
	if(front==NULL){
		front = push;
		rear = front;
	}
	else{
		rear->next = push;
		push->previous = rear;
		push->next = NULL;
		rear = push;
	}
	count++;
}

double StackD::pop(){
	if(front==NULL) return -1;
	SnodeD *pop;
	double am;
	pop = rear;
	count--;
	if(front->next != NULL){
		rear = rear->previous;
		rear->next = NULL;
	}
	else{
		front = rear = NULL;
	}
	am = pop->amount;
	delete pop;
	return am;
}

int StackD::getcount(){ return count; }

double StackD::getfront(){ return front->amount; }

double StackD::getrear(){ return rear->amount; }

bool StackD::isempty(){ return front==NULL; }

double StackD::sum(){ //Sums all the amounts in the stack
	if(this->isempty()) return -1;
	SnodeD *temp;
	temp = front;
	double sum = 0;
	for(int i=0 ; i<count ; i++){
		sum = sum + temp->amount;
		temp = temp->next;
	}
	return sum;
}