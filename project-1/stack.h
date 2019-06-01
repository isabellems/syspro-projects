
struct Snode{ //Stack's node
	unsigned long id;
	Snode *next;
	Snode *previous;
	Snode(){
		id = 0;
		next = NULL;
		previous = NULL;
	}
	
};

class Stack //Stack of nodes' ids
{
protected:
	Snode	*front;
	Snode	*rear;
	int count;
public:
	Stack();
	~Stack();
	void push(unsigned long);
	void pop();
	int getcount();
	unsigned long getfront();
	unsigned long getrear();
	bool isempty();
	void print(bool);
	//Prints stack from front to rear
};

struct SnodeD{ //StackD's node
	double amount;
	SnodeD *next;
	SnodeD *previous;
	SnodeD(){
		amount = 0;
		next = NULL;
		previous = NULL;
	}
};

class StackD //Stack of links' amounts
{
protected:
	SnodeD	*front;
	SnodeD	*rear;
	int count;
public:
	StackD();
	~StackD();
	void push(double);
	double pop();
	int getcount();
	double getfront();
	double getrear();
	bool isempty();
	double sum();
	//Sums the amounts of the stack and returns the value
};

