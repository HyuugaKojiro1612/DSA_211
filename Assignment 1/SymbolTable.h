#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Node {
public:
    string name, type, value;
    Node* prev;
    Node* next;

    Node() : prev(nullptr), next(nullptr) {};
    Node(string name, string type) {
        this->name = name;
        this->type = type;
        if (type == "string") this->value = "''";
        else if (type == "number") this->value = "";
        prev = nullptr;
        next = nullptr;
    }
};

class Block {
public:
    Node* head;
    Node* tail;
    Block* prev;
    int level;
public:
    Block() {
        head = nullptr; 
        tail = nullptr;
        prev = nullptr;
        level = 0;
    }
    ~Block() {
        this->clear();
    }

    void clear() {
        if (head == nullptr) return;
        Node* p = head;
        while (head != nullptr) {
            p = head;
            head = head->next;
            delete p;
        }
        tail = nullptr;
        level = 0;
    }

    void prepend(string name, string type) {
        Node* newNode = new Node(name, type);
        if (this->head == nullptr) {
            head = newNode;
            tail = newNode;
            return;
        }
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }

    Node* search(string name) {
        if(head == nullptr) return nullptr;
        Node* p = head;
        while (p != nullptr) {
            if (p->name == name) return p; 
            p = p->next;
        }
        return nullptr;
    }

    // void printST() {
    //     cout << "NULL";
    //     Node* p = head;
    //     while (p != nullptr) {
    //         cout << "<==(" << p->name << '-' << p->type << '-' << p->value << ')'; 
    //         p = p->next;}
    //     cout << "<==HEAD" << endl;
    // }

};

class SymbolTable {
    Block* curr_block;
public:
    SymbolTable() {
        curr_block = new Block();
    }
    ~SymbolTable() {
        this->clear();
    }
    void clear() {
        if (curr_block == nullptr) return;
        Block* p = curr_block;
        while (curr_block != nullptr) {
            p = curr_block;
            curr_block = curr_block->prev;
            p->clear();
            delete p;
        }
    }
    void run(string filename);
    string insert(string name, string type);
    string assign(string name, string value);
    Node* lookup(string name, int& level);
    void print(Block* pBlock, int& flag);
    void rprint();
    void beginScope();
    bool endScope();

    bool rlookup(Block* block, string name);
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

bool is_value(string value) {
    return is_number(value) || is_string(value);
}

bool is_type_mismatched(string type, string value) {
    if (type == "number" && (is_number(value) || value == "number")) return false;
    if (type == "string" && (is_string(value) || value == "string")) return false;
    return true;
}

void tokenize(string str, string* res, string key = " ") 
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
#endif