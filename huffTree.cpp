#include "huffTree.h"

bool huffNode::operator() (huffNode* x, huffNode* y) {
    if(x->frequency == y->frequency) {
            return x->character > y->character;
        }
        return x->frequency > y->frequency;
}

int huffTree::getNodeNum() {
    return this->NodeNum;
}

void huffTree::freqCount(string &inFilePath) {
    long long freq[256] = {0};  // record the number of occurrences of every charaacter
    char ch;
    unsigned int temp;
    huffNode* nodes;
    ifstream input;
    input.open(inFilePath, ios::in | ios::binary);

    // count
    while(input.get(ch)) {
        temp = (int)(unsigned char)ch;
        freq[temp] ++;
    }
    // get nodeNum
    for(int i = 0; i < 256; i ++) {
		if(freq[i]) {
			this->NodeNum ++;
		}
	}
    input.close();
    // build leaf nodes
    nodes = new huffNode[this->NodeNum];
    for(int i = 0, index = 0; i < 256; i ++) {
        if(freq[i]) {
            nodes[index].character = i;
            nodes[index].frequency = freq[i];
            nodes[index].INDEX = index;
            index ++;
        }
    }
    this->Nodes = nodes;
}

void huffTree::createTree() {
    // cases : no branch nodes
    if(this->NodeNum <= 1) {
        this->Root = &(this->Nodes[0]);
        return;
    }

    int index = 0;
    huffNode* branches = new huffNode[this->NodeNum - 1];
    priority_queue<huffNode*, vector<huffNode*>, huffNode> pq;
    for(int i = 0; i < this->NodeNum; i ++) {
        pq.push(&(this->Nodes[i]));
    }
    //huffNode left, right;
    huffNode *p1, *p2;
    while(pq.size() > 1) {
        // get first two and build a branch node
        p1 = pq.top();
        pq.pop();
        p2 = pq.top();
        pq.pop();
        branches[index].frequency = p1->frequency + p2->frequency;
        branches[index].character = p1->character;
        branches[index].left = p1;
        branches[index].right = p2;
        branches[index].INDEX = this->NodeNum + index;
        huffNode* temp;
        temp = &branches[index];
        pq.push(temp);
        index ++;
    }
    this->Root = pq.top();
    pq.pop();
    this->Branches = branches;
}

void huffTree::encode() {
    // case : only one character
    if(this->NodeNum == 1){
        this->Root->code = "1";
        return;
    }

    queue<huffNode*> todo;
    this->Root->code = "";
    todo.push(this->Root);

    while(!todo.empty()) {
        huffNode* temp = todo.front();
        todo.pop();
        huffNode* lChild = temp->left;
        if(lChild != nullptr) {
            string parentCode = temp->code;
            lChild->code = parentCode + '0';
            todo.push(lChild);
            huffNode* rChild = temp->right;
            rChild->code = parentCode + '1';
            todo.push(rChild);
        }        
    }
}

void huffTree::compressOne(string &inFilePath, ofstream &output) {
    long long pos = output.tellp();
    unsigned char myWrite[16];
    myWrite[0] = this->NodeNum >> 8;
    myWrite[1] = this->NodeNum & 255;
    myWrite[2] = 0; // reserve a place for toByte
    output.write(reinterpret_cast<char*>(myWrite), 3);

    // store tree
    this->storeTree(output);
    // store compressed text
    this->storeCtx(inFilePath, output, pos);

    delete [](this->Nodes);
    delete [](this->Branches);
}
void huffTree::storeTree(ofstream &output) {
    uint8_t myWrite[256];
    // leaf nodes
    for(int i = 0; i < this->NodeNum; i ++) {
        myWrite[i] = this->Nodes[i].character;
    }
    output.write(reinterpret_cast<char*>(myWrite), this->NodeNum);
    // branch nodes
    for(int i = 0; i < this->NodeNum - 1; i ++) {
        short indexL = this->Branches[i].left->INDEX;
        short indexR = this->Branches[i].right->INDEX;
        myWrite[0] = indexL >> 8;
        myWrite[1] = indexL & 255;
        myWrite[2] = indexR >> 8;
        myWrite[3] = indexR & 255;
        output.write(reinterpret_cast<char*>(myWrite), 4);
    }
}
void huffTree::storeCtx(string &inFilePath, ofstream &output, long long pos) {
    uint8_t myWrite[256];
    uint8_t toByte = 0;
    ifstream input;
    input.open(inFilePath, ios::in | ios::binary);
    char ch;
    unsigned int temp;
    int count = 0;
    unsigned char myByte = 0;
    int inx[256];   // correspondence between char and index(->code)
    for(int i = 0; i < this->NodeNum; i ++) {
        inx[this->Nodes[i].character] = i;
    }
    while(input.get(ch)) {
        temp = (int)(unsigned char)ch;
        string oneCode = this->Nodes[inx[temp]].code;
        int size = oneCode.length();
        for(int i = 0; i < size; i ++) {
            myByte = myByte << 1;
            myByte += (oneCode[i] - '0');
            count ++;
            if(count == 8) {
                myWrite[0] = myByte;
                output.write(reinterpret_cast<char*>(myWrite), 1);
                count = 0;
                myByte = 0;
            }
        }
    }
    // write in toByte
    if(count) {
        toByte = 8 - count;
        myByte = myByte << toByte;
        myWrite[0] = myByte;
        output.write(reinterpret_cast<char*>(myWrite), 1);    
        output.seekp(pos + 2, ios::beg);
        myWrite[0] = toByte;
        output.write(reinterpret_cast<char*>(myWrite), 1);
        output.seekp(0, ios::end);
    }
    input.close();
}

void compress(string &inFilePath, string &outHuf) {
    ofstream output(outHuf, ios::out | ios::binary);
    path inPath(inFilePath);
    uint8_t flag[8] = {0};
    uint8_t message[8] = {0, 255, 'h', 'u', 'f'};   // make sure it .huf
    output.write(reinterpret_cast<char*>(message), 5);

    directory_entry inEntry(inPath);
    // only compress one file
    if(!is_directory(inPath)) {
        compressFile(inEntry, output);
        output.close();
        return;
    } else {
        flag[0] = 0;
        flag[1] = inFilePath.length();
        output.write(reinterpret_cast<char*>(flag), 2);
        output << inFilePath;
    }

    recursive_directory_iterator recList(inPath);
    for(auto& it:recList) {
        string fPath = it.path().string();
        // store directory
        if(is_directory(fPath)) {
            uint8_t nameLen = fPath.length();
            flag[0] = 0;
            flag[1] = nameLen;
            output.write(reinterpret_cast<char*>(flag), 2);
            output << fPath;
        } else {
            // store file
            compressFile(it, output);
            output.seekp(0, ios::end);
        }
        
    }    
    output.close();
}
void compressFile(directory_entry inEntry, ofstream &output) {
    uint8_t flag[8] = {0};
    string fPath = inEntry.path().string();
    flag[0] = 1;
    flag[1] = fPath.length();
    output.write(reinterpret_cast<char*>(flag), 2);
    output << fPath;

    // here : file size, store a long long
    long long ctxSize = 0, storeEnd = 0;
    long long storeBeg = output.tellp();

    *(uint64_t*)flag = 0;
    output.write(reinterpret_cast<char*>(flag), 8);
    if(inEntry.file_size() == 0) {
        return;
    }
    huffTree oneTree;
    oneTree.freqCount(fPath);
    oneTree.createTree();
    oneTree.encode();
    oneTree.compressOne(fPath, output);            
    int num = oneTree.getNodeNum();
    storeEnd = output.tellp();
    ctxSize = storeEnd - storeBeg - 11 - num - 4 * (num - 1);
    output.seekp(storeBeg, ios::beg);
    *(uint64_t*)flag = ctxSize;
    output.write(reinterpret_cast<char*>(flag), 8);
}

void huffTree::decompressOne(ifstream &input, string &outFilePath) {
    ofstream output;
    output.open(outFilePath, ios::out | ios::binary);
    int nodeNum, toByte;
    unsigned char myRead[256];
    input.read(reinterpret_cast<char*>(myRead), 8);
    long long ctxSize = *(uint64_t*)myRead;

    // if empty
    if(ctxSize == 0){
        output.close();
        return;
    }

    input.read(reinterpret_cast<char*>(myRead), 3);
    nodeNum = (myRead[0] << 8) + myRead[1];
    this->NodeNum = nodeNum;
    toByte = myRead[2];
    // if only one node
    if(nodeNum == 1) {
        input.read(reinterpret_cast<char*>(myRead), 1);
        for(long long i = 0; i < ctxSize * 8 - toByte; i ++) {
            output.write(reinterpret_cast<char*>(myRead), 1);
        }
        for(long long i = 0; i < ctxSize; i ++) {
            input.read(reinterpret_cast<char*>(myRead), 1);
        }
        output.close();
        return;
    }

    this->Nodes = new huffNode[nodeNum];
    this->Branches = new huffNode[nodeNum - 1];
    // read tree
    this->readTree(input, nodeNum);

    // read text
    ctxSize = ctxSize * 8 - toByte; // !
    this->readCtx(input, output, ctxSize);

    delete [](this->Nodes);
    delete [](this->Branches);
    output.close();
}
void huffTree::readTree(ifstream &input, int nodeNum) {
    uint8_t myRead[256];
    input.read(reinterpret_cast<char*>(myRead), nodeNum);
    for(int i = 0; i < nodeNum; i ++) {
        this->Nodes[i].character = myRead[i];
        this->Nodes[i].INDEX = 1; // show this is a character
    }
    for(int i = 0; i < nodeNum - 1; i ++) {
        short indexL, indexR;
        input.read(reinterpret_cast<char*>(myRead), 4);
        indexL = (myRead[0] << 8) + myRead[1];
        indexR = (myRead[2] << 8) + myRead[3];
        this->Branches[i].INDEX = 0;
        this->Branches[i].left = (indexL < nodeNum ? &(this->Nodes)[indexL] : &(this->Branches)[indexL - nodeNum]);
        this->Branches[i].right = (indexR < nodeNum ? &(this->Nodes)[indexR] : &(this->Branches)[indexR - nodeNum]);
    }
}
void huffTree::readCtx(ifstream &input, ofstream &output, long long ctxSize) {
    uint8_t myRead[256];
    input.read(reinterpret_cast<char*>(myRead), 1);
    int oneByte = myRead[0];
    int index = 0, flag = 0;
    this->Root = &(this->Branches[this->NodeNum - 2]);
    huffNode* now = this->Root;
    for(long long i = 0; i < ctxSize; i ++) {
        if(index == 8) {
            index = 0;
            input.read(reinterpret_cast<char*>(myRead), 1);
            oneByte = myRead[0];
        }
        flag = (oneByte >> (7 - index)) & 1;
        if(flag) {
            now = now->right;
        } else {
            now = now->left;
        }
        if(now->INDEX) {
            unsigned char temp[4];
            temp[0] = now->character;
            output.write(reinterpret_cast<char*>(temp),1);
            now = this->Root;
        }
        index ++;
    }
}

void decompress(string &inHuf) {
    ifstream input(inHuf, ios::in | ios::binary);
    string fPath;
    uint8_t flag[2];
    uint8_t message[8];
    char name[257];
    input.read(reinterpret_cast<char*>(message), 6);
    input.seekg(-1, ios::cur);
    if(!isHuf(message)) {
        cout << "This is not a .huf file, so can't decompress it." << endl;
        input.close();
        return;
    }
    bool dFlag = 1;
    while(input.read(reinterpret_cast<char*>(flag), 2)) {   
        input.read(name, flag[1]);
        name[flag[1]] = '\0';
        fPath = name;
        if(_access(name, 0) == 0 && dFlag) {
            dFlag = 0;
            cout << "The file \"" << name << "\" already exists." << endl;
            cout << "Please input [0] if you want to stop decompressing," << endl;
            cout << "       input [1] if you want to overwrite it." << endl;
            char choice;
            cin >> choice;
            while(choice != '0' && choice != '1') {
                cout << "Wrong input." << endl;
                cin >> choice;
            }
            if(choice == '0') {
                cout << "Already stop." << endl;
                input.close();
                return;
            }
        }
        if(flag[0] == 0) {
            create_directories(fPath);
        } else {
            huffTree deTree;
            deTree.decompressOne(input, fPath);
        }      
    }
    input.close();
}
bool isHuf(uint8_t* message) {
    uint8_t ideal[8] = {0, 255, 'h', 'u', 'f'};
    for(int i = 0; i < 5; i ++) {
        if(message[i] != ideal[i]) return false;
    }
    if(message[5] > 1) return false;
    return true;
}

