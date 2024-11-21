//
//  main.cpp
//  CS5800-RBTree-Impl
//
//  Created by Logan Gill on 11/11/24.
//

#include <iostream>
#include <string>
#include <fstream>
#include <cstring>

using namespace std;

enum Color {
    RED,
    BLACK
};

class Nodes {
public:
    Nodes(int k, Nodes* n): key(k), color(RED), p(n), left(n), right(n)
    {}
    Nodes(int k, Nodes* n, Color c): key(k), color(c), p(n), left(n), right(n)
    {}
    int key;
    Color color;
    Nodes* p;
    Nodes* left;
    Nodes* right;
    
    virtual bool isSentinal() = 0;
    virtual int height() = 0;
};

class Sentinel: public Nodes {
public:
    Sentinel(): Nodes(0, this, BLACK)
    {}
    
    bool isSentinal() override {
        return true;
    }
    
    int height() override {
        return 0;
    }
};

class Node: public Nodes {
public:
    Node(int k, Sentinel* s): Nodes(k, s)
    {}
    
    bool isSentinal() override {
        return false;
    }
    
    int height() override {
        return 1 + max(left->height(), right->height());
    }
};


class RedBlackTree {
private:
    Nodes* root;
    
public:
    RedBlackTree() {
        root = new Sentinel();
    }
    
    int height() {
        return root->height();
    }
    
    /// Prints out a sorted version of the tree as a list
    void sort() {
        Nodes* currNode = min(root);
        
        cout << "| ";
        while (currNode && !currNode->isSentinal()) {
            cout << currNode->key << "(" << currNode->p->key << ")" << " | ";
            currNode = successor(currNode);
        }
        cout << endl;
    }
    
    void display() {
        vector<Nodes*> toPrint;
        toPrint.push_back(root);
        
        if (dynamic_cast<Sentinel*>(root)) {
            cout << "Just a sentianl" << endl;
            return;
        }
        
        Nodes* lastPar = root->p;
        while (!toPrint.empty()) {
            Nodes* currNode = toPrint[0];
            toPrint.erase(toPrint.begin());
            if (lastPar != currNode->p) {
                lastPar = currNode->p;
                cout << endl;
            }
            cout << currNode->key << "(" << currNode->p->key << "," << (currNode->color == RED ? "Red" : "Black") << ")";
            
            if (!dynamic_cast<Sentinel*>(currNode->left))
                toPrint.push_back(currNode->left);
            if (!dynamic_cast<Sentinel*>(currNode->right))
                toPrint.push_back(currNode->right);
        }
    }
    
    /// Gets the root of the tree
    Nodes* getRoot() {
        return root;
    }
    
    /// Searches for a node with the given value
    Nodes* search(int val) {
        Nodes* currNode = root;
        
        while (currNode && !currNode->isSentinal() && currNode->key != val) { // TODO: Change if sentinal
            if (val < currNode->key) {
                currNode = currNode->left;
                continue;
            }
            currNode = currNode->right;
        }
        
        if (currNode->isSentinal()) {
            string errorMsg = "Value ";
            errorMsg += to_string(val);
            errorMsg += " is not in the tree";
            throw invalid_argument(errorMsg);
        }
        return currNode;
    }
    
    /// Gets the min value from the given value node
    Nodes* min(Nodes* currNode) {
        //Nodes* currNode = search(val);
        
        if (!currNode || currNode->isSentinal()) { // TODO: Change if sentinal
            // Throw error
            throw invalid_argument("Value does not exsist in the tree");
        }
        
        // !static_cast<Sentinal*>(currNode->left)
        while (currNode->left && !currNode->left->isSentinal()) { // TODO: Change if sentinal
            currNode = currNode->left;
        }
        
        return currNode;
    }
    
    /// Gets the max value from the given value node
    Nodes* max(Nodes* currNode) {
        //Nodes* currNode = search(val);
        
        if (!currNode || currNode->isSentinal()) { // TODO: Change if sentinal
            // Throw error
            throw invalid_argument("Value does not exsist in the tree");
        }
        
        // !static_cast<Sentinal*>(currNode->right)
        while (currNode->right && !currNode->right->isSentinal()) { // TODO: Change if sentinal
            currNode = currNode->right;
        }
        
        return currNode;
    }
    
    /// Gets the node with smallest key > than val
    Nodes* successor(Nodes* currNode) {
        //Nodes* currNode = search(val);
        
        if (currNode->right && !currNode->right->isSentinal()) { // TODO: Change if sentinal
            return min(currNode->right);
        }
        
        Nodes* currPar = currNode->p;
        while (currPar && !currPar->isSentinal() && currNode == currPar->right) { // TODO: Change if sentinal
            currNode = currPar;
            currPar = currPar->p;
        }
        
        return currPar;
    }
    
    /// Gets the node with largest key < val
    Nodes* predecessor(Nodes* currNode) {
        //Nodes* currNode = search(val);
        
        if (currNode->left && !currNode->left->isSentinal()) { // TODO: Change if sentinal
            return max(currNode->left);
        }
        
        Nodes* currPar = currNode->p;
        while (currPar && !currPar->isSentinal() && currNode == currPar->left) { // TODO: Change if sentinal
            currNode = currPar;
            currPar = currPar->p;
        }
        
        return currPar;
    }
    
    // RB exclusive
    /// Inserts the given node into the tree, then fixing the tree as needed
    void insert(int val) {
        Nodes* currNode = root;
        Nodes* parNode = nullptr;
        Nodes* newNode = new Node(val, dynamic_cast<Sentinel*>(root->p));
        
        while (currNode && !currNode->isSentinal()) {
            parNode = currNode;
            
            if (val < currNode->key)
                currNode = currNode->left;
            else
                currNode = currNode->right;
        }
        
        newNode->p = parNode;
        if (!parNode || parNode->isSentinal()) {
            newNode->p = root;
            root = newNode;
            newNode->color = BLACK;
            return;
        } else if (newNode->key < parNode->key)
            parNode->left = newNode;
        else
            parNode->right = newNode;
                
        // Fix insert
        fixInsert(newNode);
    }
    
    /// Fixes the tree after insert
    void fixInsert(Nodes* currNode) {
        while (currNode->p->color == RED) {
            if (currNode->p == currNode->p->p->left) {
                Nodes* uncle = currNode->p->p->right;
                
                if (uncle->color == RED) {
                    currNode->p->color = BLACK;
                    uncle->color = BLACK;
                    currNode->p->p->color = RED;
                    currNode = currNode->p->p;
                } else {
                    if (currNode == currNode->p->right) {
                        currNode = currNode->p;
                        rotateLeft(currNode);
                    }
                    
                    currNode->p->color = BLACK;
                    currNode->p->p->color = RED;
                    rotateRight(currNode->p->p);
                }
            } else {
                Nodes* uncle = currNode->p->p->left;
                
                if (uncle->color == RED) {
                    currNode->p->color = BLACK;
                    uncle->color = BLACK;
                    currNode->p->p->color = RED;
                    currNode = currNode->p->p;
                } else {
                    if (currNode == currNode->p->left) {
                        currNode = currNode->p;
                        rotateRight(currNode);
                    }
                    
                    currNode->p->color = BLACK;
                    currNode->p->p->color = RED;
                    rotateLeft(currNode->p->p);
                }
            }
        }
        root->color = BLACK;
    }
    
    /// Removes the given node from the tree
    void remove(Nodes* currNode) {
        //Nodes* currNode = search(val);
        Nodes* fixNode = nullptr;
        
        Color startColor = currNode->color;
        
        // If there is no node with the give value
        if (!currNode || currNode->isSentinal()) {
            throw invalid_argument("Value does not exsist in the tree");
        }
        
        if (!currNode->left || currNode->left->isSentinal()) {
            fixNode = currNode->right;
            transplant(currNode, fixNode);
        } else if (!currNode->right || currNode->right->isSentinal()) {
            fixNode = currNode->left;
            transplant(currNode, fixNode);
        } else {
            Nodes* suc = min(currNode);
            startColor = suc->color;
            fixNode = suc->right;
            
            if (suc != currNode->right) {
                transplant(suc, suc->right);
                suc->right = currNode->right;
                suc->right->p = suc;
            } else {
                currNode->p = suc;
            }
            
            // Transplant
            transplant(currNode, suc);
            suc->left = currNode->left;
            currNode->left->p = suc;
            suc->color = currNode->color;
        }
        
        if (startColor == BLACK) {
            // Delete fix
        }
    }
    
    void transplant(Nodes* goneNode, Nodes* newNode) {
        if (goneNode->p->isSentinal())
            root = newNode;
        else if (goneNode->p->left == goneNode)
            goneNode->p->left = newNode;
        else
            goneNode->p->right = newNode;
        newNode->p = goneNode->p;
    }
    
    //void rotation();
    
    /// Rotates the tree left from the par node
    void rotateLeft(Nodes* par) {
        Nodes* rChild = par->right;
        
        // Update par's right child to rChild's left
        par->right = rChild->left;
        if (rChild->left && !rChild->left->isSentinal())
            rChild->left->p = par;
        
        rChild->p = par->p;
        if (par->p->isSentinal())
            root = rChild;
        if (par->p->left == par)
            par->p->left = rChild;
        else if (par->p->right == par)
            par->p->right = rChild;
        
        rChild->left = par;
        par->p = rChild;
    }
    
    /// Rotates the tree right from the par node
    void rotateRight(Nodes* par) {
        Nodes* lChild = par->left;
        
        // Update par's left child to rChild's right
        par->left = lChild->right;
        if (lChild->right && !lChild->right->isSentinal())
            lChild->right->p = par;
        
        lChild->p = par->p;
        if (par->p->isSentinal())
            root = lChild;
        if (par->p->left == par)
            par->p->left = lChild;
        else if (par->p->right == par)
            par->p->right = lChild;
        
        lChild->right = par;
        par->p = lChild;
    }
};

int main(int argc, const char * argv[]) {
    RedBlackTree rbt = RedBlackTree();
    
    string fileName = "ArrInput";
    ifstream inputFile;
    inputFile.open(fileName + ".txt");
    string line;
    
    if (!inputFile) {
        cout << "Cannot find file, continuing without file input" << endl;
    }
    
    /*
    // For each line in the file
    while (getline(inputFile, line)) {
        char cstr[line.length()];
        strcpy(cstr, line.c_str());
        char* word = strtok(cstr, " ,");
        
        // For each word in the line
        while (word) {
            int num = stoi(word);
            cout << num << endl;
            rbt.insert(num);
            word = strtok(nullptr, " ,");
        }
    }
     */
    
    
    // Command prompts
    while (true) {
        cout << "Enter a command: ";
        string command;
        cin >> command;
        
        if (cin.fail()) {
            cout << "Error reading command. Please try again" << endl;
            continue;
        }
        
        if (command == "end") {
            cout << "Promt closed" << endl;
            break;
        } else if (command == "search") {
            int num;
            cin >> num;
            if (num) {
                try {
                    Nodes* n = rbt.search(num);
                    cout << "Found " << n->key;
                } catch (const invalid_argument &e) {
                    cout << e.what();
                }
            }
        } else if (command == "insert") {
            int num;
            cin >> num;
            if (num) {
                rbt.insert(num);
                try {
                    Nodes* n = rbt.search(num);
                    cout << "Inserted " << n->key;
                } catch (const invalid_argument &e) {
                    cout << "A falure occured during insert operation";
                }
            }
        } else if (command == "delete") {
            int num;
            cin >> num;
            if (num) {
                try {
                    Nodes* sNode = rbt.search(num);
                    rbt.remove(sNode);
                    cout << "Removed " << num;
                } catch (const invalid_argument &e) {
                    cout << e.what();
                }
            }
        } else if (command == "max") {
            int num;
            cin >> num;
            if (num) {
                Nodes* sNode = rbt.search(num);
                Nodes* m = rbt.max(sNode);
                cout << "Largest from " << num << " is " << m->key;
            }
        } else if (command == "min") {
            int num;
            cin >> num;
            if (num) {
                Nodes* sNode = rbt.search(num);
                Nodes* m = rbt.min(sNode);
                cout << "Smallest from " << num << " is " << m->key;
            }
        } else if (command == "successor") {
            int num;
            cin >> num;
            if (num) {
                Nodes* sNode = rbt.search(num);
                Nodes* m = rbt.successor(sNode);
                cout << "Successor of " << num << " is " << m->key;
            }
        } else if (command == "predecessor") {
            int num;
            cin >> num;
            if (num) {
                Nodes* sNode = rbt.search(num);
                Nodes* m = rbt.predecessor(sNode);
                cout << "Predecessor of " << num << " is " << m->key;
            }
        } else if (command == "sort") {
            rbt.sort();
        } else if (command == "root") {
            cout << "Root: " << rbt.getRoot()->key;
        } else if (command == "height") {
            cout << "Height: " << rbt.height() << endl;
            continue;
        } else if (command == "display") {
            rbt.display();
        } else {
            cout << "Invalid command" << endl;
            continue;
        }
        cout << endl << "Height: " << rbt.height() << endl;
    }
    
    // TODO: read from file, testing
    return 0;
}
