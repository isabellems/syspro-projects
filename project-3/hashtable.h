#include "transfers.h"

class Account{ 
protected:
	char name[50];
	long balance;
	List* transfers; //list of transfers to this account
public:
	Account(long,char*); 
	~Account();
	bool add_transfer(char*,long);
	long get_balance();
	void get_name(char*);
	List* getlist();
	void print_balance();
	void red_balance(long); //reduces balance
};

struct Bnode{ //Bucket's node
	Account* item;
	Bnode* previous;
	Bnode* next;
	Bnode(){
		item=NULL;
		previous=NULL;
		next=NULL;
	}
};

class Bucket{
protected:
	Bnode* front;
	Bnode* rear;
	int count;
public:
	Bucket();
	~Bucket();
	void add_account(Account*);
	Bnode* search(char*); //searches for an account in the bucket
	void print_balance(char*,long*); //returns account's balance
	bool add_transfer(char*,char*,long); 
	bool is_empty();
};

class Hashtable{
protected:
	int size;
	Bucket** array;
	int count;
public:
	Hashtable(int);
	~Hashtable();
	int hash(char*); //returns hash value
	bool add_account(char*,long);
	Account* search(char*); //searches for an account in the hashtable
	bool print_balance(char*,long&); //returns account's balance
	int get_size();
	bool add_transfer(char*,char*,long); 
	bool balance_suff(char*,long); //checks if an account's balance is sufficient for a specific transfer
	bool is_empty();
};
