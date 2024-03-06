#ifndef HUFFTREE_H
#define HUFFTREE_H

#include <filesystem>
#include <iostream>
#include <fstream>
#include <queue>
#include <cstdint>
#include <io.h>

using namespace std;
using namespace std::filesystem;

// write in verification and call func 'compressFile' for each file
void compress(string &inFilePath, string &outHuf);
// call functions in class huffTree to compress one file
void compressFile(directory_entry inEntry, ofstream &output);

// judge the file type and decompress .huf
void decompress(string &inHuf);
// return true when the input file is .huf
bool isHuf(uint8_t* message);

// nodes in a huffman tree
struct huffNode {
    unsigned char character;
    long long frequency;  // the file may be larger than 4G
    huffNode* left = nullptr;
    huffNode* right = nullptr;
    short INDEX;    // = 'i' of nodes[i] or = num + 'i' of branches[i]
    string code;
    // functor, to deal with priority queue
    bool operator() (huffNode* x, huffNode* y);
};

class huffTree {
private:
    int NodeNum = 0;
    huffNode* Nodes = nullptr;  // all leaf nodes
    huffNode* Branches = nullptr;   // all branch nodes
    huffNode* Root = nullptr;   // the root

public:
    int getNodeNum();

    // get frequency of different characters from file
    // create leaf nodes
    void freqCount(string &inFilePath);

    // build a huffman tree based on nodes gotten previously
    // create branch nodes
    void createTree();

    // encode every node in the huffman tree (from root to the end)
    // if only one character in the file, encode as 1 for better recognization
    void encode();

    // store the huffman tree and compressed text, release space at last
    void compressOne(string &inFilePath, ofstream &output);
    // store the huffman tree
    void storeTree(ofstream &output);
    // store the compressed text
    void storeCtx(string &inFilePath, ofstream &output, long long pos);

    // rebuild in a huffman tree and decompress the text
    // deal with file of different size (maybe empty), different node number (only 1)
    void decompressOne(ifstream &input, string &outFilePath);
    // rebuild the huffman tree from .huf
    void readTree(ifstream &input, int nodeNum);
    // decode .huf
    void readCtx(ifstream &input, ofstream &output, long long ctxSize);

};

#endif  // HUFFTREE_H