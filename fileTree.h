#ifndef FILETREE_H
#define FILETREE_H

#include <iostream>
#include <fstream>
#include <string.h>
#include <io.h>
#include <vector>

using namespace std;

bool isHuf(uint8_t* message);
// jump to next file's name
void jumpPos(ifstream &input);

struct fileNode {
    string name;
    fileNode* left = nullptr;
    fileNode* right = nullptr;
};

class fileTree {
private:
    fileNode* Root = nullptr;
public:
    // create a binary file tree
    void createTree(string &inHuf);
    // add node to the file tree
    void addNode(fileNode *par, fileNode *add, bool where);
    // print the tree in preorder
    void printTree();
    // recursive
    void preOrder(fileNode*, int level);
    // delete the tree
    void deleteTree();
    void deleteNode(fileNode*);
};

#endif  // FILETREE_H