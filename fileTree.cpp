#include "fileTree.h"

bool isHuf(uint8_t* message) {
    uint8_t ideal[8] = {0, 255, 'h', 'u', 'f'};
    for(int i = 0; i < 5; i ++) {
        if(message[i] != ideal[i]) return false;
    }
    if(message[5] > 1) return false;
    return true;
}

void jumpPos(ifstream &input) {
    uint8_t myRead[128];
    int nodeNum;
    long long jmp = 0;
    input.read(reinterpret_cast<char*>(myRead), 8);
    long long ctxSize = *(uint64_t*)myRead;
    if(ctxSize == 0){
        return;
    }
    input.read(reinterpret_cast<char*>(myRead), 3);
    nodeNum = (myRead[0] << 8) + myRead[1];
    jmp = nodeNum + 4 * (nodeNum - 1);
    jmp += ctxSize;
    input.seekg(jmp, ios::cur);
}

// ugly but no effort to rewrite this
void fileTree::createTree(string &inHuf) {
    ifstream input(inHuf, ios::in | ios::binary);
    string fPath;
    uint8_t flag[2];
    uint8_t message[8];
    char name[257];
    input.read(reinterpret_cast<char*>(message), 6);
    input.seekg(-1, ios::cur);
    if(!isHuf(message)) {
        cout << "This is not a .huf file, so can't preview it." << endl;
        input.close();
        return;
    }
    while(input.read(reinterpret_cast<char*>(flag), 2)) {
        input.read(name, flag[1]);
        name[flag[1]] = '\0';
        fPath = name;
        fileNode* p = this->Root, *par = this->Root;
        bool where = 0;

        char* res = strtok(name, "\\");        
        while(res) {
            if(this->Root == nullptr) {
                fileNode* node = new fileNode;
                node->name = res;
                this->Root = node;
                p = node->left;
                par = node;
                res = strtok(nullptr, "\\");
                if(!res) {
                    continue;
                }
            }
            if(p == nullptr) {
                fileNode* node = new fileNode;
                node->name = res;
                addNode(par, node, where);
                where = 0;
                p = node->left;
                par = node;
                res = strtok(nullptr, "\\");             
            } else {
                if(p->name == res) {
                    par = p;
                    p = p->left;
                    where = 0;
                    res = strtok(nullptr, "\\");
                } else {
                    par = p;
                    p = p->right;
                    where = 1;
                }                
            }            
        }

        if(flag[0]) {
            jumpPos(input);
        }
    }
    input.close();
}

void fileTree::addNode(fileNode *par, fileNode *add, bool where) {
    if(!where) {
        par->left = add;
    } else if(where) {
        par->right = add;
    }
}

void fileTree::printTree() {
    preOrder(this->Root, 0);
}
void fileTree::preOrder(fileNode* node, int level) {
    static bool flag[256] = {0};
    if(node) {
        if(!node->right) {
            flag[level - 1] = 1;
        }
        for(int i = 0; i < level - 1; i ++) {
            if(flag[i] == 0) {
                cout << "│ ";
            } else {
                cout << "  ";
            }
        }
        if(level) {
            if(node->right) {
                cout << "├─";
                flag[level - 1] = 0;
            } else {
                cout << "└─";
            }
        }
        cout << node->name << endl;
        preOrder(node->left, level + 1);
        preOrder(node->right, level);
    }
}

void fileTree::deleteTree() {
    this->deleteNode(this->Root);
}
void fileTree::deleteNode(fileNode* in) {
    if(in == nullptr) return;
    if(in->left != nullptr) {
        deleteNode(in->left);
    }
    if(in->right != nullptr) {
        deleteNode(in->right);
    }
    delete in;
}