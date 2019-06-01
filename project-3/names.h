struct Node{ //SList's node
	char name[50];
	Node *next;
	Node(){
		next = NULL;
	}
};

class SList{
protected:
	Node *front;
	Node *rear;
	int count;
public:
	SList();
	~SList();
	bool add(char*,bool);
	Node* search(char*); //searches for a specific name in the list
	bool get(char*);
	int get_count();
};

