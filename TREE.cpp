#include "fileTree.h"

int main(int argc, char *argv[]) {
    if(argc != 2) {
        cout << "Wrong input." << endl;
        cout << "e.g. ./preview xxx.huf" << endl;
        return 0;
    }
    char* in = argv[1];

    if(_access(in, 0) == -1) {
        cout << "File doesn't exist." << endl;
        return 0;
    }
    string inHuf = in;
    fileTree ft;
    ft.createTree(inHuf);
    ft.printTree();
    ft.deleteTree();
    return 0;
}