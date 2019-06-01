#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include "channel.h"

using namespace std;


Queue::Queue(){
	front = rear = NULL;
	count = 0;
}

Queue::~Queue(){
	Message *temp;
	while(front!=NULL){
		temp = front;
		front = front->next;
		delete temp;
	}
}

void Queue::add(Message *mes){ //Adds message to the queue
	if(front==NULL){ 
		front = mes;
		front->next = NULL;
		front->previous = NULL;
	}
	else{
		rear->next = mes;
		mes->previous = rear;
		mes->next = NULL;
	}
	rear = mes;
	count++;
}


Message* Queue::remove(char *fbuff){ //Removes message from the queue,returns it, and returns it's file bile buffer as an argument(so it can be freed)
	if(front == NULL){ return NULL;}
	Message *rem=front;
	count--;
	if(front->next != NULL){
		front = front->next;
		return rem;
	}
	else{
		front = rear = NULL;
	}
	if(rem->file){
		fbuff =(rem->fbuffer);
	}
	return rem;
}

bool Queue::isempty(){ return count==0 ; }

int Queue::getcount(){ return count ; }

Channel::Channel(long id_,char* name_){
	id = id_;
	strcpy(name,name_);
	queue = new Queue();
}

Channel::~Channel(){
	delete queue;
}

void Channel::addmessage(Message *mess){ //Adds Message to a channel
	queue->add(mess);
}

Message* Channel::getmessage(char* fbuff){ //Removes message from channel's queue (and returns also it's file buffer)
	Message *mes = queue->remove(fbuff);
	return mes;
}

long Channel::getid(){ return id; }

void Channel::getname(char *name_){
	strcpy(name_,name);
}

int Channel::getcount(){
	return queue->getcount();
}

List::List(){
	front = rear = NULL;
	count = 0;
}

List::~List(){
	Node *temp;
	while(front!=NULL){
		temp = front;
		front = front->next;
		delete temp->chan;
		delete temp;
	}
}

Node* List::search(long id){ //Returns a pointer to a channel
	if(front==NULL) return NULL;
	Node* temp = front;
	while(temp!=NULL){
		if(temp->chan->getid() == id){
			return temp;
		}
		temp = temp->next;
	}
	return NULL;
}

bool List::addchan(long id,char* name){ //Adds channel to the list
	if(search(id)!=NULL) {
		return false;
	}
	Node *add = new Node();
	Channel* chan = new Channel(id,name);
	add->chan = chan; 
	if(front == NULL){
		front = add;
	}
	else{
		rear->next = add;
		add->previous = rear;
	}
	rear = add;
	count++;
	return true;
}

bool List::addmessage(Message *mess,long id){ //Adds message to a specific channel
	Node* listchan = search(id);
	if(listchan == NULL) return false;
	listchan->chan->addmessage(mess);
	return true;
}

bool List::sendfile(Message *mess,long id){ //Sends a file to a specific channel
	Node* listchan = search(id);
	if(listchan == NULL) return false;
	listchan->chan->addmessage(mess);
	return true;
};

Channel* List::getchannel(int pos){
	if(pos>count) return NULL;
	Node* temp=front;
	for(int i=0 ; i<pos ; i++){
		temp = temp->next;
	}
	return temp->chan;
}

 Message* List::getmessage(long id,char* fbuff){ //Gets message from a specific channel
	Node* srch = search(id);
	if(srch == NULL) return NULL;
	Message *mes = srch->chan->getmessage(fbuff);
	return mes;
}

int List::getchcount(long id){ //Get number of messages of a specific channel
	Node *srch = search(id);
	if(srch == NULL ) return -1;
	return srch->chan->getcount();
}

int List::getcount(){ return count; }
