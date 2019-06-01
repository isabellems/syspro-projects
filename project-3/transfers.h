class Transfer{
protected:
	char name[50];
	long amount;
public:
	Transfer(long,char*);
	void get_name(char*);
	long get_amount();
	void add_amount(long);
	void print();
};

struct Lnode{ //Transfer List's node
	Transfer* item;
	Lnode* next;
	Lnode* previous;
	Lnode(){
		item = NULL;
		next = NULL;
		previous = NULL;
	}
};

class List{
private:
	Lnode* front;
	Lnode* rear;
	int count;
public:
	List();
	~List();
	Lnode* search(char*);
	void add_trans(char*,long);
	void print();
	bool is_empty();
};