#include <iostream>
#include <string>
#include <cctype>
#include <stack>

using namespace std;

// define const
const int HASH_TABLE_LENGTH = 101;
const int NODE_ARRAY_LENGTH = 101;

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
            newCommand += "( quote ";
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
    int root=0, temp, tokenHashValue;
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

bool isNumber(int tokenIndex) {
    int result;
    string symbol = HashTable[-tokenIndex].symbol;

    for(int i = 0; i < symbol.length(); i++) {
        if(!isdigit(symbol[i])) {
            return false;
        }
    }

    return true;
}

int functionDefinition(int tokenIndex) {
    for(int i = 1; i < tokenIndex; i++) {
        if(NodeArray[i].left == NodeArray[tokenIndex].left) {
            return i;
        }
    }
    return tokenIndex;
}

bool isUserDefinedFunction(int root) {
    if(root < 0) {
        return false;
    }
    int value = HashTable[-NodeArray[root].left].value;

    return value > 0;
}

int Eval(int root) {
    int result, tokenIndex;

    tokenIndex = NodeArray[root].left;
    if(tokenIndex == GetHashValue("+")) {
        int result = GetHashValue(to_string(stof(HashTable[-Eval(NodeArray[NodeArray[root].right].left)].symbol)
                                        + stof(HashTable[-Eval(NodeArray[NodeArray[NodeArray[root].right].right].left)].symbol)));
        return result;
    }

    else if(tokenIndex == GetHashValue("-")) {
        int result = GetHashValue(to_string(stof(HashTable[-Eval(NodeArray[NodeArray[root].right].left)].symbol)
                                        - stof(HashTable[-Eval(NodeArray[NodeArray[NodeArray[root].right].right].left)].symbol)));
        return result;
    }

    else if(tokenIndex == GetHashValue("*")) {
        int result = GetHashValue(to_string(stof(HashTable[-Eval(NodeArray[NodeArray[root].right].left)].symbol)
                                        * stof(HashTable[-Eval(NodeArray[NodeArray[NodeArray[root].right].right].left)].symbol)));
        return result;
    }

    else if(tokenIndex == GetHashValue("/")) {
        int result = GetHashValue(to_string(stof(HashTable[-Eval(NodeArray[NodeArray[root].right].left)].symbol)
                                        / stof(HashTable[-Eval(NodeArray[NodeArray[NodeArray[root].right].right].left)].symbol)));
        return result;
    }

    else if(tokenIndex == GetHashValue("number?")) {
        if(isNumber(Eval(NodeArray[NodeArray[root].right].left))) {
            return GetHashValue("#t");
        }
        else {
            return GetHashValue("#f");
        }
    }

    else if(tokenIndex == GetHashValue("symbol?")) {
        try {
            if(!isNumber(Eval(NodeArray[NodeArray[root].right].left))) {
                return GetHashValue("#t");
            }
            else {
                return GetHashValue("#f");
            }
        }
        catch(const exception& e) {
            return GetHashValue("#f");
        }
    }

    else if(tokenIndex == GetHashValue("null?")) {
        try {
            int result = Eval(NodeArray[NodeArray[root].right].left);
            if(!result) {
                return GetHashValue("#t");
            }
            return GetHashValue("#f");
        }
        catch(const exception& e) {
            return GetHashValue("#t");
        }
    }

    else if(tokenIndex == GetHashValue("cons")) {
        int len = length(NodeArray);
        NodeArray[len+1].left = Eval(NodeArray[NodeArray[root].right].left);
        NodeArray[len+1].right = Eval(NodeArray[NodeArray[NodeArray[root].right].right].left);
        return len+1;
    }

    else if(tokenIndex == GetHashValue("cond")) {
        while(NodeArray[NodeArray[root].right].right) {
            root = NodeArray[root].right;
            if(Eval(NodeArray[NodeArray[root].left].left) == GetHashValue("#t")) {
                return Eval(NodeArray[NodeArray[root].left].right);
            }
        }

        if(NodeArray[NodeArray[NodeArray[root].right].left].left != GetHashValue("else")) {
            cout << "error: else omitted" << endl;
        }
        return Eval(NodeArray[NodeArray[NodeArray[NodeArray[root].right].left].right].left);
    }

    else if(tokenIndex == GetHashValue("car")) {
        int result = NodeArray[Eval(NodeArray[NodeArray[root].right].left)].left;
        return result;
    }

    else if(tokenIndex == GetHashValue("cdr")) {
        int result = NodeArray[Eval(NodeArray[NodeArray[root].right].left)].right;
        return result;
    }

    else if(tokenIndex == GetHashValue("define")) {
        // if function define
        if(NodeArray[NodeArray[NodeArray[NodeArray[root].right].right].left].left
                == GetHashValue("lambda")) {
            HashTable[-NodeArray[NodeArray[root].right].left].value
                = Eval(NodeArray[NodeArray[NodeArray[root].right].right].left);
        }

        // if variable define
        else {
            HashTable[-NodeArray[NodeArray[root].right].left].value
                = Eval(NodeArray[NodeArray[NodeArray[root].right].right].left);
        }
    }

    else if(tokenIndex == GetHashValue("quote")) {
        int result = NodeArray[NodeArray[root].right].left;
        return result;
    }

    else if(isUserDefinedFunction(root)) {
        stack<HashSlot> s;
        HashSlot slot;
        int result, param, arg, lambda, eval, a, b;

        lambda = HashTable[-NodeArray[root].left].value;

        param = NodeArray[NodeArray[lambda].right].left;
        arg = NodeArray[root].right;
        do {
            // push current values to stack
            slot = HashTable[-NodeArray[param].left];
            s.push(slot);

            // set parameters by function argument
            HashTable[-NodeArray[param].left].value = Eval(NodeArray[arg].left);

            // move to next parameter and argument
            param = NodeArray[param].right;
            arg = NodeArray[arg].right;
        } while(param != 0);


        // evaluate
        eval = Eval(NodeArray[NodeArray[NodeArray[lambda].right].right].left);

        // pop the values from stack
        param = NodeArray[NodeArray[lambda].right].left;
        while(!s.empty()) {
            slot = s.top();
            HashTable[-NodeArray[param].left].value = slot.value;

            s.pop();
        }

        return eval;
    }

    else if(root < 0) {
        int result = HashTable[-root].value;
        if(result != 0) {
            return result;
        }
    }

    result = root;

    return result;
}

// print parse tree
void PRINT(int index, bool startList) {
    if(index == 0) {
        cout << "()";
    }
    else if(index > 0) {
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

void PRINT_VALUE(int index) {
    if(index < 0) {
        cout << HashTable[-index].symbol << endl;
    }
}

// print hash table
void PrintHashTable() {
    cout << "Hash table = " << endl;
    for(int i = 0; i < HASH_TABLE_LENGTH; i++) {
        if(!HashTable[i].symbol.empty()) {
            cout << -i << ":" << HashTable[i].symbol << "," << HashTable[i].value << endl;
        }
    }
    cout << endl;
}

// print node array
void PrintNodeArray() {
    cout << "Node array = " << endl;
    for(int i = 0; i < NODE_ARRAY_LENGTH; i++) {
        if(NodeArray[i].left) {
            cout << i << ":" << NodeArray[i].left << " ";
            if(NodeArray[i].left < 0) {
                   cout << "[" << HashTable[-NodeArray[i].left].symbol << "] ";
            }
            cout << "," << NodeArray[i].right << endl;
        }
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
    int root, result;
    while(1) {
        getline(cin, command);
        newCommand = Preprocessing();
        command = newCommand;

        root = Read();
        result = Eval(root);

        if(root != 0) {
            cout << "Free list's root = " << length(NodeArray)+1 << endl;
            cout << "Parse tree's root = " << result << endl;
            cout << endl;
            PrintNodeArray();
            PrintHashTable();
            if(result >= 0) {
                PRINT(result, true);
            }
            else {
                PRINT_VALUE(result);
            }
            cout << endl;

            // freeNodeArray();
        }
    }
    
    return 0;
}
