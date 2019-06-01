struct Qnode{ //Queue's node
	int newsock; //socket's file descriptor
	Qnode* next;
	Qnode(int sock){
		newsock = sock;
		next = NULL;
	}
};


class Queue{
protected:
	Qnode* front;
	Qnode* rear;
	int count;
	int max_size; //maximum size
public:
	Queue(int);
	~Queue();
	bool enqueue(int);
	int dequeue();
	int get_count();
	int get_size(); //get maximum size
	bool is_full();
	bool is_empty();
};