#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Node {
public:
    string name, type, value;
    int level;

    bool function;
    string encoded_type_lis;

    Node* parent;
    Node* left;
    Node* right;

    Node() {
        this->parent = nullptr;
        this->left = nullptr;
        this->right = nullptr;
    }

    int compare(Node* obj) {
        if (obj == nullptr) return 1;
        if (this->level > obj->level) return 1;
        if (this->level < obj->level) return -1;
        if (this->name.compare(obj->name) > 0) return 1;
        if (this->name.compare(obj->name) < 0) return -1;
        return 0;
    }

    int compare(string name, int level) {
        if (this->level > level) return 1;
        if (this->level < level) return -1;
        if (this->name.compare(name) > 0) return 1;
        if (this->name.compare(name) < 0) return -1;
        return 0;
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

class SymbolTable {
public:
    Node* root;
    int curr_level;

    Block* curr_block; //head
    Block* global_block; //tail

    SymbolTable() {
        this->root = nullptr;
        this->curr_level = 0;

        Block* newBlock = new Block();
        this->curr_block = newBlock;
        this->global_block = newBlock;
    }
    ~SymbolTable() {
        this->blockClear();
        this->treeClear();
    }
    void preOrder(Node* ptr) {
        if (ptr == nullptr) return;
        cout << ptr->name << ptr->level << ' ';
        preOrder(ptr->left);
        preOrder(ptr->right);
    }
    void blockClear() {
        if (curr_block == nullptr) return;
        Block* p = curr_block;
        while (curr_block) {
            p = curr_block;
            curr_block = curr_block->prev;
            delete p;
        }
    }
    void treeClear() {
        treeDel(this->root);
        this->root = nullptr;
    }
    void treeDel(Node* node) {
        if (node == nullptr) return;
        treeDel(node->left);
        treeDel(node->right);
        delete node;
    }
protected:
    void zig(Node* x) {
        Node* p = x->parent;
        if (p->left == x) {
            Node* branch = x->right;
            x->parent = nullptr;
            x->right = p;

            p->parent = x;
            p->left = branch;
            if (branch != nullptr) branch->parent = p;
        }
        else {
            Node* branch = x->left;
            x->parent = nullptr;
            x->left = p;

            p->parent = x;
            p->right = branch;
            if (branch != nullptr) branch->parent = p;
        }
        this->root = x;
    }

    void zig_zig(Node* x) {
        Node* p = x->parent;
        Node* g = p->parent;
        if (p->left == x) {
            Node* branch_x = x->right;
            Node* branch_p = p->right;

            x->parent = g->parent;
            x->right = p;

            p->parent = x;
            p->left = branch_x;
            p->right = g;

            g->parent = p;
            g->left = branch_p;

            if (x->parent != nullptr) {
                if (x->parent->left == g) x->parent->left = x;
                else x->parent->right = x;
            } else this->root = x;
            if (branch_x != nullptr) branch_x->parent = p;
            if (branch_p != nullptr) branch_p->parent = g;
        }
        else {
            Node* branch_p = p->left;
            Node* branch_x = x->left;

            x->parent = g->parent;
            x->left = p;

            p->parent = x;
            p->left = g;
            p->right = branch_x;

            g->parent = p;
            g->right = branch_p;

            if (x->parent != nullptr) {
                if (x->parent->left == g) x->parent->left = x;
                else x->parent->right = x;
            } else this->root = x;
            if (branch_x != nullptr) branch_x->parent = p;
            if (branch_p != nullptr) branch_p->parent = g;
        }
    }

    void zig_zag(Node* x) {
        Node* p = x->parent;
        Node* g = p->parent;
        if (p->right == x) {
            Node* left_branch = x->left;
            Node* right_branch = x->right;

            x->parent = g->parent;
            x->left = p;
            x->right = g;

            p->parent = x;
            p->right = left_branch;

            g->parent = x;
            g->left = right_branch;

            if (x->parent != nullptr) {
                if (x->parent->left == g) x->parent->left = x;
                else x->parent->right = x;
            } else this->root = x;
            if (left_branch != nullptr) left_branch->parent = p;
            if (right_branch != nullptr) right_branch->parent = g;
        }
        else {
            Node* left_branch = x->left;
            Node* right_branch = x->right;

            x->parent = g->parent;
            x->left = g;
            x->right = p;

            p->parent = x;
            p->left = right_branch;

            g->parent = x;
            g->right = left_branch;

            if (x->parent != nullptr) {
                if (x->parent->left == g) x->parent->left = x;
                else x->parent->right = x;
            } else this->root = x;
            if (left_branch != nullptr) left_branch->parent = g;
            if (right_branch != nullptr) right_branch->parent = p;
        }
    }

    Node* max_descendant(Node* sub_root) {
        Node* curr = sub_root;
        while (curr->right != nullptr) {
            curr = curr->right;
        }
        return curr;
    }

    Node* min_descendant(Node* sub_root) {
        Node* curr = sub_root;
        while (curr->left != nullptr) {
            curr = curr->left;
        }
        return curr;
    }

    void splay(Node* x) {
        Node* p;
        Node* g;
        while (x->parent != nullptr) {
            p = x->parent;
            g = p->parent;
            if (g == nullptr) {
                zig(x);
            }
            else if (g->left == p && p->left == x) {
                zig_zig(x);
            }
            else if (g->right == p && p->right == x) {
                zig_zig(x);
            }
            else {
                zig_zag(x);
            }
        }
        this->root = x;
    }

    Node* search(string name, int level, int& num_comp, int& num_splay) {
        Node* res = nullptr;
        Node* curr = this->root;
        Node* prev = nullptr;
        while (curr != nullptr) {
            prev = curr;
            if (curr->compare(name, level) > 0) {
                ++num_comp;
                curr = curr->left;
            }
            else if (curr->compare(name, level) < 0) {
                ++num_comp;
                curr = curr->right;
            }
            else { //p == curr
                ++num_comp;
                res = curr;
                break;
            }
        }
        if (num_comp == 1) num_splay = 0;
        else num_splay = 1;
        if (res != nullptr) splay(res);
        else if (prev != nullptr) splay(prev);
        return res;
    }

    bool insert(Node* newNode, int& num_comp, int& num_splay) {
        if (this->root == nullptr) {
            this->root = newNode;
            num_comp = 0;
            num_splay = 0;
            return true;
        }
        num_splay = 1;
        Node* curr = this->root;
        while (curr != nullptr) {
            if (newNode->compare(curr) < 0) {
                ++num_comp;
                if (curr->left == nullptr) {
                    curr->left = newNode;
                    newNode->parent = curr;
                    splay(newNode);
                    return true;
                }
                else curr = curr->left;
            }

            else if (newNode->compare(curr) > 0) {
                ++ num_comp;
                if (curr->right == nullptr) {
                    curr->right = newNode;
                    newNode->parent = curr;
                    splay(newNode);
                    return true;
                }
                else curr = curr->right;
            }

            else {
                return false;
            }
        }
        return true;
    }

    bool remove(string name, int level) {
        int a = 0, b = 0;
        Node* del = search(name, level, a, b);
        if (del == nullptr) {
            return false;
        }
        Node* leftSubTree = del->left;
        Node* rightSubTree = del->right;
        if (del->left) {
            del->left->parent = nullptr;
            del->left = nullptr;
        }
        if (del->right) {
            del->right->parent = nullptr;
            del->right = nullptr;
        }
        if (leftSubTree == nullptr && rightSubTree == nullptr) {
            this->root = nullptr;
        }
        else if (leftSubTree == nullptr) {
            this->root = rightSubTree;
        }
        else if (rightSubTree == nullptr) {
            this->root = leftSubTree;
        }
        else {
            Node* newRoot = max_descendant(leftSubTree);
            splay(newRoot);
            newRoot->right = rightSubTree;
            rightSubTree->parent = newRoot;
        }
        delete del;
        return true;
    }

public:
    void run(string filename);
    string insert(string name, string type, string _static);
    string assign(string name, string value);
    void beginScope();
    bool endScope();
    Node* lookup(string name, int& num_comp, int& num_splay);
    void print(Node* p, string& res);
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

bool is_static(string _static) {
    return (_static == "true" || _static == "false");
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

void commandTokenize(string str, string* res, string key = " ")
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
