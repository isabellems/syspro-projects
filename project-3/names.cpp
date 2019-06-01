#include <string.h>
#include "names.h"

SList::SList(){
	front = rear = NULL;
	count = 0;
}

SList::~SList(){
	Node* temp;
	while(front!=NULL){
		temp = front;
		front = front->next;
		delete temp;
	}
}

bool SList::add(char* name,bool dup){
	Node* add;
	if(dup==true){
		add = search(name);
		if(add!=NULL) return false;
	}
	add = new Node();
	strcpy(add->name,name);
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

Node* SList::search(char* name_){
	if(count == 0) return NULL;
	Node* temp=front;
	for(int i=0 ; i<count ; i++){
		if(!strcmp(name_,temp->name)) return temp;
		temp = temp->next;
	}
	return NULL;
}

bool SList::get(char *name_){
	if(count == 0) return false;
	Node* rem = front;
	if(front->next!=NULL){
		front = front->next;
	}
	else{
		front = rear = NULL;
	}
	count--;
	strcpy(name_,rem->name);
	return true;
}

int SList::get_count(){ return count; }