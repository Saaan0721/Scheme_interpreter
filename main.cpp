#include <iostream>
#include <string>

using namespace std;

// define const
const int HASH_TABLE_LENGTH = 31;
const int NODE_ARRAY_LENGTH = 31;

// define HashTable
class HashSlot {
public:
    string symbol;
    int value;
};

HashSlot HashTable[HASH_TABLE_LENGTH];

// define NodeArray
class Node {
public:
    Node() { id = 0; left = 0; right = 0; }
    int id;
    int left;
    int right;
};

Node NodeArray[NODE_ARRAY_LENGTH];

string command;

unsigned int StringToInt(string s) {
    int length = (int)s.length();
    unsigned int answer = 0;

    if(length % 2 == 1) {
        answer = s.at(length-1);
        length--;
    }

    for(int i = 0; i < length; i += 2) {
        answer += s.at(i);
        answer += ((int)s.at(i+1)) << 8;
    }

    return answer;
}

// processes input and returns a token
string GetNextToken() {
    char ch;
    string str;

    while(1) {
        // ch = cin.get();
        ch = command[0];
        command.erase(0, 1);

        // make input lowercase
        ch = tolower(ch);

        // at end of line, stop reading
        if(ch == '\0') {
            break;
        }

        // stops when a space is encountered while reading a symbol
        else if(ch == ' ') {
            if(!str.empty()) {
                break;
            }
        }

        // stop reading when encountering parentheses
        else if(ch == '(') {
            if(str.length() > 0) {
                command = ch + command;
                break;
            }
            else {
                str += ch;
                break;
            }
        }

        else if(ch == ')') {
            if(str.length() > 0) {
                // cin.putback(ch);
                command = ch + command;
                break;
            }
            else {
                str += ch;
                break;
            }
        }

        else if(ch == '\'') {
            str += ch;
            break;
        }

        // keep reading
        else {
            str += ch;
            if(str.length() >= 10) {
                break;
            }
        }
    }

    return str;
}

// hash function using StringToInt
int Hash(string k, int i) {
    return (StringToInt(k) + i) % HASH_TABLE_LENGTH;
}

// hash insert
int HashInsert(string k) {
    for(int i = 0; i < HASH_TABLE_LENGTH; i++) {
        int j = Hash(k, i);
        if(HashTable[j].symbol == "") {
            HashTable[j].symbol = k;
            return j;
        }
    }

    return -1;
}

// hash search
int HashSearch(string k) {
    for(int i = 0; i < HASH_TABLE_LENGTH; i++) {
        int j = Hash(k, i);
        if(HashTable[j].symbol == k) {
            return j;
        }
    }

    return -1;
}

// if the input string does not exist in the hash table,
// insert it and return the hash value.
int GetHashValue(string str) {
    int value;

    value = HashSearch(str);
    if(value == -1) {
        value = HashInsert(str);
    }

    return -value;
}

// return length of NodeArray
int length(Node array[]) {
    for(int i = 1; i < NODE_ARRAY_LENGTH; i++) {
        if(array[i].id == 0) {
            return i-1;
        }
    }

    return -1;
}

string Preprocessing() {
    string newCommand, token;

    while(!(token = GetNextToken()).empty()) {
        if(token == "define") {
            newCommand += "define ";
            token = GetNextToken();
            if(token == "(") {
                token = GetNextToken();
                newCommand += (token + " ( lambda ( " + Preprocessing() + ")");
            }
            else {
                newCommand += (token + " ");
            }
        }
        else if(token == "'") {
            newCommand += "( qoute ";
            int numberOfLeftParen = 0;
            do {
                token = GetNextToken();
                newCommand += (token + " ");
                if(token == "(") {
                    numberOfLeftParen++;
                }
                else if(token == ")") {
                    numberOfLeftParen--;
                }
            } while(numberOfLeftParen > 0);
            newCommand += ") ";
        }
        else {
            newCommand += (token + " ");
        }
    }

    return newCommand;
}

// read input
int Read() {
    int root, temp, tokenHashValue;
    bool first = true;
    string newCommand;

    tokenHashValue = GetHashValue(GetNextToken());

    // scheme code starts with '('
    if(tokenHashValue == GetHashValue("(")) {

        // iterate until ')' appears
        while((tokenHashValue = GetHashValue(GetNextToken())) != GetHashValue(")")) {
            int len = length(NodeArray);

            // the first entry of the list is read
            if(first == true) {

                // index 0 is reserved for empty list
                NodeArray[len+1].id = len+1;
                temp = len+1;
                root = temp;
                first = false;
            }

            // the remaining elements in the list should be put into the right
            else {
                NodeArray[temp].right = len + 1;
                temp = NodeArray[temp].right;
                NodeArray[temp].id = temp;
            }
            
            // if the nested list appears, do recursion
            if(tokenHashValue == GetHashValue("(")) {
                // cin.putback('(');
                command = '(' + command;
                NodeArray[temp].left = Read();
            }
            else {
                NodeArray[temp].left = tokenHashValue;
            }

            // the right child of last node is zero
            NodeArray[temp].right = 0;
        }

        return root;
    }
    else {
        return tokenHashValue;
    }
}

// print parse tree
void PRINT(int index, bool startList) {
    if(index == 0) {
        cout << "()";
    }
    else {
        if(startList == true) {
            cout << "(";
        }

        if(NodeArray[index].left < 0) {
            cout << HashTable[-NodeArray[index].left].symbol << " ";
        }
        else if(NodeArray[index].left > 0) {
            PRINT(NodeArray[index].left, true);
        }

        if(NodeArray[index].right != 0) {
            PRINT(NodeArray[index].right, false);
        }
        else { 
            cout << "\b \b";
            cout << ") ";
        }
    }
}

// print hash table
void PrintHashTable() {
    cout << "Hash table = " << endl;
    for(int i = 0; i < HASH_TABLE_LENGTH; i++) {
        if(!HashTable[i].symbol.empty()) {
            cout << -i << ":" << HashTable[i].symbol << endl;
        }
    }
    cout << endl;
}

// print node array
void PrintNodeArray() {
    cout << "Node array = " << endl;
    for(int i = 0; i < NODE_ARRAY_LENGTH; i++) {
        cout << i << ":" << NodeArray[i].left << "," << NodeArray[i].right << endl;
    }
    cout << endl;
}

// free node array
void freeNodeArray() {
    int len = length(NodeArray);
    for(int i = 1; i <= len; i++) {
        NodeArray[i].id = 0;
        NodeArray[i].left = 0;
        NodeArray[i].right = 0;
    }
}


int main()
{
    string newCommand;
    int root;
    while(1) {
        getline(cin, command);
        newCommand = Preprocessing();
        command = newCommand;

        root = Read();

        if(root != 0) {
            cout << "Free list's root = " << length(NodeArray)+1 << endl;
            cout << "Parse tree's root = " << root << endl;
            cout << endl;
            PrintNodeArray();
            PrintHashTable();
            PRINT(root, true);
            cout << endl;

            // freeNodeArray();
        }
    }
    
    return 0;
}
