#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Node {
public:
    string name, type = "NONE", value;
    int level;

    bool func;
    string encoded_paras = "";

    Node() {}

    Node(string name, int level, bool func, string nop) {
        this->name = name;
        this->level = level;
        this->func = func;
        if (this->func) {
            for (int i = 0; i < stoi(nop); ++i)
                this->encoded_paras += " ";
        }
    }
};

class Block {
public:
    string mem;
    Block* prev;

    Block() {
        mem = " ";
        prev = nullptr;
    }
};

enum STATUS_TYPE {NIL, NON_EMPTY, DELETED};

string encodeKey(string name, int level) {
    string res = to_string(level);
    for (int i = 0; i < (int)name.length(); ++i) {
        res += to_string((int)name[i] - 48);
    }
    return res;
}

int mod(string num, int k) {
    int res = 0;
    for (int i = 0; i < (int)num.length(); i++)
        res = (res * 10 + (int)num[i] - '0') % k;
    return res;
}
int linearProbing(string key, int m, int c1, int c2, int i) {
    int h = mod(key, m);
    return (h + c1*i + 0*c2) % m;
}

int quadraticProbing(string key, int m, int c1, int c2, int i) {
    int h = mod(key, m);
    return (h + c1*i + c2*i*i) % m;
}

int doubleHashing(string key, int m, int c1, int c2, int i) {
    int h1 = mod(key, m);
    int h2 = 1 + mod(key, m - 2);
    return (h1 + c1*i*h2 + 0*c2) % m;
}

class SymbolTable {
public:
    int (*hp)(string, int, int, int, int);
    STATUS_TYPE* status;
    Node* data;
    int size;
    int c1, c2;
    int count = 0; //for print
    int curr_level;
    Block* curr_block;

    SymbolTable() {
        this->data = nullptr;
        this->status = nullptr;
        this->size = 0;

        this->curr_level = 0;
        Block* newBlock = new Block();
        this->curr_block = newBlock;
    }
    ~SymbolTable() {
        this->blockClear();
        this->hashClear();
    }
    void initializeHash(int size, int (*hp)(string, int, int, int, int)) {
        this->hp = hp;
        this->size = size;
        this->data = new Node[size];
        this->status = new STATUS_TYPE[size];
        for (int i = 0; i < size; i++) {
            this->status[i] = NIL;
        }

        this->curr_level = 0;
    }
    void blockClear() {
        if (this->curr_block == nullptr) return;
        Block* p = curr_block;
        while (curr_block) {
            p = curr_block;
            curr_block = curr_block->prev;
            delete p;
        }
    }

    void hashClear() {
        if (this->data != NULL) {
            delete[] this->data;
            delete[] this->status;
            this->hp = nullptr;
            this->size = 0;
            this->count = 0;
        }
    }

protected:
    int insert(Node node, int& i) {
        i = 0;
        string key = encodeKey(node.name, node.level);
        //cout << this->size << endl;
        while (i < this->size) {
            int hashVal = this->hp(key, this->size, this->c1, this->c2, i);
            //cout << "hashVal: " << hashVal << endl;
            if (this->status[hashVal] == NIL
            || this->status[hashVal] == DELETED) {
                this->status[hashVal] = NON_EMPTY;
                this->data[hashVal] = node;
                return hashVal;
            }
            else ++i;
        }
        return -1;
    }

    int search(string name, int level, int& i) {
        i = 0;
        string key = encodeKey(name, level);
        //cout << "in search," << name << "size = " << this->size << endl;
        while (i < this->size) {
            int hashVal = this->hp(key, this->size, this->c1, this->c2, i);
            if (this->data[hashVal].name == name
            && this->data[hashVal].level == level
            && this->status[hashVal] == NON_EMPTY) {
                return hashVal;
            }
            else if (this->status[hashVal] == NIL) {
                return -1;
            }
            else ++i;
        }
        return -1;
    }
// xong remove, lam lookup
    void remove(string name, int level) {
        int i = 0;
        int pos = this->search(name, level, i);
        // if (pos == -1); // Non-existed situation
        // this->data[pos].name = "";
        this->status[pos] = DELETED;
    }
public:
    void run(string filename);
    string insert(string name, string nop);
    string assign(string name, string value);
    string call(string expression);
    void beginScope();
    bool endScope();
    int lookup(string name, int& i);
    void print();
};



bool is_string(string value) {
    int length = value.length();
    if (value[0] == 39 && value[length - 1] == 39) {
        for (int i = 1; i < length -1; ++i) {
            if (!isalnum(value[i]) && value[i] != ' ') return false;
        }
        return true;
    }
    return false;
}
bool is_number(string value) {
    int length = value.length();
    for (int i = 0; i < length; ++i) {
        if (!isdigit(value[i])) return false;
    }
    return true;
}

bool is_value(string value) {
    return is_number(value) || is_string(value);
} 

bool is_identifier_name(string value) {
    int length = value.length();
    if (isalpha(value[0])) {
        for (int i = 1; i < length - 1; ++i) {
            if (!isalnum(value[i]) && value[i] != '_') return false;
        }
        return true;
    }
    return false;
}

bool is_type(string type) {
    if (type == "number" || type == "string") return true;
    return false;
}

bool is_type_mismatched(string type, string value) {
    if (type == "number" && (is_number(value) || value == "number")) return false;
    if (type == "string" && (is_string(value) || value == "string")) return false;
    return true;
}

void commandTokenize(string str, string* res, string key = " ")
{
    int length = str.length();//
    int start = 0;
    int end = (int)str.find(key);
    int idx = 0;

    while(end != -1 && (idx != 2)/**/)
    {
        res[idx] = str.substr(start, end - start);
        start = (int)end + (int)key.size();
        end = (int)str.find(key, start);
        ++idx;
    }
    res[idx] = str.substr(start, length);
}

void probeTokenize(string str, string* res, string key = " ")
{
    int length = str.length();//
    int start = 0;
    int end = (int)str.find(key);
    int idx = 0;

    while(end != -1 && (idx != 3)/**/)
    {
        res[idx] = str.substr(start, end - start);
        start = (int)end + (int)key.size();
        end = (int)str.find(key, start);
        ++idx;
    }
    res[idx] = str.substr(start, length);
}
bool is_integer(string value) {
    if (value == "0") return true;
    int length = value.length();
    if (value[0] == '0' || !isdigit(value[0])) return false;
    for (int i = 1; i < length; ++i) {
        if (!isdigit(value[i])) return false;
    }
    return true;
}
string typeTokenize(string str, string key = ",")
{
    string encoded_type_lis = "";
    int start = 0;
    int end = (int)str.find(key);
    string tmp ="";

    while(end != -1) {
        tmp = str.substr(start, end - start);
        if (tmp == "number") encoded_type_lis += "n";
        else if (tmp == "string") encoded_type_lis += "s";
        else return "false";
        start = (int)end + (int)key.size();
        end = (int)str.find(key, start);
    }
    tmp = str.substr(start, end - start);
    if (tmp == "number") encoded_type_lis += "n";
    else if (tmp == "string") encoded_type_lis += "s";
    else return "false";
    return encoded_type_lis;
}
#endif