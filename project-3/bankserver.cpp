#include <iostream>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
// #include <ctype.h>
#include <signal.h>
#include "hashtable.h"
#include "queue.h"
#include "names.h"

#define SIZE 100

using namespace std;

static pthread_mutex_t mutexes[SIZE]; //a mutex for each bucket
static pthread_mutex_t qmtx; //mutex for the queue
pthread_cond_t empty; //condition variable for empty queue
pthread_cond_t full; //condition variable for full queue

struct MT_Data{ //master thread struct
	Queue *queue_;
	int socket;
	struct sockaddr *clientptr;
	unsigned int clientlen;

};

struct WT_Data{ //worker thread struct
	Queue *queue_;
	Hashtable *hash;
};

int read_data(int fd,char* buffer); 
int write_data(int fd,char* message);
void *master_function(void* data); //master thread function
void *worker_function(void* data); //worker thread function
bool isnum(char* str); //checks if string is numeric

int main(int argc,char* argv[]){
	int n,psize,qsize,port,sock,newsock;
	int i,pid,fd1[2],fd2[2],option,status;
	char buf[60],buf2[60],buf3[60];
	unsigned int serverlen,clientlen;
	struct sockaddr_in server,client;
	struct sockaddr *serverptr,*clientptr;
	struct hostent *rem;
	if(argc!=7){
		cout << "Wrong number of arguments!" << endl;
		exit(1);
	} //parsing arguments
	if(!strcmp(argv[1],"-p")){ // port number
		port = atoi(argv[2]);
		if(!strcmp(argv[3],"-s") && !strcmp(argv[5],"-q")){ // pool size , queue size
			psize = atoi(argv[4]);
			qsize = atoi(argv[6]);
		}
		else if(!strcmp(argv[3],"-q") && !strcmp(argv[5],"-s")){
			qsize = atoi(argv[4]);
			psize = atoi(argv[6]);
		}
		else{
			cout << "Wrong arguments!" << endl;
			exit(1);
		}
	}
	else if(!strcmp(argv[1],"-s")){
		psize = atoi(argv[2]);
		if(!strcmp(argv[3],"-p") && !strcmp(argv[5],"-q")){
			port = atoi(argv[4]);
			qsize = atoi(argv[6]);
		}
		else if(!strcmp(argv[3],"-q") && !strcmp(argv[5],"-p")){
			qsize = atoi(argv[4]);
			port = atoi(argv[6]);
		}
		else{
			cout << "Wrong arguments!" << endl;
			exit(1);
		}
	}
	else if(!strcmp(argv[1],"-q")){
		qsize = atoi(argv[2]);
		if(!strcmp(argv[3],"-s") && !strcmp(argv[5],"-p")){
			psize = atoi(argv[4]);
			port = atoi(argv[6]);
		}
		else if(!strcmp(argv[3],"-p") && !strcmp(argv[5],"-s")){
			port = atoi(argv[4]);
			psize = atoi(argv[6]);
		}
		else{
			cout << "Wrong arguments!" << endl;
			exit(1);
		}
	}
	else{
		cout << "Wrong arguments!" << endl;
		exit(1);
	}
	if((sock = socket(AF_INET , SOCK_STREAM , 0)) == -1){
		perror("socket");
		exit(-1);
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	serverptr = (struct sockaddr* ) &server;
	serverlen = sizeof server;
	if(bind(sock , serverptr, serverlen) < 0){
		perror("bind") ; exit(-1);
	}
	cout << "binding" << endl;
	if(listen(sock,5) < 0){
		perror("listen") ; exit(-1);
	}
	cout << "listening" << endl;
	clientptr = (struct sockaddr* )&client;
	clientlen = sizeof clientptr;
	// if((rem = gethostbyaddr((char*)&client.sin_addr.s_addr,sizeof client.sin_addr,client.sin_family))==NULL){
	// 	perror("gethostbyaddr"); exit(-1);
	// }
	for(int i=0 ; i<SIZE ; i++){ //initializing mutexes for the hashtable's buckets
		pthread_mutex_init(&mutexes[i],NULL);
	}
	pthread_mutex_init(&qmtx,0); //mutex for the queue
	pthread_cond_init(&empty,0); //condition variable for empty queue
	pthread_cond_init(&full,0);//condition variable for full queue
	Hashtable *hash = new Hashtable(SIZE);
	Queue *queue_ = new Queue(qsize);
	pthread_t master_thread;
	pthread_t worker_thread[psize];
	MT_Data mt_d; //struct of arguments for the master thread function
	mt_d.queue_ = queue_;
	mt_d.socket = sock;
	mt_d.clientptr = clientptr;
	mt_d.clientlen = clientlen;
	if(pthread_create(&master_thread,NULL,master_function,&mt_d) < 0)
		exit(-3);
	WT_Data wt_d;	//struct of arguments for the worker thread function
	wt_d.queue_ = queue_;
	wt_d.hash = hash;
	for(int i=0 ; i < psize ; i++){
		if(pthread_create(&worker_thread[i],NULL,worker_function,&wt_d) < 0)
			exit(-3);
	}
	pthread_join(master_thread,0);
	for(int i=0; i<psize ; i++){
		pthread_join(worker_thread[i],0);
	}
	for(int i=0 ; i<SIZE ; i++)
		pthread_mutex_destroy(&mutexes[i]);
	pthread_cond_destroy(&empty);
	pthread_cond_destroy(&full);
	delete hash;
	delete queue_;
}

int read_data(int fd,char* buffer){ //read formated data
	char temp ;
	int i = 0, length = 0 ,r = 0 ;
	if(fd<0) return -1;
	if(read (fd, &length,sizeof(int)) <= 0 ) //read length first
		return -1;
	while(i < length){
		if((r = read(fd,&buffer[i],length - i))<0) //read the buffer
			exit(-3);
		i+=r;
	}
	return i;
}

int write_data(int fd,char* message){ //write formated data
	char temp ;
	int length = 0;
	length = strlen(message) + 1;
	if(write (fd,&length,sizeof(int)) <0 ){ //write length first
		exit(-2);
	}
	if(write(fd,message,length) < 0){ //write buffer
		exit(-2);
	}
	return length;
}

void *master_function(void* data){
	MT_Data *mt_d;
	mt_d = (MT_Data*) data;
	int newsock;
	char msgbuff[50];
	while((newsock = accept(mt_d->socket,mt_d->clientptr,&(mt_d->clientlen)))>0){ //accept connections
		pthread_mutex_lock(&qmtx);
		if(!mt_d->queue_->is_full()) //if queue is not full
			(mt_d->queue_)->enqueue(newsock); //add the connection to the queue
		else{
			while(mt_d->queue_->is_full()){ //if queue is full
				pthread_cond_wait(&full,&qmtx); //wait for the condition
				(mt_d->queue_)->enqueue(newsock); //add the connection to the queue
			}
		}
		pthread_mutex_unlock(&qmtx);
		pthread_cond_signal(&empty);//send signal to all the threads waiting for a non-empty queue
	}
}

void *worker_function(void* data){
	WT_Data *wt_d;
	wt_d =(WT_Data*) data;
	Hashtable *hash = wt_d->hash;
	char msgbuff[255],msgbuff1[255],buffer2[255],*tok,*tok2,*saveptr,*saveptr2,*temptok;
	int newsock,r;
	bool buckets[SIZE];
	while(1){
		pthread_mutex_lock(&qmtx); 
		if(!wt_d->queue_->is_empty()) //check if queue is empty
			newsock = wt_d->queue_->dequeue(); //if it's not pop a socket fd from the queue
		else{
			while(wt_d->queue_->is_empty()){ //if it is empty
				pthread_cond_wait(&empty,&qmtx); //wait for the condition
			}
			newsock = wt_d->queue_->dequeue(); //pop a socket fd from the queue
		}
		pthread_mutex_unlock(&qmtx);
		pthread_cond_signal(&full); //send signal to the thread waiting for a non-full queue
		while(1){
			r = read_data(newsock,msgbuff);
			if(r>0){ //if read was successfull
				bool flag=false;
				strcpy(buffer2,msgbuff);
				tok = strtok_r(msgbuff," ",&saveptr);
				if(!strcmp(tok,"add_account")){
					bool flag=false;
					long delay=0;
					long init_bal=0;
					tok = strtok_r(NULL," \0",&saveptr);
					if(tok==NULL){
						sprintf(msgbuff1,"Error. Account creation failed (unknown:unknown[:0])"); //insufficient arguments
						write_data(newsock,msgbuff1);
						continue;
					}
					if(!isnum(tok))
						flag = true;
					else
						init_bal = strtol(tok,NULL,10);
					tok = strtok_r(NULL," ",&saveptr);
					if(tok==NULL){
						sprintf(msgbuff1,"Error. Account creation failed (%ld:unknown[:0])",init_bal); //insufficient arguments
						write_data(newsock,msgbuff1);
						continue;
					}
					char name[100];
					strcpy(name,tok);
					tok = strtok_r(NULL,"\0",&saveptr);
					if(tok!=NULL){ //if there is an extra word
						temptok = strtok_r(NULL," \0",&saveptr);
						if(temptok==NULL && isnum(tok)) // and it is numeric
				    		delay = strtol(tok,NULL,10); //it's the delay time
					}
				    usleep(delay*1000); //user gives miliseconds while usleep accepts microseconds
				    int hash1 = hash->hash(name); //name's hash value
				    pthread_mutex_lock(&mutexes[hash1]);
				    if(flag == true){
				    	sprintf(msgbuff1,"Error. Account creation failed (%s:%ld[:%ld])",name,init_bal,delay);
				    }
					else if(hash->add_account(name,init_bal)){ //if account already exists add returns false
						sprintf(msgbuff1,"Success. Account creation (%s:%ld[:%ld])",name,init_bal,delay);
					}
					else
						sprintf(msgbuff1,"Error. Account creation failed (%s:%ld[:%ld])",name,init_bal,delay);
					pthread_mutex_unlock(&mutexes[hash1]);
				}
				else if(!strcmp(tok,"add_transfer")){
					bool flag = false;
					long delay=0;
					long amount=0;
					tok = strtok_r(NULL," ",&saveptr);
					if(tok==NULL){
						sprintf(msgbuff1,"Error. Transfer addition failed (unknown:unknown:unknown[:0])"); //insufficient arguments
						write_data(newsock,msgbuff1);
						continue;
					}
					if(!isnum(tok)) //if amount is not numeric
						flag = true;
					else
						amount = strtol(tok,NULL,10);
					tok = strtok_r(NULL," ",&saveptr);
					if(tok==NULL){
						sprintf(msgbuff1,"Error. Transfer addition failed (%ld:unknown:unknown[:0])",amount); //insufficient arguments
						write_data(newsock,msgbuff1);
						continue;
					}
					char src[100],dst[100];
					strcpy(src,tok);
					tok = strtok_r(NULL," ",&saveptr);
					if(tok==NULL){
						sprintf(msgbuff1,"Error. Transfer addition failed (%ld:%s:unknown[:0])",amount,src); //insufficient arguments
						write_data(newsock,msgbuff1);
						continue;
					}
					strcpy(dst,tok);
					tok = strtok_r(NULL," \0",&saveptr);
					int hash1,hash2;
					hash1 = hash->hash(src);
					hash2 = hash->hash(dst);
					pthread_mutex_lock(&mutexes[hash1]); //locks source
					if(hash1!=hash2) //if the two accounts are not in the same bucket
						pthread_mutex_lock(&mutexes[hash2]); //locks destination
					if(tok!=NULL){
						temptok = strtok_r(NULL," \0",&saveptr);
						if(temptok==NULL && isnum(tok))
							delay = strtol(tok,NULL,10);
					}
				    usleep(delay*1000);
					if(hash->add_transfer(dst,src,amount) && flag==false)
						sprintf(msgbuff1,"Success. Transfer addition(%s:%s:%ld:%ld)",src,dst,amount,delay);
					else
						sprintf(msgbuff1,"Error. Transfer addition failed(%s:%s:%ld:%ld)",src,dst,amount,delay);
					pthread_mutex_unlock(&mutexes[hash1]);
					if(hash1!=hash2)
						pthread_mutex_unlock(&mutexes[hash2]);
				}
				else if(!strcmp(tok,"add_multi_transfer")){
					long delay=0,amount=0;
					bool flag = false;
					int accounts = 0 ,hash1 = 0 ,hash2 =0;
					tok = strtok_r(NULL," ",&saveptr);
					if(tok==NULL){
						sprintf(msgbuff1,"Error. Multi_transfer addition failed (unknown:unknown[:0])"); //insufficient arguments
						write_data(newsock,msgbuff1);
						continue;
					}
					if(!isnum(tok))
						flag = true;
					else
						amount = strtol(tok,NULL,10);
					tok = strtok_r(NULL," ",&saveptr);
					if(tok==NULL){
						sprintf(msgbuff1,"Error. Multi_transfer addition failed (%ld:unknown[:0])",amount); //insufficient arguments
						write_data(newsock,msgbuff1);
						continue;
					}
					char src[100],temp[100];
					SList *names = new SList(); //creates a list of names to avoid duplicates
					strcpy(src,tok);
					if(hash->search(src)==NULL){ //is source doesn't exist
						flag = true;
					}
					else{
						hash1 = hash->hash(src);
						names->add(src,true); //add name to the list
						buckets[hash1] = true; //mark the bucket that needs to be locked
					}
					tok2 = strtok_r(buffer2," ",&saveptr2);
					for(int i=0 ; i<3 ; i++)
						tok2 = strtok_r(NULL," ",&saveptr2);
					while(tok!=NULL){
						tok = strtok_r(NULL," \0",&saveptr);
						tok2 = strtok_r(NULL," \0",&saveptr2);
						if(tok!=NULL && tok2==NULL && isnum(tok)){ //check if delay is needed
							delay = strtol(tok,NULL,10);
						}
						else if(tok!=NULL){
							accounts++;
							if(hash->search(tok)==NULL){ //if destination doesn't exist in the hashtable
								flag = true;
							}
							else if(names->add(tok,true)==false){ //if name already in the list
								flag = true;
							}
							else{
								hash2 = hash->hash(tok);
								buckets[hash2] = true; //mark the bucket that needs to be locked
							}
						}
					}
					if(names->get_count()==1){
						sprintf(msgbuff1,"Error. Multi_transfer addition failed (%ld:%s[:0])",amount,src); //insufficient arguments
						delete names;
						write_data(newsock,msgbuff1);
						continue;
					}
					for(int i=0 ; i<SIZE ; i++){
						if(buckets[i]==true){
							pthread_mutex_lock(&mutexes[i]); //lock all the marked buckets
						}
					}
			    	usleep(delay*1000);
					if(flag == true){
						sprintf(msgbuff1,"Error. Multi_transfer addition failed.(%s:%ld[:%ld])",src,amount,delay); //insufficient arguments
					} 
					else{
						long tot_am = amount*accounts; //total amount needed for all transfer
						if(hash->balance_suff(src,tot_am)){ //check if source's balance is sufficient
							names->get(src); //first name is the source
							char acc[50];
							while(names->get(acc)){ //get names from the queue
								hash->add_transfer(acc,src,amount); //and then transfer
							}
							sprintf(msgbuff1,"Multi_Transfer addition (%s:%ld[:%ld])",src,amount,delay);
						}
						else{
							sprintf(msgbuff1,"Error. Multi_transfer addition failed.(%s:%ld[:%ld])",src,amount,delay);
						}
					}
					delete names;
					for(int i=0 ; i<SIZE ; i++){
						if(buckets[i]==true)
							pthread_mutex_unlock(&mutexes[i]); //unlock all locked buckets
						buckets[i] = false; //reset the array
					}
				}
				else if(!strcmp(tok,"print_balance")){
					int hash1;
					long delay=0;
					tok = strtok_r(NULL," \0",&saveptr);
					if(tok==NULL){
						sprintf(msgbuff1,"Error. Balance (unknown:unknown[:0])"); //insufficient arguments
						write_data(newsock,msgbuff1);
						continue;
					}
					char name[50];
					strcpy(name,tok);
					tok = strtok_r(NULL," \0",&saveptr);
					if(tok!=NULL){
						temptok = strtok_r(NULL," \0",&saveptr);
						if(temptok==NULL && isnum(tok))
							delay = strtol(tok,NULL,10);
					}
				    usleep(delay*1000);
					long bal;
					hash1 = hash->hash(name);
					pthread_mutex_lock(&mutexes[hash1]);
					if(hash->print_balance(name,bal))
						sprintf(msgbuff1,"Success. Balance (%s:%ld:[%ld])",name,bal,delay);
					else
						sprintf(msgbuff1,"Error. Balance (%s:[%ld])",name,delay);
					pthread_mutex_unlock(&mutexes[hash1]);
				}
				else if(!strcmp(tok,"print_multi_balance")){
					sprintf(msgbuff1,"Success. Multi-Balance(");
					long delay = 0 , bal = 0;
					bool flag;
					int hash1;
					char buff[30];
					SList *names = new SList(); //create names' list
					SList *names_err = new SList(); //create names' list for the output 
					tok2 = strtok_r(buffer2," ",&saveptr2);
					tok2 = strtok_r(NULL," ",&saveptr2);
					do{
						tok = strtok_r(NULL," ",&saveptr);
						tok2 = strtok_r(NULL," ",&saveptr2);
						if(tok!=NULL){
							if(tok!=NULL && tok2==NULL && isnum(tok)){
								delay = strtol(tok,NULL,10);
			    				usleep(delay*1000);
							}
							else{
								hash1 = hash->hash(tok);
								buckets[hash1] = true; //mark buckets that will be locked
								if(names->add(tok,true)==false) flag = false; //add names to a list without duplicates
								names_err->add(tok,false); //add names to a list with duplicates (for the output needed when the command fails)
							}
						}
					}
					while(tok!=NULL);
					if(names->get_count()==0){
						sprintf(msgbuff1,"Error. Multi-Balance (unknown:unknown[:0])"); //insufficient arguments
						delete names;
						delete names_err;
						write_data(newsock,msgbuff1);
						continue;
					}
					for(int i=0 ; i<SIZE ; i++){
						if(buckets[i]==true)
							pthread_mutex_lock(&mutexes[i]); //lock marked buckets
					}
					char acc[50],acc1[50];
					while(names->get(acc)){
						if(hash->print_balance(acc,bal)){
							strcat(msgbuff1,acc);
							sprintf(buff,"/%ld:",bal);
							strcat(msgbuff1,buff);
							flag = true;
						}
						else
							flag = false;
						if(flag==false){
							sprintf(msgbuff1,"Error. Multi-Balance(");
							while(names_err->get(acc1)){
								strcat(msgbuff1,acc1);
								strcat(msgbuff1,":");
							}
							break;
						}
					}
					delete names;
					delete names_err;
					for(int i = 0 ; i<SIZE ; i++){
						if(buckets[i]==true)
							pthread_mutex_unlock(&mutexes[i]);
						buckets[i] = false;
					}
					char del[10];
					sprintf(del,"[%ld]",delay);
					strcat(msgbuff1,del);
					strcat(msgbuff1,")");
				}
				else{
					sprintf(msgbuff1,"Error. Unknown command");
				}
				write_data(newsock,msgbuff1);
			}
			else{
				close(newsock);
				break;
			}
		}
	}
}

bool isnum(char* str){ //checks is a string is numeric
	int len = strlen(str);
	for(int i=0; i<len ; i++){
		if(!isdigit(str[i])){
			return false;
		}
	}
	return true;
}

