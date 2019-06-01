#define SIZE 4096

struct Message{ 
	bool file; //if message is a file this is true
	char filename[100];
	char buffer[SIZE+1];
	char *fbuffer;
	long size;
	Message *previous;
	Message *next;
	Message(){
		file = false;
		previous = NULL;
		next = NULL;
	}
};

class Queue{ //Queue for messages in each channel
protected:
	Message* front;
	Message* rear;
	int count;
public:
	Queue();
	~Queue();
	void add(Message *);
	Message* remove(char*);
	bool isempty();
	int getcount();
};

class Channel{
protected:
	long id;
	char name[50];
	Queue *queue;
public:
	Channel(long,char[]);
	~Channel();
	void addmessage(Message *);
	void addtofile(char*);
	Message* getmessage(char*);
	long getid();
	void getname(char *);
	int getcount();
};

struct Node{
	Channel* chan;
	Node* previous;
	Node* next;
	Node(){
		chan = NULL;
		previous = NULL;
		next = NULL;
	}
};

class List{ //List of channels
protected:
	Node* front;
	Node* rear;
	int count;
public:
	List();
	~List();
	Node* search(long);
	bool addchan(long,char*);
	bool addmessage(Message *,long);
	bool sendfile(Message *,long);
	Channel* getchannel(int);
	int getchcount(long); //Return a particular channel's number of messages
	Message* getmessage(long,char*);
	int getcount();
};