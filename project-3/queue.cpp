#include <cstdlib>
#include "queue.h"

Queue::Queue(int size){
	front = rear = NULL;
	count = 0;
	max_size = size; 
}

Queue::~Queue(){
	Qnode* temp;
	while(front!=NULL){
		temp = front;
		front = front->next;
		delete temp;
	}
}

bool Queue::enqueue(int sock){
	if(count == max_size) return false;
	Qnode* add = new Qnode(sock);
	if(front==NULL){
		front = add;
	}
	else{
		rear->next = add;
	}
	rear = add;
	count ++;
	return true;
}

int Queue::dequeue(){
	if(count==0) return -1;
	Qnode* rem = front;
	if(front->next!=NULL){
		front = front->next;
	}
	else{
		front = rear = NULL;
	}
	count--;
	return rem->newsock;
}

int Queue::get_count(){ return count; }

int Queue::get_size(){return max_size; }

bool Queue::is_full(){ return count==max_size; }

bool Queue::is_empty(){ return count<=0; }