#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
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

bool fexists(const char *filename) { //checks if file exists
  ifstream ifile(filename);
  return ifile;
}

int main(int argc,char *argv[]){
	char msgbuf[SIZE+1],filebuf[SIZE+1],msgbuf1[SIZE+1],btos[100],stob[100],servfile[100];
	int fd1,fd2,nwrite,read1=1;
	DIR* dir = opendir(argv[1]); 
	sprintf(servfile,"%s/pidfile.txt",argv[1]);
	if(!dir){	//Checks if directory exists
		cout << "Directory doesn't exist." << endl;
		exit(1); //Exit, if it doesn't
	}
	if(!fexists(servfile)){ //Checks if file with server's pid exists
    	cout << "There is not such active board." << endl;
    	exit(1); //If not , exits
   	}
	sprintf(btos,"%s/btos",argv[1]);
	sprintf(stob,"%s/stob",argv[1]);
	signal(SIGINT,SIG_IGN); //Ignore ^C
    if((fd2=open(stob,O_RDONLY)) < 0){ //Opens server to boardpost pipe for reading
    	perror("fifo open problem2"); exit(3); 
    }
     if((fd1=open(btos, O_WRONLY)) < 0){ //Opens boardpost to server pipe for writing
        	perror("fifo open problem1"); exit(3);    
    }
    while(1){
    	cin.getline(msgbuf,100); //getline from user
    	if(!fexists(servfile)){ //In each loop check if server is still open
    		cout << "There is not such active board." << endl;
    		exit(1); //Exit if it isn't
    	}
    	if(!strcmp(msgbuf,"exit")){ //If users types exit , program terminates
    		close(fd1);
    		close(fd2);
			exit(1);
		
		}
		if(strcmp(msgbuf,"\0")){ //If line isn't empty
	    	if((nwrite = write(fd1, msgbuf , SIZE+1)) == -1){ //Send line to the server
				perror("Error in writing");
			    exit(2);
	    	}
	    	char *tok = strtok(msgbuf," ");
	    	if(!strcmp(tok,"list")){ //If first token is list
	    		cout << "Available channels: " << endl;	
	    		while(read>0){ //Loops until it receives "break" from the server
					read1=read(fd2, msgbuf1 , SIZE+1); 
					if(strcmp(msgbuf1,"break")){
						cout << msgbuf1 << endl; //Prints every channel
					}
					else{
						break;
					}
				}
			}
			else if(!strcmp(tok,"write")){//If token is write
				read1=read(fd2, msgbuf1 , SIZE+1); 
				if(read1>0)
					cout << msgbuf1 << endl; //Prints server's message
			}
			else if(!strcmp(tok,"send")){ //If token is "send"
				bool flag = false;
				for(int i=0 ; i<2 ; i++){ //Checks if number of arguments is correct
					tok = strtok(NULL," \0\n");
					if(tok==NULL){
						flag = true;
						break;
					}
				}
				if(flag==false){
					int pfile;
					pfile = open(tok,O_RDONLY); //Opens file
					if(pfile>0){ //If it exists		
						struct stat stat_buf;
						int rc = fstat(pfile, &stat_buf); //gets file's size
    					long size = (rc == 0 ? stat_buf.st_size : -1);
    					sprintf(msgbuf,"%ld",size);
    					if((nwrite = write(fd1, msgbuf , SIZE+1)) == -1){ //Sends file's size
							perror("Error in writing");
					   	 	exit(2);
			    		}
						char filebuf[SIZE+1];
						while (read(pfile,filebuf,SIZE+1) > 0){ //Reads file
			     			if((nwrite = write(fd1, filebuf , SIZE+1)) == -1){ //Sends file's content to the server
								perror("Error in writing");
					   		 	exit(2);
			    			}
			    		}
			    		strcpy(msgbuf,"break");
			    		if((nwrite = write(fd1, msgbuf , SIZE+1)) == -1){ //Sends a special word for server to stop waiting for file segments
							perror("Error in writing");
					   	 	exit(2);
			    		}
			    	}
			    	else{
			    		strcpy(filebuf,"ABORT/"); //Sends a special message to the server so that he stops waiting for the file
			    		if((nwrite = write(fd1, filebuf , SIZE+1)) == -1){
							perror("Error in writing");
					   	 	exit(2);
			    		}
			    	}
		    	}
		    	read1=read(fd2, msgbuf1 , SIZE+1); 
				if(read1>0) 
						cout << msgbuf1 << endl; //Prints server's message
			}
			else{
				read1=read(fd2, msgbuf1 , SIZE+1); 
				if(read1>0){
					cout << msgbuf1 << endl;
				}
			}
		}
    }
}
