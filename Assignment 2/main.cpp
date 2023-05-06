#include "SymbolTable.h"
#include "SymbolTable.cpp"
using namespace std;

void test(string filename)
{
    SymbolTable *st = new SymbolTable();
    try
    {
        st->run(filename);
    }
    catch (exception &e)
    {
        cout << e.what();
    }
    delete st;
}
// void validSubmittedFiles(string filename, string *allowedIncludingFiles, int numOfAllowedIncludingFiles = 1)
// {
//     ifstream infile(filename);
//     string line;
//     while (getline(infile, line))
//     {
//         if (line.find("#include") != string::npos)
//         {
//             for (int i = 0; i < numOfAllowedIncludingFiles; i++)
//             {
//                 if (line.find(allowedIncludingFiles[i]) != string::npos)
//                     continue;
//                 cout << "Use disallowed file: " + line << endl;
//                 exit(1);
//             }
//         }
//     }
//     infile.close();
// }
int main() {
    string filename = "testcase.txt";
    test(filename);

    // string s1 = "b1";
    // string s2 = "b2";
    // string e = "";
    // Node* b1 = new Node(s1, "number", 0, 1, e, 0);
    // Node* b2 = new Node(s2, "string", 0, 0, e, 0);
    // if (b1 < b2) cout << "b1 be b2";

    // SymbolTable s;
    // s.root = new Node();
    // s.root->name = "p";
    // s.root->level = 3;

    // Node* t1 = new Node();
    // t1->name = "p"; t1->level = 4;
    // t1->parent = s.root; s.root->left = t1;

    // Node* d2 = new Node();
    // d2->name = "d"; d2->level = 2;
    // d2->parent = s.root; s.root->right = d2;

    // Node* p1 = new Node();
    // p1->name = "p"; p1->level = 1;
    // p1->parent = t1; t1->left = p1;

    // Node* x1 = new Node();
    // x1->name = "x"; x1->level = 1;
    // x1->parent = t1; t1->right = x1;

    // Node* b2 = new Node();
    // b2->name = "b"; b2->level = 2;
    // b2->parent = d2; d2->left = b2;

    // Node* p2 = new Node();
    // p2->name = "p"; p2->level = 2;
    // p2->parent = d2; d2->right = p2;
    // s.preOrder(s.root);

    // string name = "k";
    // Node* p = s.lookup(s.root, name);
    // if (p != nullptr)
    // cout << p->level; else cout << "None";
    cout << endl;
    system("pause");
    return 0;
}