#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include "channel.h"

using namespace std;

bool fexists(const char *filename) { //Checks if a particular file exists
  ifstream ifile(filename);
  return ifile;
}

void filename(char *path,char *name){ //Keeps only the name of the file from a path
	char *tok,*prev;
	tok = strtok(path,"/\n\0");
	while(1){
		prev = tok;
		tok = strtok(NULL,"/\n\0");
		if(tok==NULL){
			strcpy(name,prev);
			break;
		}
	}
}

void namefile(char* path,char *name,char *form){ //Names the file
	int i=0;
	char filename[100],temp[100],*tok;
	sprintf(filename,"%s/%s",path,name);
	tok = strtok(name,".");
	strcpy(name,tok);
	tok = strtok(NULL,"\0");
	strcpy(form,tok);
	strcpy(temp,name);
	while(fexists(filename)){ //While a file with this name exists , it changes it
		i++; 
		sprintf(temp,"%s%d",name,i);
		sprintf(filename,"%s/%s.%s",path,temp,form);
	}
	sprintf(name,"%s.%s",temp,form);
}

void deletefiles(char *path){ //Deletes pipes and pid file
	char buffer[150];
	sprintf(buffer,"%s/pidfile.txt",path);
	remove(buffer);
	sprintf(buffer,"%s/ctos",path);
	remove(buffer);
	sprintf(buffer,"%s/stoc",path);
	remove(buffer);
	sprintf(buffer,"%s/stob",path);
	remove(buffer);
	sprintf(buffer,"%s/btos",path);
	remove(buffer);
}

int main(int argc,char *argv[]){
	char path[100];
	char fname[150];
	pid_t pid,pidserv;
	ifstream pidfile;
	int nwrite;
	signal(SIGINT,SIG_IGN); //Ignore ^C
	if(argv[1]==NULL){
		cout << "No path was given." << endl;
		exit(1);
	}
	strcpy(path,argv[1]);
	DIR* dir = opendir(path); //Open directory
	if(!dir){
		mkdir(path,0744); //Creates one if it doesn't exist
	}
	sprintf(fname,"%s/pidfile.txt",path);
	if(!fexists(fname)){ //if pidfile doesn't exist
		pid = fork(); //fork
		if(pid == -1){
			perror("Failed to fork");
			exit(1);
		}
	}
	else{
		pidfile.open(fname); //Keeps server's process id if it does exist
		pidfile >> pidserv;
		pid = pidserv;
	}
	if(pid == 0){ //Server's Code
		signal(SIGINT,SIG_IGN); //Ignore ^C
		char ctos[100],stoc[100],stob[100],btos[100],msgbuf[SIZE+1],msgbuf1[SIZE+1],msgbuf2[SIZE+1],msgbuf3[SIZE+1];
		int fd1,fd2,fd3,fd4;
		ofstream pfile(fname); //Creates pid file and writes process id
		pfile << getpid();
		pfile.close();
		sprintf(ctos,"%s/ctos",path);
		if(mkfifo(ctos,0666) == -1){ //Creates client to server pipe
			if( errno!=EEXIST){
				perror("receiver : mkfifo");
				exit(6);
			}
		}
		sprintf(stoc,"%s/stoc",path);
		if(mkfifo(stoc,0666) == -1){ //Creates server to client pipe
			if( errno!=EEXIST){
				perror("receiver : mkfifo");
				exit(6);
			}
		}
		sprintf(stob,"%s/stob",path);
		if(mkfifo(stob,0666) == -1){ //Creates server to boardpost pipe
			if( errno!=EEXIST){
				perror("receiver : mkfifo");
				exit(6);
			}
		}
		sprintf(btos,"%s/btos",path);
		if(mkfifo(btos,0666) == -1){ //Creates boardpost to server pipe
			if( errno!=EEXIST){
				perror("receiver : mkfifo");
				exit(6);
			}
		}
		List *channels = new List(); //Creates list of channels
		if ((fd1=open(ctos, O_RDONLY|O_NONBLOCK )) < 0) { //Opens all the pipes
        	perror("fifo open problem"); exit(3);    
    	}
    	if((fd2=open(stoc,O_WRONLY)) < 0){
    		perror("fifo open problem"); exit(3); 
    	}
    	if((fd3=open(stob,O_RDWR)) < 0){ //Opens server to boardpost in RDWR mode for pipe to open even if boardpost isn't running at the moment
    		perror("fifo open problem"); exit(3); 
    	}
    	if((fd4=open(btos,O_RDONLY|O_NONBLOCK)) < 0){
    		perror("fifo open problem fd4"); exit(3); 
    	}
    	cout << "Fifos were created" << endl;
		while(1){ 
			int read1=read(fd1, msgbuf, SIZE+1); //Read from client
        	if (read1 > 0) {
            	char *tok;
            	tok = strtok(msgbuf," \0\n");
            	if(!strcmp(tok,"createchannel")){
            		char name[50];
            		long id;
            		for(int i=0; i<2 ; i++){
            			if((tok = strtok(NULL," ")) == NULL ){
            				strcpy(msgbuf1,"Not enough arguments to function createchannel.");
            				break;
            			}
            			if(i==0) id = strtol(tok,NULL,10);
            			else {
            				strcpy(name,tok); //Creates a new channel if arguments are ok
            				if(channels->addchan(id,name)) //If addchan returned true ,channel was created
            					strcpy(msgbuf1,"Channel created.");
            				else //If not ,a channel with the same id already exists
            					sprintf(msgbuf1,"Channel with id %ld already exists.",id);
            			}
            		}
            		if((nwrite = write(fd2, msgbuf1 , SIZE+1)) == -1){ //Write back to the client
						perror("Error in writing");
						exit(2);
					}	
            	}
            	else if(!strcmp(tok,"getmessages")){ 
            		if((tok = strtok(NULL," ")) == NULL){ //Check number of arguments
            			strcpy(msgbuf1,"Not enough arguments to function getmessages.\n");
            			if((nwrite = write(fd2, msgbuf1 , SIZE+1)) == -1){
							perror("Error in writing");
							exit(2);
						}
					}
            		else{
            			long id = strtol(tok,NULL,10);
            			Message *mes;
            			char *fb = NULL;
            			int count = channels->getchcount(id); //Gets number of messages in this channel
            			if(count!=-1){
            				for(int i=0 ; i<count ; i++){  //Loops for every message
            					mes = channels->getmessage(id,fb);
            					if(mes->file==false){ //If message isn't a file
            						strcpy(msgbuf1,mes->buffer); //Send it to the client
            						if((nwrite = write(fd2, msgbuf1 , SIZE+1)) == -1){
										perror("Error in writing");
										exit(2);
									}
            					}
            					else{ //If it is a file
            						sprintf(msgbuf1,"FILE %s %ld",mes->filename,mes->size); //Sends name to the client
            						if((nwrite = write(fd2, msgbuf1 , SIZE+1)) == -1){
										perror("Error in writing");
										exit(2);
									}
									nwrite = 0;
									int written = 0;
									do{
										nwrite = write(fd2, mes->fbuffer+written, SIZE+1); //Write each file's content's segment to the client
										written+=nwrite;
									}
									while(written<mes->size); //Until they are all written
									strcpy(msgbuf1,"break1");
									if((nwrite = write(fd2, msgbuf1 , SIZE+1)) == -1){
										perror("Error in writing");
										exit(2);
									}
            					}
            					if(mes->file) {
            						free(fb); //File's content's buffer, was malloced
						}
            				}
            			}
            			else{
            				strcpy(msgbuf1,"Could not load any messages (There is no such channel)."); //If channel doesn't exist
            				if((nwrite = write(fd2, msgbuf1 , SIZE+1)) == -1){ //Sends this message to the client
								perror("Error in writing");
								exit(2);
							}
            			}	
            		}
            		strcpy(msgbuf1,"break"); //Sends "break" to the client so that he stops waiting for messages
            		if((nwrite = write(fd2, msgbuf1 , SIZE+1)) == -1){
						perror("Error in writing");
						exit(2);
					}
            	}
            	else if(!strcmp(tok,"shutdown")){
            		tok = strtok(NULL," ");
            		delete channels; //Destroys list of channels
            		deletefiles(path); //Delete pipes and pid file
            		close(fd1);
            		close(fd2);
            		close(fd3);
            		close(fd4); //Close 4 pipes
            		exit(1);
            	}
            	else{ //If command does not exist
            		strcpy(msgbuf1,"There is no such command.");
            		if((nwrite = write(fd2, msgbuf1 , SIZE+1)) == -1){
						perror("Error in writing");
						exit(2);
					}
            	}
        	}
        	read1=read(fd4, msgbuf2 , SIZE+1); //Reads from boardpost
        	if (read1 > 0) { //If bytes were read
        		char* tok = strtok(msgbuf2," ");
        		if(!strcmp(tok,"list")){
        			if((tok=strtok(NULL," "))!=NULL){ //Check arguments
        				strcpy(msgbuf3,"Wrong number of arguments.");
        				if((nwrite = write(fd3, msgbuf3 , SIZE+1)) == -1){
							perror("Error in writing");
							exit(2);
						}	
						continue;
        			} 
        			int count = channels->getcount(); //Get number of channels in the list
        			if(count==0){
        				strcpy(msgbuf3,"None");
        				if((nwrite = write(fd3, msgbuf3 , SIZE+1)) == -1){
							perror("Error in writing");
							exit(2);
						}	
        			}
        			for(int i=0 ; i<count ; i++){
        				Channel *chan = channels->getchannel(i); //Get one channel at a time 
        				char name[50];
        				chan->getname(name);
        				sprintf(msgbuf3,"%s %ld",name,chan->getid());
        				if((nwrite = write(fd3, msgbuf3 , SIZE+1)) == -1){ //Write each channel to boardpost
							perror("Error in writing");
							exit(2);
						}	
        			}
        			strcpy(msgbuf3,"break"); //Send "break" to boardpost so that he stops looping
        			if((nwrite = write(fd3, msgbuf3 , SIZE+1)) == -1){
						perror("Error in writing");
						exit(2);
					}	
        		}
        		else if(!strcmp(tok,"write")){
        			long id;
        			bool flag = false;
        			for(int i=0 ; i<2 ; i++){
        				if(i==0){
        					tok = strtok(NULL," ");
        					if(tok==NULL){ //Checks number of arguments
        						strcpy(msgbuf3,"Wrong number of arguments.");
        						if((nwrite = write(fd3, msgbuf3 , SIZE+1)) == -1){
									perror("Error in writing");
									exit(2);
								}	
								flag = true;
								break;
        					}
        						id = strtol(tok,NULL,10);
        				}
        				else{
        					tok = strtok(NULL,"\0\n");
        					if(tok==NULL){
        						strcpy(msgbuf3,"Wrong number of arguments.");
        						if((nwrite = write(fd3, msgbuf3 , SIZE+1)) == -1){
									perror("Error in writing");
									exit(2);
								}	
								flag = true;
								break;
        					}
        					Message *mess = new Message();
        					mess->file = false;
        					strcpy(mess->buffer,tok);
        					if(channels->addmessage(mess,id)){ //Add message to the channel if it exists
        						strcpy(msgbuf3,"Message was successfully written.");
        					}
        					else{
        						strcpy(msgbuf3,"Could not write message (There is no such channel).");
        					}
        				}
        			}
        			if(flag == false){ //Write back to the boardpost
	        			if((nwrite = write(fd3, msgbuf3 , SIZE+1)) == -1){
							perror("Error in writing hereee");
							exit(2);
						}
					}
        		}
        		else if(!strcmp(tok,"send")){
        			bool flag = false;
        			long id;
        			char name[100],filebuf[SIZE+1];
        			for(int i=0 ; i<2 ; i++){
        				if(i==0){
        					tok = strtok(NULL," ");
    						if(tok==NULL){
	    						strcpy(msgbuf3,"Wrong number of arguments.");
	    						if((nwrite = write(fd3, msgbuf3 , SIZE+1)) == -1){
									perror("Error in writing");
									exit(2);
								}
								flag = true;
								break;
    						}
    						id = strtol(tok,NULL,10);
        				}
        				else if(i==1){
        					tok = strtok(NULL," ");
        					   	if(tok==NULL){
	    							strcpy(msgbuf3,"Wrong number of arguments.");
	    							if((nwrite = write(fd3, msgbuf3 , SIZE+1)) == -1){
										perror("Error in writing");
										exit(2);
									}	
									flag = true;
									break;
    							}
        					filename(tok,name); //Gets file's name from the path
        				}
        			}
        			if(flag == false){
        				long size;
        				char *filebuf;
        				read1=read(fd4, msgbuf2 , SIZE+1);
        				size = strtol(msgbuf2,NULL,10);
        				Message *mess = new Message(); //Creates new message
        				mess->size = size; //Assigns size
        				mess->file = true; //Type
        				strcpy(mess->filename,name); //Assigns name
        				if(size>0){
        					mess->fbuffer = (char*)malloc(sizeof(char)*size); //Mallocs file buffer
						if(mess->buffer == NULL){
							strcpy(msgbuf3,"Cannot allocate more space.");
							if((nwrite = write(fd3, msgbuf3 , SIZE+1)) == -1){
                                                                                perror("Error in writing");
                                                                                exit(2);
                                                                        }
						continue;
						}
					}
        				for(int i=0; ;i++){ //Gets file in segments and keeps them to a malloced buffer
		         			read1=read(fd4, msgbuf2 , SIZE+1); //Reads file's content from bordpost
							if(!strcmp(msgbuf2,"break")){ //Break is the special word sent when EOF
								if(channels->sendfile(mess,id)){
				      				strcpy(msgbuf3,"File was successfully sent.");
				      				if((nwrite = write(fd3, msgbuf3 , SIZE+1)) == -1){
										perror("Error in writing");
										exit(2);
									}
								}
				         		else{
			        				strcpy(msgbuf3,"Could not send file (There is no such channel).");
			        				if((nwrite = write(fd3, msgbuf3 , SIZE+1)) == -1){
										perror("Error in writing");
										exit(2);
									}
								}
								break;
							}
		        			if(strcmp(msgbuf2,"ABORT/")){ 
			        			if(read1>0){
			        				memcpy(mess->fbuffer+i*(SIZE+1),msgbuf2,SIZE+1); //Writes file segments in the file buffer
								}
							}
							else{ //If boardpost send "abort" it means that the file doesn't exist
								strcpy(msgbuf3,"Could not send file (File doesn't exist).");
								if((nwrite = write(fd3, msgbuf3 , SIZE+1)) == -1){
									perror("Error in writing hereee");
									exit(2);
								}
								break;
							}
						}
					}
        		}
        		else{
        			strcpy(msgbuf3,"There is no such command.");
					if((nwrite = write(fd3, msgbuf3 , SIZE+1)) == -1){
						perror("Error in writing");
						exit(2);
					}	
        		}
        	}
        	usleep(100000);
		}
	}
	else{ //Client's code
		signal(SIGINT,SIG_IGN); //Ignore ^C
		char line[100],msgbuf[SIZE+1],ctos[100],stoc[100],msgbuf1[SIZE+1];
		int fd1,fd2,nwrite,read1;
		sprintf(ctos,"%s/ctos",path);
		sprintf(stoc,"%s/stoc",path);
    	while((fd1=open(ctos, O_WRONLY)) < 0); //Open client to server for writing
		while((fd2=open(stoc, O_RDONLY)) < 0); //Open server to client for reading
		while(1){
			cin.getline(msgbuf,100);
			if(strcmp(msgbuf,"\0")&&strcmp(msgbuf,"^C")){
				if(!strcmp(msgbuf,"exit")){
					close(fd1);
					close(fd2);
					exit(1); //Close pipes and exit
				}
				else if(!strcmp(msgbuf,"shutdown")){
					sprintf(msgbuf,"shutdown");
					if((nwrite = write(fd1, msgbuf , SIZE+1)) == -1){ //Writes shutdown to server
						perror("Error in writing");
						exit(2);
					}
					close(fd1); //closes pipes
					close(fd2);
					exit(1); //exits
				}
				else{ // Just writes line to the server
					if((nwrite = write(fd1, msgbuf , SIZE+1)) == -1){
						perror("Error in writing");
						exit(2);
					}
				}
				char *tok = strtok(msgbuf," ");
		    	if(strcmp(tok,"getmessages")){ //If command is not "getmessages"
					read1 = read(fd2, msgbuf1, SIZE+1); 
					if(read1 > 0){
						cout << msgbuf1 << endl; //Just prints what the server sent
					}
				}
				else{ //If command is getmessages
					cout << "Channel's available messages:" << endl;
					while(1){ //Loops until reads "break"
						read1 = read(fd2, msgbuf1, SIZE+1);	//Read from server
						if(read1 > 0){
							if(strcmp(msgbuf1,"break")){
								char tempbuf[100],form[10];
								strcpy(tempbuf,msgbuf1);
								char* tok = strtok(tempbuf," ");
								if(strcmp(tok,"FILE")) //Prints message if it is not a file
									cout << msgbuf1 << endl;
					 			else{ //If it is a file
						 			char name[100],*tok,name1[100];
						 			tok = strtok(NULL," ");
						 			strcpy(name,tok);
						 			namefile(argv[1],name,form); //Checks if file with the same name exists and changes name if it does
						 			sprintf(name1,"%s/%s",argv[1],name);
						 			ofstream sfile;
							 		FILE *fp = fopen(name1,"wb");
									for(int i=0 ; ; i++){
						 			 read(fd2, msgbuf1, SIZE+1); //Reads file's content
						 			 if(!strcmp(msgbuf1,"break1")) break;
						 			 		fwrite(msgbuf1,1,SIZE+1,fp);
									}
								}
							}
							else   
								break; 
						}	
					}
				}
			}
		}
	}
}

