#include <iostream>
#include <fstream>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "queue.h"


using namespace std;

int read_data(int fd,char *buffer);
int write_data(int fd,char* message);

int main(int argc,char* argv[]){
	long i , port , sock ;
	char buf[60],filename[60];
	unsigned int serverlen;
	struct sockaddr_in server;
	struct sockaddr *serverptr;
	struct hostent *rem;
	if(argc!=7){
		cout << "Wrong number of arguments!" << endl;
		exit(1);
	}
	if(!strcmp(argv[1],"-h")){
		if((rem = gethostbyname(argv[2])) == NULL ){
			perror("gethostbyname");
			exit(-1);
		}
		if(!strcmp(argv[3],"-p") && !strcmp(argv[5],"-i")){
			port = atoi(argv[4]);
			strcpy(filename,argv[6]);
		}
		else if(!strcmp(argv[3],"-i") && !strcmp(argv[5],"-p")){
			strcpy(filename,argv[4]);
			port = atoi(argv[6]);
		}
		else{
			cout << "Wrong arguments!" << endl;
			exit(1);
		}
	}
	else if(!strcmp(argv[1],"-p")){
		port = atoi(argv[2]);
		if(!strcmp(argv[3],"-h") && !strcmp(argv[5],"-i")){
			if((rem = gethostbyname(argv[4])) == NULL ){
				perror("gethostbyname");
				exit(-1);
			}
			strcpy(filename,argv[6]);
		}
		else if(!strcmp(argv[3],"-i") && !strcmp(argv[5],"-h")){
			strcpy(filename,argv[4]);
			if((rem = gethostbyname(argv[6])) == NULL ){
				perror("gethostbyname");
				exit(-1);
			}
		}
		else{
			cout << "Wrong arguments!" << endl;
			exit(1);
		}
	}
	else if(!strcmp(argv[1],"-i")){
		strcpy(filename,argv[2]);
		if(!strcmp(argv[3],"-h") && !strcmp(argv[5],"-p")){
			if((rem = gethostbyname(argv[4])) == NULL ){
				perror("gethostbyname");
				exit(-1);
			}
			port = atoi(argv[6]);
		}
		else if(!strcmp(argv[3],"-p") && !strcmp(argv[5],"-h")){
			port = atoi(argv[4]);
			if((rem = gethostbyname(argv[6])) == NULL ){
				perror("gethostbyname");
				exit(-1);
			}
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
	if((sock = socket(AF_INET,SOCK_STREAM,0))<0){
		perror("socket");
		exit(-1);
	}
	server.sin_family = AF_INET;
	bcopy((char*) rem->h_addr,(char*)&server.sin_addr,rem->h_length);
	server.sin_port = htons(port);
	serverptr = (struct sockaddr* )&server;
	serverlen = sizeof server;
	if(connect(sock,serverptr,serverlen)<0){
		perror("connect"); exit(-1);
	}
	cout << "connecting" << endl;
	char msgbuff[255],msgbuff1[255],com[255],*tok,*saveptr;
	long delay;
	ifstream infile(filename);
	while(infile.getline(msgbuff,300)){ //reading from file
		if(!strcmp(msgbuff,"\0"))
			continue;
		strcpy(com,msgbuff);
		tok = strtok_r(msgbuff," ",&saveptr);
		if(!strcmp(tok,"sleep")){
			tok = strtok_r(NULL,"\0",&saveptr);
			delay = strtol(tok,NULL,10);
			usleep(delay*1000);
		}
		else if(!strcmp(tok,"exit")){
			write_data(sock,com);
			sleep(50);
			exit(1);
		}
		else{
			write_data(sock,com);
			if(read_data(sock,msgbuff1)>0){
				cout << msgbuff1 << endl;
			}
		}
	}
	while(cin.getline(msgbuff,300)){ //reading from file
		if(!strcmp(msgbuff,"\0"))
			continue;
		strcpy(com,msgbuff);
		tok = strtok_r(msgbuff," ",&saveptr);
		if(!strcmp(tok,"sleep")){
			tok = strtok_r(NULL,"\0",&saveptr);
			delay = strtol(tok,NULL,10);
			usleep(delay*1000);
		}
		else if(!strcmp(tok,"exit")){
			exit(1);
		}
		else{
			write_data(sock,com);
			if(read_data(sock,msgbuff1)>0){
				cout << msgbuff1 << endl;
			}
		}
	}

	close(sock);
}

int read_data(int fd,char* buffer){ //read formated data
	char temp;
	int i = 0, length = 0 ,r= 0 ;
	if(read (fd, &length,sizeof(int)) < 0 ){ //read length first
		exit(-3);
	}
	while(i < length){
		if((r = read(fd,&buffer[i],length - i))<0){
			exit(-3);
		}
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
	if(write(fd,message,length) < 0){
		exit(-2);
	}
	return length;
}