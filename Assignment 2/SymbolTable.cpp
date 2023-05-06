#include "SymbolTable.h"

void SymbolTable::run(string filename)
{
    ifstream file;
    file.open(filename, ios::in);
    string tmp = "";
    //int ccc = 0;
    while (!file.eof()) 
    {
        //cout << ++ccc <<": ";
        getline(file, tmp);
        string cmd[4];
        commandTokenize(tmp, cmd);
        if (cmd[0] == "INSERT") 
        {
            string flag = insert(cmd[1], cmd[2], cmd[3]);
            if (flag == "InvalidInstruction") throw InvalidInstruction(tmp);
            if (flag == "InvalidDeclaration") throw InvalidDeclaration(tmp);
            if (flag == "Redeclared") throw Redeclared(tmp);
            cout << flag << endl;
        }
        else if (cmd[0] == "ASSIGN")
        {
            cmd[2] += cmd[3];
            string flag = assign(cmd[1], cmd[2]);
            if (flag == "InvalidInstruction") throw InvalidInstruction(tmp);
            if (flag == "Undeclared") throw Undeclared(tmp);
            if (flag == "TypeMismatch") throw TypeMismatch(tmp);
            cout << flag << endl;
        }
        else if (cmd[0] == "BEGIN" && cmd[1] == "" && cmd[2] == "" && cmd[3] == "")
        {
            this->beginScope();
        }
        else if (cmd[0] == "END" && cmd[1] == "" && cmd[2] == "" && cmd[3] == "")
        {
            bool flag = endScope();
            if (flag == false) throw UnknownBlock();
        }
        else if (cmd[0] == "LOOKUP" && is_identifier_name(cmd[1]) && cmd[2] == "" && cmd[3] == "") 
        {
            int a = 0, b = 0;
            Node* flag = lookup(cmd[1], a, b);
            if (flag == nullptr) throw Undeclared(tmp);
            cout << flag->level << endl;
        }
        else if (cmd[0] == "PRINT" && cmd[1] == "" && cmd[2] == "" && cmd[3] == "")
        {
            string res = "";
            print(this->root, res);
            if (res != "") {
                res.erase(res.end() - 1);
                cout << res << endl;
            } 
        }
        else throw InvalidInstruction(tmp);
    }
    file.close();
    if (this->curr_level != 0) throw UnclosedBlock(this->curr_level);
}


Node* SymbolTable::lookup(string name, int& num_comp, int& num_splay) {
    Block* pBlock = this->curr_block;
    int plevel = this->curr_level;
    Node* p = nullptr;
    string found = "";
    while (pBlock != nullptr) {
        string _name_ = " " + name + " ";
        if ((int)pBlock->mem.find(_name_) != -1) {
            found = name;
            break;
        }
        plevel--;
        pBlock = pBlock->prev;
    }
    if (found == "") return nullptr;
    p = search(name, plevel, num_comp, num_splay);
    return p;
}


string SymbolTable::insert(string name, string str, string _static) {
    if (!is_identifier_name(name) || !is_static(_static)) return "InvalidInstruction";
    Node* newNode = new Node();
    if (is_type(str)) {
        newNode->type = str;
        newNode->name = name;
        if (str == "string") newNode->value = "''";
        else if (str == "number") newNode->value = "";
        if (_static == "true") newNode->level = 0;
        else newNode->level = this->curr_level;
        newNode->function = false;
        newNode->parent = nullptr;
        newNode->left = nullptr;
        newNode->right = nullptr;
    }  
    else {
        int l = str.length();
        if (l < 10 || str[0] != '(' || str[l - 9] != ')'
        || str[l - 8] != '-' || str[l - 7] != '>') return "InvalidInstruction"; 
        //------------general syntax is ok---------
        string type = str.substr(l - 6, 6);
        if (!is_type(type)) return "InvalidInstruction"; 
        //--------------type is ok-----------------
        str = str.substr(1, l - 10);
        string etl = "";
        if (str != "") {
            etl = typeTokenize(str);
            if (etl == "false") return "InvalidInstruction"; 
        }
        //-----------etl is ok---------------------
        newNode->name = name;
        newNode->type = type;
        if (type == "string") newNode->value = "''";
        else newNode->value = "";
        if (_static == "true") newNode->level = 0;
        else newNode->level = this->curr_level;
        newNode->function = true;
        newNode->encoded_type_lis = etl;
        newNode->parent = nullptr;
        newNode->left = nullptr;
        newNode->right = nullptr;
        //if (!newNode) return "InvalidInstruction"; 
        if (newNode->level > 0 && this->curr_level > 0) {
            delete newNode;
            return "InvalidDeclaration";
        } 
    } 
    int num_comp = 0;
    int num_splay = 0;
    if (!insert(newNode, num_comp, num_splay)) {
        delete newNode;
        return "Redeclared";
    } 

    //addIdentifier(name, _static);
    if (_static == "false") {
            this->curr_block->mem += name;
            this->curr_block->mem += " ";
        } 
    else {
        this->global_block->mem += name;
        this->global_block->mem += " ";
    }
    return to_string(num_comp) + " " + to_string(num_splay);
}

void SymbolTable::beginScope() {
    Block* newBlock = new Block();
    newBlock->prev = this->curr_block;
    this->curr_block = newBlock;
    this->curr_level++;
}

bool SymbolTable::endScope() {
    if (this->curr_level == 0) return false;
    //----------tokenizing to delete-----------
    string key = " ";
    string str = this->curr_block->mem;
    if (str != " ") {
        str.erase(str.end() - 1);
        str.erase(str.begin());
        int start = 0;
        int end = (int)str.find(key);
        string del;

        while(end != -1) {
            del = str.substr(start, end - start);
            remove(del, this->curr_level);
            start = (int)end + (int)key.size();
            end = (int)str.find(key, start);
        }
        del = str.substr(start, end - start);
        remove(del, this->curr_level);
    }
    //---------delete block-------------------
    Block* temp = curr_block;
    curr_block = curr_block->prev;
    curr_level--;
    delete temp;
    return true;
}

string SymbolTable::assign(string name, string str) {
    if (!is_identifier_name(name)) return "InvalidInstruction";
    Node* p;
    int cp = 0, sp = 0;

    if (is_identifier_name(str)) {
        int cq = 0, sq = 0;
        Node* q = lookup(str, cq, sq);
        if (!q) return "Undeclared";
        if (q->function) return "TypeMismatch";
        p = lookup(name, cp, sp);
        if (!p) return "Undeclared";
        if (p->function) return "TypeMismatch";
        if (is_type_mismatched(p->type, q->type)) return "TypeMismatch";
        p->value = q->value;
        return to_string(cp + cq) + " " + to_string(sp + sq);
    }

    if (is_value(str)) {
        p = lookup(name, cp, sp);
        if (!p) return "Undeclared";
        if (p->function) return "TypeMismatch";
        if (is_type_mismatched(p->type, str)) return "TypeMismatch";
        p->value = str;
        return to_string(cp) + " " + to_string(sp);
    }
    
    int l = str.length();
    int found = str.find("(");
    if ((found == -1) || (str[l - 1] != ')')) return "InvalidInstruction";
    string func_name = str.substr(0, found);
    if (!is_identifier_name(func_name)) return "InvalidInstruction";
    int cf = 0, sf = 0;
    Node* f = lookup(func_name, cf, sf);
    if (!f) return "Undeclared";
    if (f->function == false) return "TypeMismatch";
    
    string value_lis = str.substr(found + 1, (l-2) - (found+1) + 1);
    string type = f->encoded_type_lis;
    int lv = value_lis.length();
    int lt = type.length();
    if (lt == 0 && lv != 0) return "TypeMismatch";
    if (lt != 0 && lv == 0) return "TypeMismatch";
    if (lt == 0 && lv == 0) {
        p = lookup(name, cp, sp);
        if (!p) return "Undeclared";
        if (p->function) return "TypeMismatch"; 
        if (is_type_mismatched(p->type, f->type)) return "TypeMismatch";
        return to_string(cp + cf) + " " + to_string(sp + sf);
    }
    int cc = 1;
    for (int i = 0; i < lv; ++i) {
        if (value_lis[i] == ',') cc++;
    }
    if (cc != lt) return "TypeMismatch";
    // tokenize
    string value[cc];
    string key = ",";
    int start = 0;
    int end = (int)value_lis.find(key);
    int idx = 0;
    while(end != -1) {
        value[idx] = value_lis.substr(start, end - start);
        start = (int)end + (int)key.size();
        end = (int)value_lis.find(key, start);
        ++idx;
    }
    value[idx] = value_lis.substr(start, end);

    Node* q;
    int sum_cq = 0, sum_sq = 0;
    for (int i = 0; i < cc; ++i) {
        if (is_number(value[i])) {
            if (type[i] != 'n') return "TypeMismatch";
        }
        else if (is_string(value[i])) {
            if (type[i] != 's') return "TypeMismatch";
        }
        else if (is_identifier_name(value[i])) {
            int cq = 0, sq = 0;
            q = lookup(value[i], cq, sq);
            if (!q) return "Undeclared";
            if (q->function) return "TypeMismatch";
            if ((q->type == "number" && type[i] == 's')
            || (q->type == "string" && type[i] == 'n')) return "TypeMismatch";
            sum_cq += cq; sum_sq += sq;
        }
        else return "InvalidInstruction";
    }

    p = lookup(name, cp, sp);
    if (!p) return "Undeclared";
    if (p->function) return "TypeMismatch";
    if (is_type_mismatched(p->type, f->type)) return "TypeMismatch";
    return to_string(cp + sum_cq + cf) + " " + to_string(sp + sum_sq + sf);
}

void SymbolTable::print(Node* p, string& res) {
    if (p == nullptr) return;
    res += (p->name + "//" + to_string(p->level) + " ");
    print(p->left, res);
    print(p->right, res);
}