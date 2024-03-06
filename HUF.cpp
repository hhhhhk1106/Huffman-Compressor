#include "huffTree.h"

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cout << "Wrong input." << endl;
        cout << "e.g. ./huf xxx.huf 1.txt" << endl;
        return 0;
    }
    char* out = argv[1];
    char* in = argv[2];
    if(_access(in, 0) == -1) {
        cout << "File doesn't exist." << endl;
        return 0;
    }
    if(_access(out, 0) == 0) {
        cout << "The file \"" << out << "\" already exists." << endl;
        cout << "Please input [0] if you want to stop compressing," << endl;
        cout << "       input [1] if you want to overwrite it." << endl;
        char choice;
        cin >> choice;
        while(choice != '0' && choice != '1') {
            cout << "Wrong input." << endl;
            cin >> choice;
        }
        if(choice == '0') {
            cout << "Already stop." << endl;
            return 0;
        }
    }
    string outFile = out;
    string inFile = in;
    compress(inFile, outFile);
    cout << "Compression completed." << endl;
    return 0;
}