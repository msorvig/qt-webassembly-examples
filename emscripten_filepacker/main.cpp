
#include <stdio.h>
#include <iostream>
#include <fstream>

#include <emscripten.h>

using namespace std;

int main() {
    
    cout << "main()" << endl;
    
    string line;
    ifstream myfile ("assets/data1.txt");
    if (myfile.is_open())
    {
       while (getline(myfile,line))
       {
         cout << line << '\n';
       }
       myfile.close();
     }
    
    return 0;
}
