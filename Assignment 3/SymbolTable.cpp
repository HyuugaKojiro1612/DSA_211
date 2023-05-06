#include "SymbolTable.h"

void SymbolTable::run(string filename)
{
    ifstream file;
    file.open(filename, ios::in);
    string tmp = "";
    if (!file.eof()) {
        getline(file, tmp);
        string cmd[4];
        probeTokenize(tmp, cmd);
        if (cmd[0] == "LINEAR" && cmd[3] == "")
        {
            if (!is_integer(cmd[1]) || !is_integer(cmd[2])) throw InvalidInstruction(tmp);
            initializeHash(stoi(cmd[1]), linearProbing);
            this->c1 = stoi(cmd[2]);
            this->c2 = 0;
            //this->size = stoi(cmd[1]);
            //cout << "in run, this size " << this->size << endl;
        }
        else if (cmd[0] == "QUADRATIC")
        {
            if (!is_integer(cmd[1]) || !is_integer(cmd[2]) || !is_integer(cmd[3]))
                throw InvalidInstruction(tmp);
            initializeHash(stoi(cmd[1]), quadraticProbing);
            this->c1 = stoi(cmd[2]);
            this->c2 = stoi(cmd[3]);
            
        }
        else if (cmd[0] == "DOUBLE" && cmd[3] == "")
        {
            if (!is_integer(cmd[1]) || !is_integer(cmd[2])) throw InvalidInstruction(tmp);
            initializeHash(stoi(cmd[1]), doubleHashing);
            this->c1 = stoi(cmd[2]);
            this->c2 = 0;
        }
        else throw InvalidInstruction(tmp);
    }
    while (!file.eof()) {
        getline(file, tmp);
        string cmd[3];
        commandTokenize(tmp, cmd);
        if (cmd[0] == "INSERT") 
        {
            string flag = insert(cmd[1], cmd[2]);
            if (flag == "InvalidInstruction") throw InvalidInstruction(tmp);
            //if (flag == "InvalidDeclaration") throw InvalidDeclaration(tmp);
            //if (flag == "Redeclared") throw Redeclared(tmp);
            if (flag == "Overflow") throw Overflow(tmp);
            cout << flag << endl;
        }
        else if (cmd[0] == "ASSIGN")
        {
            string flag = assign(cmd[1], cmd[2]);
            if (flag == "InvalidInstruction") throw InvalidInstruction(tmp);
            if (flag == "TypeMismatch") throw TypeMismatch(tmp);
            if (flag == "TypeCannotBeInferred") throw TypeCannotBeInferred(tmp);//
            cout << flag << endl;
        }
        else if (cmd[0] == "CALL") 
        {
            cmd[1] += cmd[2];
            string flag = call(cmd[1]);
            if (flag == "InvalidInstruction") throw InvalidInstruction(tmp);
            if (flag == "TypeMismatch") throw TypeMismatch(tmp);
            if (flag == "TypeCannotBeInferred") throw TypeCannotBeInferred(tmp);//
            cout << flag << endl;
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
            int i = 0;
            int slot = lookup(cmd[1], i);
            if (slot == -1) throw Undeclared(cmd[1]);
            cout << slot << endl;
        }
        else if (cmd[0] == "PRINT" && cmd[1] == "" && cmd[2] == "") 
        {
            this->print();
            if (this->count != 0) cout << endl;
        }
        else throw InvalidInstruction(tmp);
    }
    file.close();
    if (this->curr_level != 0) throw UnclosedBlock(this->curr_level);
}


int SymbolTable::lookup(string name, int& i) {
    int level = this->curr_level;
    int pos = -1;
    while (level >= 0) {
        i = 0;
        pos = this->search(name, level, i);
        if (pos != -1) return pos;
        --level;
    }
    return -1;
}

string SymbolTable::insert(string name, string nop) {
    if (!is_identifier_name(name)) return "InvalidInstruction";
    Node newNode;
    // nop == "" means name is not a function
    if (nop == "") {
        int i = 0;
        // if name has already been in st, it is redeclared
        if (search(name, this->curr_level, i) != -1) {
            //cout << "found " << name << " " << this->curr_level << endl;
            throw Redeclared(name);
        } //return "Redeclared";
            
        newNode = Node(name, this->curr_level, false, "");
    }
    // nop is int means name is a function
    else if (is_integer(nop)) {
        int i = 0;
        if (search(name, this->curr_level, i) != -1) //return "Redeclared";
            throw Redeclared(name);
        if (this->curr_level > 0) //return "InvalidDeclaration";
            throw InvalidDeclaration(name);
        newNode = Node(name, this->curr_level, true, nop);
    }
    else return "InvalidInstruction";
    int num_slot = 0;
    int pos = insert(newNode, num_slot);
    if (pos == -1) return "Overflow";
    // add pos to block
    this->curr_block->mem += to_string(pos);
    this->curr_block->mem += " ";
    this->count++;
    //cout << "them " << name << "//" << this->curr_level << " tai: " << pos << endl;
    return to_string(num_slot);
}

void SymbolTable::beginScope() {
    //cout << "BEGIN" << endl;
    Block* newBlock = new Block();
    newBlock->prev = this->curr_block;
    this->curr_block = newBlock;
    //cout << "level before block: " << curr_level << endl;
    this->curr_level++;
    //cout << "level of block: " << curr_level << endl;
}

bool SymbolTable::endScope() {
    if (this->curr_level == 0) return false;
    //-------tokenizing to delete----------
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
            //remove(del, this->curr_level);
            this->status[stoi(del)] = DELETED;
            this->count--;
            start = (int)end + (int)key.size();
            end = (int)str.find(key, start);
        }
        del = str.substr(start, end - start);
        //remove(del, this->curr_level);
        this->status[stoi(del)] = DELETED;
        this->count--;
    }
    //---------delete block-------------------
    Block* temp = curr_block;
    curr_block = curr_block->prev;
    curr_level--;
    delete temp;
    return true;
}

void SymbolTable::print() {
    //string res = "";
    int c = 0;
    for (int i = 0; i < this->size; ++i) {
        if (c == this->count) return;
        if (this->status[i] == NON_EMPTY) {
            if (c != 0) cout << ";";
            cout << i << " " << this->data[i].name << 
            "//" << this->data[i].level;
            ++c;
        }
    }
}

string SymbolTable::call(string str) {
    int l = str.length();
    int found = str.find("(");
    if ((found == -1) || (str[l - 1] != ')')) return "InvalidInstruction";
    string func_name = str.substr(0, found);
    if (!is_identifier_name(func_name)) return "InvalidInstruction";
    int i_f = 0;
    int f = lookup(func_name, i_f);
    if (f == -1) throw Undeclared(func_name);
    //cout << "found " << func_name << " at " << i_f << endl;
    if (data[f].func == false) return "TypeMismatch";

    string value_lis = str.substr(found + 1, (l-2) - (found+1) + 1);
    string type = data[f].encoded_paras;
    // type la encoded cua f()
    // value_lis la phan trong ()
    int lv = value_lis.length();
    int lt = type.length();
    if (lt == 0 && lv != 0) return "TypeMismatch";
    if (lt != 0 && lv == 0) return "TypeMismatch";
    // f() ko tham so, và valu_lis cung ko tham so
    if (lt == 0 && lv == 0) {
        if (data[f].type == "NONE") {
            data[f].type = "void";
        }
        else if (data[f].type == "number" 
            && data[f].type == "string") {
            return "TypeMismatch";
        } //else data[f].type is void
        return to_string(i_f);
    }

    int cc = 1; //cc is the number of value in value_lis
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

    int q;
    int sum_iq = 0;
    for (int i = 0; i < cc; ++i) {
        if (is_number(value[i])) {
            if (type[i] == 's') return "TypeMismatch";
            if (type[i] == ' ') type[i] = 'n';
        }
        else if (is_string(value[i])) {
            if (type[i] == 'n') return "TypeMismatch";
            if (type[i] == ' ') type[i] = 's';
        }
        else if (is_identifier_name(value[i])) {
            int iq = 0;
            q = lookup(value[i], iq);
            if (q == -1) throw Undeclared(value[i]);
            if (data[q].func) return "TypeMismatch";
            if (data[q].type == "number") {
                if (type[i] == 's') return "TypeMismatch";
                if (type[i] == ' ') type[i] = 'n';
            }
            else if (data[q].type == "string") {
                if (type[i] == 'n') return "TypeMismatch";
                if (type[i] == ' ') type[i] = 's';
            }
            else {//data[q].type == "NONE";
                if (type[i] == ' ') return "TypeCannotBeInferred";
                if (type[i] == 'n') data[q].type = "number";
                else if (type[i] == 's') data[q].type = "string";
            }
            sum_iq += iq;
        }
        else return "InvalidInstruction";
    }
    // update data parameter type change
    data[f].encoded_paras = type;
    if (data[f].type == "NONE") {
        data[f].type = "void";
    }
    else if (data[f].type == "number" 
        && data[f].type == "string") {
        return "TypeMismatch";
    } //else data[f].type is void
    //cout << data[f].type << ' ' << data[f].encoded_paras << endl;
    return to_string(i_f + sum_iq);
}

string SymbolTable::assign(string name, string str) {
    if (!is_identifier_name(name)) return "InvalidInstruction";
    int p;
    int ip = 0;
    if (is_identifier_name(str)) {
        int iq = 0;
        int q = lookup(str, iq);
        if (q == -1) throw Undeclared(str);
        if (data[q].func) return "InvalidInstruction";
        p = lookup(name, ip);
        if (p == -1) throw Undeclared(name);
        if (data[p].func) return "TypeMismatch";
        //----checking type------
        if (data[p].type == "NONE" && data[q].type == "NONE") {
            return "TypeCannotBeInferred";
        }
        else if (data[p].type == "NONE") {
            data[p].type = data[q].type;
        }
        else if (data[q].type == "NONE") {
            data[q].type = data[p].type;
        }
        else {
            if (is_type_mismatched(data[p].type, data[q].type))
                return "TypeMismatch";
        }
        data[p].value = data[q].value;
        return to_string(ip + iq);
    }

    if (is_value(str)) {
        p = lookup(name, ip);
        if (p == -1) throw Undeclared(name);
        if (data[p].func) return "TypeMismatch";
        if (data[p].type == "NONE") {
            if (is_string(str)) data[p].type = "string";
            else data[p].type = "number";
        }
        else {
            if (is_type_mismatched(data[p].type, str))
                return "TypeMismatch";
        }
        data[p].value = str;
        return to_string(ip);
    }

    int l = str.length();
    int found = str.find("(");
    if ((found == -1) || (str[l - 1] != ')')) return "InvalidInstruction";
    string func_name = str.substr(0, found);
    if (!is_identifier_name(func_name)) return "InvalidInstruction";
    int i_f = 0;
    int f = lookup(func_name, i_f);
    if (f == -1) throw Undeclared(func_name);
    if (data[f].func == false) return "TypeMismatch";

    string value_lis = str.substr(found + 1, (l-2) - (found+1) + 1);
    string type = data[f].encoded_paras;
    int lv = value_lis.length();
    int lt = type.length();
    if (lt == 0 && lv != 0) return "TypeMismatch";
    if (lt != 0 && lv == 0) return "TypeMismatch";
    if (lt == 0 && lv == 0) {
        p = lookup(name, ip);
        if (p == -1) throw Undeclared(name);
        if (data[p].func) return "TypeMismatch";
        //-----checking type---------
        if (data[p].type == "NONE" && data[f].type == "NONE") {
            return "TypeCannotBeInferred";
        }
        else if (data[f].type == "void") {
            return "TypeMismatch";
        }
        else if (data[p].type == "NONE") {
            data[p].type = data[f].type;
        }
        else if (data[f].type == "NONE") {
            data[f].type = data[p].type;
        }
        else {
            if (is_type_mismatched(data[p].type, data[f].type))
                return "TypeMismatch";
        }
        return to_string(ip + i_f);
    }
    int cc = 1; //cc is the number of value in value_lis
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

    int q;
    int sum_iq = 0;
    for (int i = 0; i < cc; ++i) {
        if (is_number(value[i])) {
            if (type[i] == 's') return "TypeMismatch";
            if (type[i] == ' ') type[i] = 'n';
        }
        else if (is_string(value[i])) {
            if (type[i] == 'n') return "TypeMismatch";
            if (type[i] == ' ') type[i] = 's';
        }
        else if (is_identifier_name(value[i])) {
            int iq = 0;
            q = lookup(value[i], iq);
            if (q == -1) throw Undeclared(value[i]);
            if (data[q].func) return "TypeMismatch";
            if (data[q].type == "number") {
                if (type[i] == 's') return "TypeMismatch";
                if (type[i] == ' ') type[i] = 'n';
            }
            else if (data[q].type == "string") {
                if (type[i] == 'n') return "TypeMismatch";
                if (type[i] == ' ') type[i] = 's';
            }
            else {//data[q].type == "NONE";
                if (type[i] == ' ') return "TypeCannotBeInferred";
                if (type[i] == 'n') data[q].type = "number";
                else if (type[i] == 's') data[q].type = "string";
            }
            sum_iq += iq;
        }
        else return "InvalidInstruction";
    }
    // update data type change
    type = data[f].encoded_paras;
    if (data[f].type == "void") return "TypeMismatch";
    p = lookup(name, ip);
    if (p == -1) throw Undeclared(name);
    if (data[p].func) return "TypeMismatch";
    //-----checking type---------
    if (data[p].type == "NONE" && data[f].type == "NONE") {
        return "TypeCannotBeInferred";
    }
    else if (data[f].type == "void") {
        return "TypeMismatch";
    }
    else if (data[p].type == "NONE") {
        data[p].type = data[f].type;
    }
    else if (data[f].type == "NONE") {
        data[f].type = data[p].type;
    }
    else {
        if (is_type_mismatched(data[p].type, data[f].type))
            return "TypeMismatch";
    }
    return to_string(ip + sum_iq + i_f);
}