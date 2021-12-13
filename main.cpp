#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <array>
#include <algorithm>

// #include "Block.h"
// #include "Record.h"

#define BLOCK_SIZE 100

using namespace std;

vector<string> split (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}


int MAX = 3; //size of each node

class Record {
    string tconst;
    double average;
    int numVotes;
    int blockId;
    public:
        Record() {}
        Record(string tc, double avg, int votes) {
            tconst = tc;
            average = avg;
            numVotes = votes;
        }

        void say() {
            cout << tconst << " " << average << " " << numVotes << " " << blockId << endl;
        }
        void say1() {
            cout << tconst << "        " << average << "        " << numVotes << "        " << blockId << endl;
        }


        int getBlockId(){
            return blockId;
        }

        void setBlockId(int id) {
            blockId = id;
        }

        string getTconst() {
            return tconst;
        }

        double getAverage() {
            return average;
        }

        int getNumVotes() {
            return numVotes;
        }

        void setTconst(string tc) {
            tconst = tc;
        }

        void setAverage(double avg) {
            average = avg;
        }

        void setNumVotes (int votes) {
            numVotes = votes;
        }

};

class Node {
	bool IS_LEAF;
	int size;
    Record** key_record;
	Node** ptr;
	friend class BPTree;
public:
	Node() {
        //dynamic memory allocation
        key_record = new Record*[MAX];
        ptr = new Node*[MAX+1];
    }
};

class BPTree {
	Node *root;
	void insertInternal(Record*,Node*,Node*);
	void removeInternal(Record*,Node*,Node*);
	Node* findParent(Node*,Node*);
public:
	BPTree();
	void search(double, vector<Record*>*);
    void searchRange(double, double, vector<Record*>*);
	void insert(Record *);
	void removeValue(double);
    void removeRange(double lowerBound, double upperBound);
	void display(Node*);
	Node* getRoot();
	void cleanUp(Node*);
	void height(Node*);
	~BPTree();
};

BPTree::BPTree()
{
	root = NULL;
}

void BPTree::search(double avg, vector<Record*> * vr) {
    searchRange(avg, avg, vr);
}

void BPTree::searchRange(double lower, double upper, vector<Record*> *vr) {
    //search logic
	if(root==NULL) {
		//empty
		cout << "Tree empty\n";
	}
	else {
		Node* cursor = root;
		//in the following while loop, cursor will travel to the leaf node possibly consisting the key
		int index_counter = 0;

		while(cursor->IS_LEAF == false) {

            index_counter++;
            cout << "Accessed Index Node Content " << index_counter << "\n";
            for(int i = 0; i < cursor->size; i++) {
                //Index Content
                cout << "Address to records = " << cursor->key_record[i] <<endl;
                cout << "Key value = " << cursor->key_record[i]->getAverage() << endl;
                //Return tconst attribute of record
                //cout << "Tconst of record = " << cursor->key_record[i]->getTconst() << endl; No need return here

            }
            cout << "\n";

			for(int i = 0; i < cursor->size; i++) {
				if( lower  <= (cursor->key_record[i])->getAverage() ) {
					cursor = cursor->ptr[i];
					break;
				}
				if(i == cursor->size - 1) {
					cursor = cursor->ptr[i+1];
					break;
				}
			}
		}
		cout << "Number of indexes accessed=  " << index_counter << endl;
        cout << "\n";
        int index = 0;
        Node *cursor_check = cursor;
        int block_id;
        vector <int> blocksAccessed;

        while (cursor_check->key_record[index]->getAverage() <= upper) {
            if (cursor_check->key_record[index]->getAverage() >= lower) {
                vr->push_back(cursor_check->key_record[index]);
                block_id = cursor_check->key_record[index]->getBlockId();
                cout << "Block ID #" << block_id << " Accessed" << endl;
                if ( std::find(blocksAccessed.begin(), blocksAccessed.end(), block_id) != blocksAccessed.end()) {
                }
                else{
                    blocksAccessed.push_back(block_id);
                }

            int blks = blocksAccessed.size();
            cout << "Number of blocks accessed =  " << blks << endl;
            }

            if (index == cursor_check->size-1) {
                //cursor_check->key_record[index]->say();
                cursor_check = cursor_check->ptr[index+1];
                index = 0;
            }
            else{
                index ++;
            }
            if (cursor_check == NULL) { // last record
                break;
            }
        }
	}
}

// Calculate height
void BPTree::height(Node* root) {
    int x = 0;
    int max_depth = 0;
    Node *cursor = root;
    if (cursor == NULL){
        max_depth = 0;
    }
    else{
        for(int i = 0; i < cursor->size; i++){
            int height = 0;
            while(cursor->IS_LEAF == false){
                cursor = cursor->ptr[i];
                height++;
                if (height>max_depth){
                    max_depth = height;
                }
            }
        }
    }
    cout << "Height = " << max_depth << "\n";
}

void BPTree::insert(Record * r)
{
	//insert logic
	if(root==NULL)
	{
		root = new Node;
		root->key_record[0] = r;
		root->IS_LEAF = true;
		root->size = 1;
		cout << "Created root\n Inserted " << r->getTconst() << " successfully\n";
	}
	else
	{
        double avg = r->getAverage();
		Node* cursor = root;
		Node* parent;
		//in the following while loop, cursor will travel to the leaf node possibly consisting the key
		while(cursor->IS_LEAF == false)
		{
			parent = cursor;
			for(int i = 0; i < cursor->size; i++)
			{
				if( avg < cursor->key_record[i]->getAverage() )
				{
					cursor = cursor->ptr[i];
					break;
				}
				if(i == cursor->size - 1)
				{
					cursor = cursor->ptr[i+1];
					break;
				}
			}
		}
		//now cursor is the leaf node in which we'll insert the new key
		if(cursor->size < MAX) {
			//if cursor is not full
			//find the correct position for new key
			int i = 0;
            Record ** rec = cursor->key_record;
			while( (i < cursor->size) && (avg >= rec[i]->getAverage() )) {
                cout << "insert Record" << rec[i]->getTconst() << "..." << endl;
                i++;
            }
			//make space for new key
			for(int j = cursor->size; j > i; j--) {
				rec[j] = rec[j-1];
			}
			cursor->key_record[i] = r;
			cursor->size++;
			cursor->ptr[cursor->size] = cursor->ptr[cursor->size-1];
			cursor->ptr[cursor->size-1] = NULL;
			cout << "Inserted record " << r->getTconst() << " successfully\n";
		}
		else {
			cout << "Inserted record " << r->getTconst() << " successfully\n";
			cout << "Overflow in leaf node!\nSplitting leaf node\n";
			//overflow condition
			//create new leaf node
			Node* newLeaf = new Node;
			//create a virtual node and insert x into it
			Record *virtualNode[MAX+1];
			for(int i = 0; i < MAX; i++) {
				virtualNode[i] = cursor->key_record[i];
			}
			int i = 0, j;
			while( i < MAX && (avg >= virtualNode[i]->getAverage()) ) i++;
			//make space for new key
			for(int j = MAX+1;j > i; j--) {
				virtualNode[j] = virtualNode[j-1];
			}
			virtualNode[i] = r;
            cout << "Split node of 4 to 2 nodes of 2\n";

			newLeaf->IS_LEAF = true;
			//split the cursor into two leaf nodes
			cursor->size = (MAX+1)/2;
			newLeaf->size = MAX+1-(MAX+1)/2;
			//make cursor point to new leaf node
			cursor->ptr[cursor->size] = newLeaf;
			//make new leaf node point to the next leaf node
			newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];
			cursor->ptr[MAX] = NULL;

			//now give elements to new leaf nodes
			for(i = 0; i < cursor->size; i++) {
				cursor->key_record[i] = virtualNode[i];
			}
			for(i = 0, j = cursor->size; i < newLeaf->size; i++, j++) {
				newLeaf->key_record[i] = virtualNode[j];
			}
			//modify the parent
			if(cursor == root) {
				//if cursor is a root node, we create a new root
				Node* newRoot = new Node;
				newRoot->key_record[0] = newLeaf->key_record[0];
				newRoot->ptr[0] = cursor;
				newRoot->ptr[1] = newLeaf;
				newRoot->IS_LEAF = false;
				newRoot->size = 1;
				root = newRoot;
				cout << "Created new root\n";
			}
			else {
				//insert new key in parent node
				insertInternal(newLeaf->key_record[0],parent,newLeaf);
			}
		}
	}
}

void BPTree::insertInternal(Record* r, Node* cursor, Node* child)
{
	if(cursor->size < MAX) {
		//if cursor is not full
		//find the correct position for new key
		int i = 0;
		while( i < cursor->size && (r->getAverage() >= cursor->key_record[i]->getAverage()) ) i++;
		//make space for new key
		for(int j = cursor->size;j > i; j--) {
			cursor->key_record[j] = cursor->key_record[j-1];
		}
        //make space for new pointer
		for(int j = cursor->size+1; j > i+1; j--) {
			cursor->ptr[j] = cursor->ptr[j-1];
		}
		cursor->key_record[i] = r;
		cursor->size++;
		cursor->ptr[i+1] = child;
		cout <<"Inserted key in an Internal node successfully\n";
	}
	else
	{
		cout <<"Inserted key in an Internal node successfully\n";
		cout <<"Overflow in internal node!\nSplitting internal node\n";
		//if overflow in internal node
		//create new internal node
		Node* newInternal = new Node;
		//create virtual Internal Node;
		Record *virtualKey[MAX+1];
		Node* virtualPtr[MAX+2];
		for(int i = 0; i < MAX; i++)
		{
			virtualKey[i] = cursor->key_record[i];
		}
		for(int i = 0; i < MAX+1; i++) {
			virtualPtr[i] = cursor->ptr[i];
		}
		int i = 0, j;
		while( i < MAX && (r->getAverage() >= virtualKey[i]->getAverage() ) ) i++;
		//make space for new key
		for(int j = MAX+1;j > i; j--) {
			virtualKey[j] = virtualKey[j-1];
		}
		virtualKey[i] = r;

		for(int j = MAX+2;j > i+1; j--)
		{
			virtualPtr[j] = virtualPtr[j-1];
		}
		virtualPtr[i+1] = child;
		newInternal->IS_LEAF = false;
		//split cursor into two nodes
		cursor->size = (MAX+1)/2;
		newInternal->size = MAX-(MAX+1)/2;
		//give elements and pointers to the new node
		for(i = 0, j = cursor->size+1; i < newInternal->size; i++, j++) {
			newInternal->key_record[i] = virtualKey[j];
		}
		for(i = 0, j = cursor->size+1; i < newInternal->size+1; i++, j++) {
			newInternal->ptr[i] = virtualPtr[j];
		}
		// m = cursor->key[cursor->size]
		if(cursor == root)
		{
			//if cursor is a root node, we create a new root
			Node* newRoot = new Node;
			newRoot->key_record[0] = cursor->key_record[cursor->size];
			newRoot->ptr[0] = cursor;
			newRoot->ptr[1] = newInternal;
			newRoot->IS_LEAF = false;
			newRoot->size = 1;
			root = newRoot;
			cout <<"Created new root\n";
		}
		else
		{
			//recursion
			//find depth first search to find parent of cursor
			insertInternal(cursor->key_record[cursor->size] ,findParent(root,cursor) ,newInternal);
		}
	}
}

Node* BPTree::findParent(Node* cursor, Node* child)
{
	//finds parent using depth first traversal and ignores leaf nodes as they cannot be parents
	//also ignores second last level because we will never find parent of a leaf node during insertion using this function
	Node* parent;
	if(cursor->IS_LEAF || (cursor->ptr[0])->IS_LEAF) {
		return NULL;
	}
	for(int i = 0; i < cursor->size+1; i++)
	{
		if(cursor->ptr[i] == child)
		{
			parent = cursor;
			return parent;
		}
		else
		{
			parent = findParent(cursor->ptr[i],child);
			if(parent!=NULL)return parent;
		}
	}
	return parent;
}

void BPTree::removeRange(double lower_bound, double upper_bound) {
    cout << "removeRange\n";
}

void BPTree::removeInternal(Record* r, Node* cursor, Node* child) {
    cout << "removeInternal\n";
}

int numNodes = 0;
void BPTree::display(Node* cursor)
{
	//depth first display
	if(cursor!=NULL) {
        numNodes++;

		for(int i = 0; i < cursor->size; i++) {
			cout << cursor->key_record[i] << " " << cursor->key_record[i]->getAverage() << " ";

        }
		cout<<"\n";
		if(cursor->IS_LEAF != true)
		{
			for(int i = 0; i < cursor->size+1; i++)
			{
				display(cursor->ptr[i]);
			}
		}
	}
	else{
        cout << "Empty Tree\n";
	}
}


Node* BPTree::getRoot() {
    return root;
}

void BPTree::cleanUp(Node* cursor)
{
    //clean up logic
    if(cursor!=NULL) {

        delete cursor->key_record;
        delete cursor->ptr;
        delete cursor;
        cursor = nullptr;
        // check if cursor is empty
        if (cursor != nullptr)
        {
            cout << "Haven't deleted all yet!" << endl;
        }

        else
        {

            cout << "Deleted all!" << endl;
        }
    }

}

BPTree::~BPTree()
{
	//calling cleanUp routine
	cleanUp(root);
}

void f(vector<Record*> *vr, Record *r) {
    vr->push_back(r);
}

vector<Record*> f1(Record *r) {
    vector<Record*> vrptr;
    vrptr.push_back(r);
    return vrptr;
}

class Block {
    int id;
    vector <Record> recordVector;

    public:
        Block() {
            id = 0;
        }
        Block(int id1) {
            id = id1;
        }


        // return
            // 0: Can add 1 more record;
            // 1: Memory exceed 100B
        int checkBlockAvailabilty() {
            int recordSize = (int) sizeof(Record);
            int total = recordVector.size();
            int base = (int) (sizeof(id) + recordSize*total);
            // cout << "Size of block " << id << " is " << base << endl;

            if (base + recordSize > BLOCK_SIZE) {
                // cout << "Memory exceed." << endl;
                return 1;
            }
            else {
                // cout << "Can add 1 record to block " << id << endl;
                return 0;
            }
        }

        // add record to block if there is room
        // need to check rec is NULL (not yet)
        void addRecord(Record &rec) {
            recordVector.push_back(rec);
            rec.setBlockId(id);
            // cout << "Add record " << rec.getTconst() << " to block " << id << endl;
            // cout << "Size of block " << id << " is " << 4+48*recordVector.size() << endl;
        }

        // delete a record from a block, given its position
        // legal position: 0 -> vector.size()-1
        void deleteRecord(int position) {
            if (position >= recordVector.size() || position < 0) {
                cout << "Position of record not available" << endl;
            }
            else {
                recordVector.erase(recordVector.begin() + position);
                cout << "Delete record!" << endl;
            }
        }

        Record * getRecord(int position) {
            if (position < 0 || position >= recordVector.size()) {
                cout << "Invalid position in block " << id << endl;
                return NULL;
            }
            else {
                return &recordVector[position];
            }
        }

        int getBlockSize() {
            return (int) sizeof(int) + recordVector.size() * sizeof(Record);
        }

        void say() {
            cout << recordVector.size() << " records in block " << id << endl;
        }

};

class DBManager {
    int current_size; // = blockVector size
    vector <Block> blockVector;

    public:
        DBManager() {
            current_size = 0;
        }



        // check avalabilty in the last block
        // if full, create new block and add record to the new one
        // else, add record to the last block
        void addRecord(Record &r) {
            if (blockVector.empty()) { // empty DB
                Block b(++current_size);
                b.addRecord(r);
                blockVector.push_back(b);
                ;
            }
            else {
                // extract the last block
                Block *lastBlock;
                lastBlock = &blockVector[current_size-1];

                // check for availabilty: 0 (can add), 1 (full)
                int avalability = lastBlock->checkBlockAvailabilty();
                if (avalability == 1) {
                    Block b(++current_size);
                    b.addRecord(r);
                    blockVector.push_back(b);
                }
                else if (avalability == 0) {
                    lastBlock->addRecord(r);
                }
            }
        }

        // detele block at position
        void deleteBlock(int position) {
            if (position >= blockVector.size() || position < 0) {
                cout << "Position of block not available" << endl;
            }
            else {
                blockVector.erase(blockVector.begin() + position);
                cout << "Delete block!";
            }
        }

        // only help to reduce size of vector to n if n < current_size:
        // keep the first n items
        void resizeBlockVector(int new_size) {
            if (new_size < current_size) {
                blockVector.resize(new_size);
            }
        }



        // return pointer to Block given the index / position
        Block *getBlock (int position) {
            if (position < 0 || position >= current_size) {
                cout << "Invalid block position in DB\n";
                return NULL;
            }
            else {
                return &blockVector[position];
            }
        }

        Record * getRecord() {
            return NULL;
        }

        void removeLast(Block *block_ptr) {
            blockVector.pop_back();
        }

        // read file for a limited number of line
        // if limit = 0, then read all lines in the file
        void readFile(string filepath, int limit) {
            string delim = "\t";
            ifstream file(filepath);
            if (file.is_open()) {
                cout << "File opened" << endl;
                string line;
                int i = 0;
                getline(file, line);
                while (getline(file, line) && ((i < limit) || limit == 0)) {
                    // cout << line << endl;
                    vector<string>tokens = split(line, delim);

                    string tc = tokens[0];
                    double avg = stod(tokens[1]);
                    int votes = stoi(tokens[2]);

                    Record r(tc, avg, votes);
                    addRecord(r);
                    i++;
                }
                file.close();
            }
            else {
                exit(EXIT_FAILURE);
            }
        }


        // return number of blocks
        int numOfBlocks() {
            return current_size;
        }

        void showDBSize() {
            if (current_size == 0) {
                cout << "Nothing inside\n";
            }
            else {
                Block* lastBlock = getBlock(current_size-1);
                int lastBlockSize = lastBlock->getBlockSize();
                int prevSize = (int) (current_size-1) * BLOCK_SIZE;
                cout << "Number of blocks of DB: " << current_size << endl;
                cout << "Size of Database (B): " << lastBlockSize + prevSize << endl;
            }
        }
        void free() {
            current_size = 0;
            blockVector.clear();
        }

};

int main() {
    DBManager db;
    db.readFile("data.tsv", 0);
    vector <Record*> vectorRecord;
    // for block pointers
   	vector <Block*> testBlocks;
   	// for inserting records into blocks
    vector <Record*> recordsToBlock;
    // for creating records that would be added to b+ tree

    BPTree bpt;

    int n = 0;
    int numOfBlocks = db.numOfBlocks();  //Modify to change sample size
    int choice = -1;
    bool loop = true;

    while(loop == true){
        cout << "\n";
        cout << "Enter 1 to see numOfBlocks and DBSize\n";
        cout << "Enter 2 to store records from database to B+ tree indexing\n";
        cout << "Enter 3 to display B+ Tree and find number of nodes\n";
        cout << "Enter 4 to find height of B+Tree\n";
        cout << "Enter 5 to search movies according to averageRatings\n";
        cout << "Enter 6 to search movies according to a range of averageRatings\n";
        cout << "Enter 7 to delete movies according to averageRatings\n";
        cout << "Enter 8 to exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cout << "--------------------------------------------------------\n";

        switch(choice) {
            case 1:
                db.showDBSize();
                break;
            case 2:
                for (int i = 0; i < numOfBlocks; i++){                                            //Loop blocks
                    testBlocks.emplace_back(db.getBlock(i));                        //Add block pointer into vector

                    for(int j=0;j<2;j++){                                           //Loop record 0 and 1 within the block
                        recordsToBlock.emplace_back(testBlocks[i]->getRecord(j));   //Place record inside another vector
                        recordsToBlock[n]->say();
                        bpt.insert(recordsToBlock[n]);                               //Insert record from vector to B+ Tree
                        n++;

                    }
                }
                break;
            case 3:
                cout << "Display B+Tree\n";
                bpt.display(bpt.getRoot());
                cout << "Number of nodes in B+Tree: " << numNodes << endl;
                break;
            case 4:
                cout << "Height of B+Tree" << endl;
                bpt.height(bpt.getRoot());
                break;
            case 5:
                cout << "Input the averageRatings you want to search: " << endl;
                double average;
                cin >> average;
                cout << "\n";
                cout << "Searching for Tconst of movies with averageRatings of " << average << "....." << endl;
                cout << "\n";
                bpt.search(average, &vectorRecord);
                cout << "Content of Data Blocks Accessed\n";
                cout << "Tconst | averageRatings | numVotes | BlockID" << endl;
                for (auto j: vectorRecord) {
                    j->say1();
                }
                vectorRecord.clear();
                break;
            case 6:
                double minimum;
                double maximum;
                cout << "Input the minimum averageRatings  to search: " << endl;
                cin >> minimum;
                cout << "Input the maximum averageRatings to search: " << endl;
                cin >> maximum;
                cout << "Searching for Tconst of movies with averageRatings of " << minimum << "to" << maximum << "." << endl;
                bpt.searchRange(minimum, maximum,&vectorRecord);
                cout << "Content of Data Blocks Accessed\n";
                cout << "Tconst | averageRatings | numVotes | BlockID" << endl;
                for (auto j: vectorRecord) {
                    j->say1();

                }
                vectorRecord.clear();
                break;
            case 7:
                double avg;
                cout << "Input the averageRatings of movies you want to delete: " <<endl;
                cin >> avg;
                break;
            case 8:
                cout<<"\n";
                cout << "Exiting.." << endl;
                loop = false;
                break;
            default :
                cout << "Invalid input. Please enter again." << endl;
                break;
        }
    };

    return 0;

}
