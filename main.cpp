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

// stack for evaluation of user defined function
stack<HashSlot> s;

// convert string to int for hash function
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

// hash function using StringToInt
int Hash(string k, int i) {
    return (StringToInt(k) + i) % HASH_TABLE_LENGTH + 1;
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
        else if(HashTable[j].symbol == "") {
            return -1;
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

// processes input and returns a token
string GetNextToken() {
    char ch;
    string str;

    while(1) {
        // read the first character
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
                command = ch + command;
                break;
            }
            else {
                str += ch;
                break;
            }
        }

        // stop reading when encountering quote
        else if(ch == '\'') {
            str += ch;
            break;
        }

        // ignore indentation
        else if(ch == '\t') {
            continue;
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

// return length of NodeArray
int length(Node array[]) {
    for(int i = 1; i < NODE_ARRAY_LENGTH; i++) {
        if(array[i].id == 0) {
            return i-1;
        }
    }

    return -1;
}

// preprocess command(lambda, quote)
string Preprocessing() {
    string newCommand, token;

    // until the end of command
    while(!(token = GetNextToken()).empty()) {

        // if token is define
        if(token == "define") {
            newCommand += "define ";
            token = GetNextToken();

            // if function define, insert lambda
            if(token == "(") {
                token = GetNextToken();
                newCommand += (token + " ( lambda ( " + Preprocessing() + ")");
            }

            // if variable define, do nothing
            else {
                newCommand += (token + " ");
            }
        }

        // if token is quote
        else if(token == "'") {

            // insert quote
            newCommand += "( quote ";
            int numberOfLeftParen = 0;

            // read until the close of parentheses
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


// check whether the value corresponding to a given
// index in a hash table is a number
bool isNumber(int tokenIndex) {
    string symbol = HashTable[-tokenIndex].symbol;

    // check each digit
    for(int i = 0; i < symbol.length(); i++) {
        if(!isdigit(symbol[i])) {
            return false;
        }
    }

    return true;
}

// check whether it is user defined function
bool isUserDefinedFunction(int root) {
    if(root < 0) {
        return false;
    }
    int value = HashTable[-NodeArray[root].left].value;

    return value > 0;
}

// evaluate
int Eval(int root) {
    int result, tokenIndex;

    tokenIndex = NodeArray[root].left;

    // if token is "+"
    if(tokenIndex == GetHashValue("+")) {
        int result = GetHashValue(to_string(stof(HashTable[-Eval(NodeArray[NodeArray[root].right].left)].symbol)
                                        + stof(HashTable[-Eval(NodeArray[NodeArray[NodeArray[root].right].right].left)].symbol)));
        return result;
    }

    // if token is "-"
    else if(tokenIndex == GetHashValue("-")) {
        int result = GetHashValue(to_string(stof(HashTable[-Eval(NodeArray[NodeArray[root].right].left)].symbol)
                                        - stof(HashTable[-Eval(NodeArray[NodeArray[NodeArray[root].right].right].left)].symbol)));
        return result;
    }

    // if token is "*"
    else if(tokenIndex == GetHashValue("*")) {
        int result = GetHashValue(to_string(stof(HashTable[-Eval(NodeArray[NodeArray[root].right].left)].symbol)
                                        * stof(HashTable[-Eval(NodeArray[NodeArray[NodeArray[root].right].right].left)].symbol)));
        return result;
    }

    // if token is "/"
    else if(tokenIndex == GetHashValue("/")) {
        int result = GetHashValue(to_string(stof(HashTable[-Eval(NodeArray[NodeArray[root].right].left)].symbol)
                                        / stof(HashTable[-Eval(NodeArray[NodeArray[NodeArray[root].right].right].left)].symbol)));
        return result;
    }

    // if token is "number?"
    else if(tokenIndex == GetHashValue("number?")) {
        if(isNumber(Eval(NodeArray[NodeArray[root].right].left))) {
            return GetHashValue("#t");
        }
        else {
            return GetHashValue("#f");
        }
    }

    // if token is "symbol?"
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

    // if token is "null?"
    else if(tokenIndex == GetHashValue("null?")) {
        try {
            int result = HashTable[-Eval(NodeArray[NodeArray[root].right].left)].value;
            if(!result) {
                return GetHashValue("#t");
            }
            return GetHashValue("#f");
        }
        catch(const exception& e) {
            return GetHashValue("#t");
        }
    }

    // if token is "cons"
    else if(tokenIndex == GetHashValue("cons")) {

        // allocate new memory and return it
        int len = length(NodeArray);
        NodeArray[len+1].id = len+1;
        NodeArray[len+1].left = Eval(NodeArray[NodeArray[root].right].left);
        NodeArray[len+1].right = Eval(NodeArray[NodeArray[NodeArray[root].right].right].left);
        return len+1;
    }

    // if token is "cond"
    else if(tokenIndex == GetHashValue("cond")) {

        // check each condition and return expression if the condition is true
        while(NodeArray[NodeArray[root].right].right) {
            root = NodeArray[root].right;
            if(Eval(NodeArray[NodeArray[root].left].left) == GetHashValue("#t")) {
                return Eval(NodeArray[NodeArray[NodeArray[root].left].right].left);
            }
        }

        // if "else" is omitted, raise error
        if(NodeArray[NodeArray[NodeArray[root].right].left].left != GetHashValue("else")) {
            cout << "error: else omitted" << endl;
            return 0;
        }

        // return the expression of "else"
        return Eval(NodeArray[NodeArray[NodeArray[NodeArray[root].right].left].right].left);
    }

    // if token is "car"
    else if(tokenIndex == GetHashValue("car")) {
        int result = NodeArray[Eval(NodeArray[NodeArray[root].right].left)].left;
        return result;
    }

    // if token is "cdr"
    else if(tokenIndex == GetHashValue("cdr")) {
        int result = NodeArray[Eval(NodeArray[NodeArray[root].right].left)].right;
        return result;
    }

    // if token is "define"
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

    // if token is "quote"
    else if(tokenIndex == GetHashValue("quote")) {
        int result = NodeArray[NodeArray[root].right].left;
        return result;
    }

    // if token is user defined function
    else if(isUserDefinedFunction(root)) {
        HashSlot slot;
        int result, param, arg, lambda, eval, a, b;

        lambda = HashTable[-NodeArray[root].left].value;

        // set the first parameter and argument
        param = NodeArray[NodeArray[lambda].right].left;
        arg = NodeArray[root].right;

        // until the end of parameter
        do {
            // push current values to stack
            slot = HashTable[-NodeArray[param].left];
            s.push(slot);

            // set parameters by argument
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
        
        // if root is already evaluated, return its value
        if(result) {
            return result;
        }

        // otherwise, just return root
        else {
            return root;
        }
    }

    return root;
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

// print value
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

// count the characters in string
int count(char ch, string str) {
    int count = 0;
    for(char c: str) {
        if(c == ch) {
            count++;
        }
    }

    return count;
}


int main()
{
    string newCommand, rowCommand;
    int root, result, openCount, closeCount;
    while(1) {
        // get input until opening parentheses == closing parentheses
        command.clear(); 
        A:
        cout << ">>> ";
        
        getline(cin, rowCommand);
        command += (" " + rowCommand);

        openCount = count('(', command);
        closeCount = count(')', command);

        if(openCount > closeCount) {
            goto A;
        }
        else if(openCount < closeCount) {
            cout << "error: there are more closing parentheses than opening parentheses" << endl;
            command.clear();
            goto A;
        }

        newCommand = Preprocessing();
        command = newCommand;

        root = Read();
        result = Eval(root);

        if(result != 0) {
            // cout << "Free list's root = " << length(NodeArray)+1 << endl;
            // cout << "Parse tree's root = " << root << endl;
            // cout << endl;

            PrintNodeArray();
            PrintHashTable();

            if(result >= 0) {
                if(NodeArray[root].left != GetHashValue("define")) {
                    PRINT(result, true);
                    cout << endl;
                }
            }
            else {
                PRINT_VALUE(result);
            }

            // freeNodeArray();
        }
    }
    
    return 0;
}
