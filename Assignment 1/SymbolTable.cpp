#include "SymbolTable.h"

void SymbolTable::run(string filename) 
{
    ifstream file;
    file.open(filename, ios::in);
    string tmp = "";
    while(!file.eof()) 
    {
        getline(file, tmp);
        string cmd[3];
        tokenize(tmp, cmd);
        if (cmd[0] == "INSERT") 
        {
            string flag = insert(cmd[1], cmd[2]);
            if (flag == "InvalidInstruction") throw InvalidInstruction(tmp);
            if (flag == "Redeclared") throw Redeclared(tmp);
            cout << flag << endl;
        }
        else if (cmd[0] == "ASSIGN") 
        {
            string flag = assign(cmd[1], cmd[2]);
            if (flag == "InvalidInstruction") throw InvalidInstruction(tmp);
            if (flag == "Undeclared") throw Undeclared(tmp);
            if (flag == "TypeMismatch") throw TypeMismatch(tmp);
            cout << "success" << endl;
        }
        else if (cmd[0] == "BEGIN" && cmd[1] == "" && cmd[2] == "")
        {
            this->beginScope();
        }
        else if (cmd[0] == "END" && cmd[1] == "" && cmd[2] == "") 
        {
            bool flag = endScope();
            if (flag == false) throw UnknownBlock();
        }
        else if (cmd[0] == "LOOKUP" && is_identifier_name(cmd[1]) && cmd[2] == "") 
        {
            int level = 0;
            bool flag = lookup(cmd[1], level);
            if (flag == false) throw Undeclared(tmp);
            cout << level << endl;
        }
        else if (cmd[0] == "PRINT" && cmd[1] == "" && cmd[2] == "") 
        {
            int flag = 0;
            this->print(this->curr_block, flag);
            if (flag) cout << endl;
        }
        else if (cmd[0] == "RPRINT" && cmd[1] == "" && cmd[2] == "")
        {
            this->rprint();
        }
        else throw InvalidInstruction(tmp);
    }
    file.close();
    if (this->curr_block->level != 0) throw UnclosedBlock(this->curr_block->level);
}

string SymbolTable::insert(string name, string type) {
    //insert an identifier at the head of the Scope LL

    if (!is_identifier_name(name) || !is_type(type)) return "InvalidInstruction";
    if (this->curr_block->search(name) != nullptr) return "Redeclared"; 
    
    this->curr_block->prepend(name, type);
    return "success";
}

string SymbolTable::assign(string name, string value) {
    if (!is_identifier_name(name)) return "InvalidInstruction";
    int p_level = 0;
    Node* p = this->lookup(name, p_level);

    if (is_identifier_name(value)) {
        int q_level = 0;
        Node* q = this->lookup(value, q_level);
        if (!p || !q) return "Undeclared";
        if (is_type_mismatched(p->type, q->type)) return "TypeMismatch";
        p->value = q->value;
        return "success";
    }
    else if (is_value(value)) {
        if (!p) return "Undeclared";
        if (is_type_mismatched(p->type, value)) return "TypeMismatch";
        p->value = value;
        return "success";
    }
    return "InvalidInstruction";
}

void SymbolTable::beginScope() {
    Block* newBlock = new Block();
    newBlock->prev = curr_block;
    newBlock->level = curr_block->level + 1;
    curr_block = newBlock;
    //++curr_level;
}

bool SymbolTable::endScope() {
    if (curr_block->level == 0) return false;
    Block* p = curr_block;
    curr_block = curr_block->prev;
    p->clear();
    delete p;
    //--curr_level;
    return true;
}

Node* SymbolTable::lookup(string name, int& level) {
    Block* pBlock = curr_block;
    Node* p;
    level = curr_block->level;
    while (pBlock != nullptr) {
        p = pBlock->search(name);
        if (p != nullptr) {
            return p;
        } 
        --level;
        pBlock = pBlock->prev;
    }
    return nullptr;
}

bool SymbolTable::rlookup(Block* block, string name) {
    Block* pBlock = curr_block;
    while (pBlock != block) {
        if (pBlock->search(name) != nullptr) return true;
        pBlock = pBlock->prev;
    }
    return false;
}

void SymbolTable::rprint() {
    Block* pBlock = curr_block;
    int level = curr_block->level;
    int flag = 0;
    Node* p;
    while (pBlock != nullptr) {
        p = pBlock->head;
        while (p != nullptr) {
            if (rlookup(pBlock, p->name) == 0) {
                if (flag) cout << ' ';
                cout << p->name << "//" << level;
                ++flag;
            } 
            p = p->next;
        }
        pBlock = pBlock->prev;
        --level;
    }
    if (flag) cout << endl;
}

void SymbolTable::print(Block* pBlock, int& flag) {
    if (pBlock == nullptr) {
        return;
    }
    print(pBlock->prev, flag);
    int level = pBlock->level;
    Node* p = pBlock->tail;
    while (p != nullptr) {
        if (rlookup(pBlock, p->name) == 0) {
            if (flag) cout << ' ';
            cout << p->name << "//" << level;
            ++flag;
        } 
        p = p->prev;
    }
}