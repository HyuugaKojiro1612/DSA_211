#include "SymbolTable.h"
#include "SymbolTable.cpp"
using namespace std;

// #include <stdio.h>
// #include <stdlib.h>
// #include <crtdbg.h>

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





int main() {
    string filename = "testcase.txt";
    // SymbolTable *st = new SymbolTable;
    // st->run(filename);

    test(filename);
    
    cout << endl;
    system("pause");
    return 0;
}


