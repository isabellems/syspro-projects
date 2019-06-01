#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include "graph.h"
#include "hash.h"

using namespace std;

Node::Node(unsigned long id_){ 
	id = id_;
	linksf = new Linklist(); //Creates list with links from the node
	linkst = new Linklist(); //Creates list with links to the node
}


Node::~Node(){
	delete linksf;
	delete linkst;	
}

unsigned long Node::getid(){ return id; }

Linklist* Node::getlinksf(){ return linksf; }

Linklist* Node::getlinkst(){ return linkst; }

void Node::addlinkf(unsigned long tail,double amount_){
	if(linksf->isempty()){ //If list is empty creates a new link and add it to it
		Link *link = new Link(id,tail,amount_);
		linksf->Add(*link);
	}
	else{ //If list is not empty
		Lnode* lpos = linksf->Search(id,tail); //Checks if link already exists
		if(lpos!=NULL){ 
			lpos->item->addamount(amount_); //If it does just adds the amount
		}
		else{
			Link *link = new Link(id,tail,amount_); //If it doesn't, creates a new link and adds it
			linksf->Add(*link);
		}
	}
}

void Node::addlinkt(unsigned long head,double amount_){
	if(linkst->isempty()){ //If list is empty creates a new link and adds it to it
		Link *link = new Link(head,id,amount_);
		linkst->Add(*link);
	}
	else{ //If list is not empty
		Lnode* lpos = linkst->Search(head,id); //Checks if link already exists
		if(lpos!=NULL){
			lpos->item->addamount(amount_); //If it does just adds the amount
		}
		else{
			Link *link = new Link(head,id,amount_); //If it doesn't creates a new link and adds it
			linkst->Add(*link);
		}
	}
}

void Node::removelinkf(unsigned long tail){
	Lnode *pos = linksf->Search(this->id,tail); //Searches for this link
	if(pos!=NULL){ //If it exists removes (and deletes) it
		linksf->Remove(pos);
		cout << "success : deleted transaction " << this->id << " " << tail << endl << endl;	
	}
	else
		cout << "failure : there is no link between " << this->id << " " << tail << endl << endl; 
}

void Node::removelinkt(unsigned long head){
	Lnode *pos = linkst->Search(head,this->id); //Searches for this link
	if(pos!=NULL){ //If it exists removes (and deletes) it
		linkst->Remove(pos);
	}
}

void Node::print(){
	cout << "vertex(" << id << ") = " ;
	linksf->print(); //Prints links heading from the node
}

double Node::sumout(){ return linksf->sum(); } //Returns amount of money that the node gives

double Node::sumin(){ return linkst->sum(); } //Returns amount of money that the node gets

Link::Link(unsigned long id1,unsigned long id2,double amount_){
	head = id1;
	tail = id2;
	amount = amount_;
}

unsigned long Link::gethead(){ return head; }

unsigned long Link::gettail(){ return tail; }

double Link::getamount(){ return amount; }

void Link::addamount(double amount_){ amount += amount_ ;}

void Link::print(){ 
	cout << "(" << tail << "," << amount << ")";
}

Graph::Graph(int hashsize){
	size = hashsize;
	hash = new Hashtable(size);
}

Graph::~Graph(){
	delete hash;
	cout << "success : cleaned memory " << endl << endl;
}

int Graph::getsize(){ return size; }

void Graph::createnode(unsigned long id){
	int length = 1;
	int temp = id;
	while( temp /= 10) //Checks if node's id is a 7-digit number
		length++;
	if(length!=7){ //Returns if it isn't
		cout << "Node's id must be a 7-digit number. " << endl << endl;
		return;
	}
	if(hash->Search(id,false)!=NULL){ //Searches for the node,returns if it exists
		cout << "failure : " << id << " already exists" << endl << endl;
		return;
	} 
	Node *node = new Node(id);//Creates it if it doesn't exist
	hash->addNode(node);//Adds it to the hashtable
	cout << "success : created " << id << endl << endl;
}

void Graph::delnode(unsigned long id){
	Node *node;
	if((node = hash->Search(id,false)) == NULL){ //Checks if the node exists
		cout << "failure : there is no such node as " << id << endl << endl; 
	 	return; //Returns , if it doesn't
	}
	if(!node->getlinksf()->isempty() || !node->getlinkst()->isempty()){ //Checks if node has links
		cout << "failure : cannot delete " << id << " due to links " << endl << endl; 
	 	return;//Returns if it has
	}
	node = hash->Remove(id);//Else: removes and deletes it
	delete node;
	cout << "success : deleted " << id << endl << endl;
}

bool Graph::addtran(unsigned long head,unsigned long tail,double amount){
	Node *headn;
	if((headn = hash->Search(head,false)) == NULL){ //Searches for head 
		cout << "failure : there is no such node as " << head << endl << endl;
		return false;
	} 
	Node *tailn;
	if((tailn = hash->Search(tail,false)) == NULL){ //Searches for tail
		cout << "failure : there is no such node as " << tail << endl << endl;
		return false;
	}
	headn->addlinkf(tail,amount); //If they do adds links (or amounts)
	tailn->addlinkt(head,amount);
	cout << "success : added transaction " << head << " " << tail << " with amount " << amount << endl << endl;
	return true;
}

bool Graph::deltran(unsigned long head,unsigned long tail){
	Node *headn;
	if((headn = hash->Search(head,false)) == NULL){//Searches for head
		cout << "failure : there is no such node as " << head << endl << endl;
		return false;
	} 
	Node *tailn;
	if((tailn = hash->Search(tail,false)) == NULL){//Searches for tail
		cout << "failure : there is no such node as " << tail << endl << endl; 
		return false;
	} 
	headn->removelinkf(tail); //Removes it from the two lists if it exists
	tailn->removelinkt(head);
	return true;
}

void Graph::lookup(int flag,unsigned long id){
	Node *node;
	double sum,sum1,sum2;
	if((node = hash->Search(id,false)) == NULL){ //Searches for the node
		cout << "failure : there is no such node as " << id << endl << endl;
		return ;
	}
	cout << "success : ";
	if(flag==1){ //Prints the amount of money that the node gets
		sum = node->sumin();
		cout << "in(" << id << ")=" << sum << endl << endl;
	}
	else if(flag==2){ //Prints the amount of money that the node gives
		sum = node->sumout();
		cout << "out(" << id << ")=" << sum << endl << endl;
	}
	else{
		 sum = 0; //Prints the total amount that the node gets
		 sum =  node->sumin()-node->sumout();
		 cout << "sum(" << id << ")=" << sum << endl << endl;
	}
}

void Graph::searchtr(Stack *st,unsigned long id,double amount,bool &flag){ //Flag=true if at least a triangle was found
	Node *node;
	if(st->getcount()==4){ //If stack has four members
		if(st->getfront()!=st->getrear()){ //and the first and the last are not equal, this isn't a triangle
			st->pop(); //removes the last member of the stack
			return ;
		}
		else{ //If they are equal, this is a triangle
			if(flag==false)
				cout << "success: triangle(" << st->getfront() << "," << amount << ")=" << endl; 
			st->pop(); //pops the last id
			st->print(true);  //and prints
			flag = true;
			return ;
		}
	}
	if((node = hash->Search(id,true)) == NULL){ //Searches for the node and marks it as visited
		st->pop(); //If it was already visited
		return ; //Pops it from the stack and returns
	}
	Linklist* linksf = node->getlinksf(); //Gets list of links heading from the node
	if(linksf->isempty()){ //If it is empty pops the stack and return
		st->pop();
		return ;
	}
	int count = linksf->getcount(); //Gets list's size
	Lnode *ln;
	unsigned long nid;
	for(int i=0 ; i<count ; i++){ //And recurses for all the members of the list
		ln = linksf->getlnode(i);
		nid = ln->item->gettail();
		if(ln->item->getamount()<amount) return; //Return if the amount is less than the amount given 
		st->push(nid);
		searchtr(st,nid,amount,flag);
	}
	hash->reset(); //Marks all nodes as not visited
	st->pop(); //Pops the stack
	return ;
}

void Graph::searchconn(Stack *st,unsigned long id1,unsigned long id2,bool &flag){
	Node *node;
	if(st->getrear()==id2){ //If last member of the stack is equal to the second node's id
		cout << "success : conn(" << st->getfront() << "," << id2 << ") = " ; //there is a connection
		st->print(true); //Prints the stack
		flag = true;
		return ;
	}
	if((node = hash->Search(id1,true)) == NULL){ //Searches for the node and marks it as visited
		st->pop(); //If it was already visited pops it from the stack and returns
		return ;
	}
	Linklist* linksf = node->getlinksf();
	if(linksf->isempty()){ //Gets list of links heading from the node
		st->pop();	//If it is empty pops the stack and return
		return ;
	}
	int count = linksf->getcount(); //Gets list's size
	Lnode *ln;
	unsigned long nid;
	for(int i=0 ; i<count ; i++){ //And recurses for all the members of the list
		ln = linksf->getlnode(i);
		nid = ln->item->gettail();
		st->push(nid);
		searchconn(st,nid,id2,flag);
		if(flag==true) return ;
	}
	st->pop(); //Pops the stack
	return ;
}

void Graph::searchcycl(Stack *st,unsigned long id,bool &flag){
	Node *node;
	if(st->getrear()==st->getfront() && st->getcount()>1){ //If the first and the last members of the stack are equal and stack's count is greater that 1
		if(st->getcount()<3){ //If stack's size is less than 3 , this is not a cycle
			 st->pop(); //Pops the stack
			 hash->unmark(st->getrear()); //Marks rear as not visited
			 return;
		}
		if(flag==false) //This is a cycle
			cout << "success: cycles " << st->getfront() << " = " << endl;
		st->pop(); //Pops stack and print
		st->print(true);
		flag = true;
		return ;
	}
	if((node = hash->Search(id,true)) == NULL){ //Checks if node is visited
		st->pop(); 
		return ;
	}
	Linklist* linksf = node->getlinksf(); //Gets links heading from the node
	if(linksf->isempty()){
		st->pop();
		return ;
	}
	int count = linksf->getcount();
	Lnode *ln;
	unsigned long nid;
	for(int i=0 ; i<count ; i++){ //Recurses for all the links
		ln = linksf->getlnode(i);
		nid = ln->item->gettail();
		st->push(nid);
		searchcycl(st,nid,flag);
		//st->pop();
	}
	st->pop(); //Pops the stack
	return ;
}

void Graph::dfs(Stack *st,StackD *std,unsigned long id,int l,bool &flag){
	Node *node;
	if((node = hash->Search(id,true)) == NULL){ //Checks if node is visited
		st->pop(); 
		std->pop();
		return ;
	}
	if((st->getcount())==l+1){ //If stack's size is depth+1
		if(flag==false) //Prints path, and total amount
			cout << "success: traceflow(" << st->getfront() << "," << l << ") = " << endl;
		st->print(false);
		st->pop(); //pops the two stacks
		cout << std->sum() << ")" << endl;
		std->pop();
		flag = true;
		return ;
	}
	Linklist* linksf = node->getlinksf(); //Gets links heading from the node
	if(linksf->isempty()){
		std->pop();
		return ;
	}
	int count = linksf->getcount();
	double amount;
	Lnode *ln;
	unsigned long nid;
	for(int i=0 ; i<count ; i++){ //Recurses for all links
		ln = linksf->getlnode(i);
		nid = ln->item->gettail();
		amount = ln->item->getamount();
		st->push(nid); //Pushes id to the stack
		std->push(amount);
		dfs(st,std,nid,l,flag);
	}
	//hash->reset(); 
	st->pop(); //Pops the two stacks
	std->pop();
}

void Graph::triangle(unsigned long id,double amount){
	Node *node;
	if((node = hash->Search(id,false)) == NULL){ //Checks if node exists
		cout << "failure : there is no such node as " << id << endl << endl; 
	 	return;
	}
	Stack *s = new Stack();
	s->push(id);  //Pushes it to the stack
	bool flag = false;
	searchtr(s,id,amount,flag); //Calls recursive function to search for triangles
	if(flag==false)
		cout << "No triangles were found for " << "(" << id << "," << amount << ")" << endl << endl;
	else
		cout << endl;
	hash->reset(); //Marks all nodes as not visited
	delete s;
}

void Graph::conn(unsigned long id1,unsigned long id2){
	Node *node;
	if((node = hash->Search(id1,false)) == NULL){ //Checks if node1 exists
		cout << "failure : there is no such node as " << id1 << endl << endl; 
	 	return;
	}
	if((node = hash->Search(id2,false)) == NULL){  //Checks if node2 exists
		cout << "failure : there is no such node as " << id2 << endl << endl; 
	 	return;
	}
	Stack *s = new Stack();
	s->push(id1); //Pushes node1 to the stack
	bool flag = false;
	searchconn(s,id1,id2,flag); //Calls recursive function to search for a connection
	if(flag==false)
		cout << "conn(" << id1 << "," << id2 << ") not found " << endl << endl;
	else
		cout << endl;
	hash->reset(); //Marks all nodes as not visited
	delete s;
}

void Graph::allcycles(unsigned long id){
	Node *node;
	if((node = hash->Search(id,false)) == NULL){ //Checks if node exists
		cout << "failure : there is no such node as " << id << endl << endl; 
	 	return;
	}
	Stack *s = new Stack();
	s->push(id); //Pushes it to the stack
	bool flag = false;
	searchcycl(s,id,flag); //Calls recursive function to search for cycles
	if(flag==false)
		cout << "no cycle found for " << id << endl << endl;
	else
		cout << endl;
	hash->reset(); //Marks all nodes as not visited
	delete s;
}

void Graph::traceflow(unsigned long id,int l){
	Node *node;
	if((node = hash->Search(id,false)) == NULL){ //Checks if node exists
		cout << "failure : there is no such node as " << id << endl << endl; 
	 	return;
	}
	Stack *s = new Stack(); //Creates stack for ids
	StackD *sd = new StackD(); //Creates stack for amounts
	s->push(id); //Pushes id to the stack
	bool flag = false;
	dfs(s,sd,id,l,flag); //Calls recursive function to search for the paths with depth l
	if(flag==false)
		cout << "traceflow cannot find a path from " << id << " with depth = " << l << endl; 
	cout << endl;
	hash->reset(); //Marks all nodes as not visited
	delete s;
	delete sd;
}

void Graph::print(){
	hash->print();
	cout << endl;
}

void Graph::dump(char* filename){
	Node *node;
	int count,lcount;
	Linklist *list;
	ofstream dumpfile(filename);
	unsigned long id;
	Link *link;
	int size = hash->getsize();
	for(int i=0; i<size ; i++){ //For all the buckets
		if(hash->getcount(i)!=0)
			dumpfile << "createnodes " ;
		count = hash->getcount(i); 
		for(int j=0 ; j<count ; j++){ //Prints each node to the file
			node = hash->getnode(i,j);
			if(node!=NULL){
				id = node->getid();
				dumpfile << id << " " ;
			}
		}
		dumpfile << endl;
	}
	for(int i=0; i<size ; i++){ //For all the buckets
		count = hash->getcount(i);
		for(int j=0; j<count ; j++){
			node = hash->getnode(i,j); //For all the nodes
			if(node!=NULL){
				list = node->getlinksf(); //Prints all the links to the file
				lcount = list->getcount();
				for(int k=0 ; k<lcount ; k++){
					link = list->getlnode(k)->item;
					dumpfile << "addtran " << node->getid() << " " << link->gettail() << " " << link->getamount() << endl;
				}
			}
		}
	}
	dumpfile.close();
	cout << "success : dumped to filename " << endl << endl;
}

