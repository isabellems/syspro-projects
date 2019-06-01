#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <string>
#include <cstdlib>
#include "graph.h"

using namespace std;

void parse(char *line,Graph *graph,int hash,bool &create){ //Gets a line and calls the proper graph function
	char *tok=NULL;
	char word[50];
	tok = strtok(line," ");
	strcpy(word,tok);
	if(!strcmp(word,"createnodes")){
		unsigned long id=0;
		tok = strtok(NULL," ");
		while(tok!=NULL){
			id = strtol(tok,NULL,10); //Converting id to long int number
			graph->createnode(id);
			tok = strtok(NULL," ");
		}
	}
	else if(!strcmp(word,"delnodes")){
		unsigned long id=0;
		tok = strtok(NULL," ");
		while(tok!=NULL){
			id = strtol(tok,NULL,10);
			graph->delnode(id);
			tok = strtok(NULL," ");
		}
	}
	else if(!strcmp(word,"addtran")){
		unsigned long id[2];
		double amount=0;
		for(int i=0 ; i<3 ; i++){
			tok = strtok(NULL," ");
			if(tok==NULL){
				cout << "addtran needs 3 arguments to function" << endl; 
				return;
			}
			if(i<2)
				id[i] = strtol(tok,NULL,10);
			if(i==2)
				amount = strtod(tok,NULL);
		}
		graph->addtran(id[0],id[1],amount);
	}
	else if(!strcmp(word,"deltran")){
		unsigned long id[2];
		for(int i=0 ; i<2 ; i++){
			tok = strtok(NULL," ");
			if(tok==NULL){
				cout << "deltran needs 2 arguments to function" << endl;
				return;
			}
			id[i] = strtol(tok,NULL,10);
		}
		graph->deltran(id[0],id[1]);
	}
	else if(!strcmp(word,"lookup")){
		unsigned long id=0;
		int flag=0;
		for(int i=0 ; i<2 ; i++){
			tok = strtok(NULL," ");
			if(tok==NULL){
				cout << "lookup needs 2 arguments to function" << endl;
				return;
			}
			if(i==0){
				if(!strcmp(tok,"in"))
					flag = 1;
				else if(!strcmp(tok,"out"))
					flag = 2;
				else if(!strcmp(tok,"sum"))
					flag = 3;
				else{
					cout <<  "Wrong input!"  << endl;
					return;
				}
			}
			else
				id = strtol(tok,NULL,10);
		}
		graph->lookup(flag,id);
	}
	else if(!strcmp(word,"triangle")){
		unsigned long id=0;
		double amount=0;
		for(int i=0 ; i<2 ; i++){
			tok = strtok(NULL," ");
			if(tok==NULL){
				cout << "triangle needs 2 arguments to function" << endl;
				return;
			}
			if(i==0){
				id = strtol(tok,NULL,10);
			}
			else
				amount = strtod(tok,NULL);
		}
		graph->triangle(id,amount);
	}
	else if(!strcmp(word,"conn")){
		unsigned long id[2];
		for(int i=0 ; i<2 ; i++){
			if(tok==NULL){
				cout << "conn needs 2 arguments to function" << endl;
				return;
			}
			tok = strtok(NULL," ");
			id[i] = strtol(tok,NULL,10);
		}
		graph->conn(id[0],id[1]);
	}
	else if(!strcmp(word,"allcycles")){
		unsigned long id=0;
		tok = strtok(NULL," ");
		if(tok==NULL){
			cout << "conn needs an argument to function" << endl;
			return;
		}
		id = strtol(tok,NULL,10);
		graph->allcycles(id);
	}
	else if(!strcmp(word,"traceflow")){
		unsigned long id=0;
		int depth;
		for(int i=0 ; i<2 ; i++){
			tok = strtok(NULL," ");
			if(tok==NULL){
				cout << "traceflow needs 2 arguments to function" << endl;
				return;
			}
			if(i==0)
				id = strtol(tok,NULL,10);
			else
				depth = strtol(tok,NULL,10);
		}
		graph->traceflow(id,depth);
	}
	else if(!strcmp(word,"bye")){
		delete graph;
		create = true;
	}
	else if(!strcmp(word,"print")){
		graph->print();
	}
	else if(!strcmp(word,"dump")){
		tok = strtok(NULL," ");
		graph->dump(tok);
	}
	else
		cout << "Wrong input!" << endl << endl; 
}


int main(int argc,char* argv[]){
	if(argc!=5 && argc!=3){
		cout << "Wrong number of arguments." << endl;
		return -1;
	}
	Graph *graph;
	char oper[30],line[1000]; //oper:input file, line:stores each line
	int hash=0;
	bool create = false;
	if(argc==5){
		if((strcmp(argv[1],"-o") && strcmp(argv[1],"-b")) && (strcmp(argv[4],"-o") && strcmp(argv[4],"-b"))){ 
			cout << "Not valid arguments." << endl;
			return -1;
		}
		if(!strcmp(argv[1],"-o")){
			strcpy(oper,argv[2]);
			hash = atoi(argv[4]);
		}
		else if(!strcmp(argv[3],"-o")){
			strcpy(oper,argv[4]);
			hash = atoi(argv[2]);
		}
		else{
			cout << "Not valid arguments. " << endl;
			return -1;
		}
		graph = new Graph(hash); //If there is a file
		ifstream operations(oper);
		while(operations.getline(line,1000)){ //Reads file first
			if(strlen(line)>1){
				parse(line,graph,hash,create);
				if(create==true){	
					graph = new Graph(hash);
					create = false;
				}
			}
		}
		while(cin.getline(line,1000)){ //Reads from stdin
			if(strlen(line)>1){
				parse(line,graph,hash,create);
					if(create==true){
						graph = new Graph(hash);
						create = false;
					}
			}
		}
		if(graph!=NULL)
			delete graph;
	}
	else{
		if(strcmp(argv[1],"-b")){ //If there is not an input file reads directly from stdin
			cout << "Not valid arguments." << endl;
			return -1;
		}
		int hash = atoi(argv[2]);
		graph = new Graph(hash);
		while(cin.getline(line,1000)){
			if(strlen(line)>1){
				parse(line,graph,hash,create);
				if(create==true){
					graph = new Graph(hash);
					create = false;
				}
			}
		}
		if(graph!=NULL) //Before exiting, deletes the graph if it still exists
			delete graph;
	}
}
