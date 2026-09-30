#include <iostream>
#include <string>
#include<fstream>
using namespace std;

// ey j si inu {funccall} ich istemaal kiya jaandeya
struct NestedCall {
    bool isTernary;
    string plainName;      // used if !isTernary
    bool condition;        // used if isTernary 
    string ternaryTrue;    // used if isTernary
    string ternaryFalse;   // used if isTernary
};

// One function definition, e.g. funA(3){funB,(true?funC:funD)} 5
struct FunctionDef {
    string name;
    string recursionExpr;
    NestedCall* nestedCalls;    //jo udday ich nestedcall si uda array
    int numNested;
    int requestedMemory;
};

// Hik call stack di entery
struct Frame {
    string funcName;
    int allocatedSize;   // allocated mem != raw mem (round up to the smallest mult of 4 > raw mem)
};

struct DefCollection {  //where all the function definations live
    FunctionDef* data;  //dynamic array
    int capacity;
    int count;
};

DefCollection makeCollection() {    //concstructor hi smjh lo (not really)
    DefCollection c;
    c.capacity = 8;
    c.count = 0;
    c.data = new FunctionDef[c.capacity];
    return c;
}

void addDef(DefCollection& c, FunctionDef def) {    //same logic
    if (c.count == c.capacity) {
        c.capacity *= 2;
        FunctionDef* newData = new FunctionDef[c.capacity];
        for (int i = 0; i < c.count; i++) newData[i] = c.data[i];
        delete[] c.data;
        c.data = newData;
    }
    c.data[c.count] = def;
    c.count++;
}

class CallStack {   //specifically used for enteries to be called
private:
    Frame* data;
    int capacity;
    int count;

    void resize() {
        capacity *= 2;
        Frame* newData = new Frame[capacity];
        for (int i = 0; i < count; i++) newData[i] = data[i];
        delete[] data;
        data = newData;
    }

public:
    CallStack() {
        capacity = 8;
        count = 0;
        data = new Frame[capacity];
    }

    void push(Frame f) {
        if (count == capacity) resize();
        data[count] = f;
        count++;
    }

    void pop() {
        if (count > 0) count--;
        else cout << "underflow";
    }

    Frame& top() {
        return data[count - 1];
    }

    bool isEmpty() {
        return count == 0;
    }

    int size() {
        return count;
    }

    Frame& at(int i) {   // for printing the whole stack 
        return data[i];
    }
};

class chstk {   //for inf -> post
private:
    char* data;
    int capacity;
    int count;

    void resize() {
        capacity *= 2;
        char* newData = new char[capacity];
        for (int i = 0; i < count; i++) newData[i] = data[i];
        delete[] data;
        data = newData;
    }

public:
    chstk() {
        capacity = 8;
        count = 0;
        data = new char[capacity];
    }

    void push(char f) {
        if (count == capacity) resize();
        data[count] = f;
        count++;
    }

    void pop() {
        if (count > 0) count--;
        else cout << "underflow";
    }

    char& top() {
        return data[count - 1];
    }

    bool isEmpty() {
        return count == 0;
    }

    int size() {
        return count;
    }
};


class intstk {  //for post -> pre
private:
    int* data;
    int capacity;
    int count;

    void resize() {
        capacity *= 2;
        int* newData = new int[capacity];
        for (int i = 0; i < count; i++) newData[i] = data[i];
        delete[] data;
        data = newData;
    }

public:
    intstk() {
        capacity = 8;
        count = 0;
        data = new int[capacity];
    }

    void push(int f) {
        if (count == capacity) resize();
        data[count] = f;
        count++;
    }

    void pop() {
        if (count > 0) count--;
        else cout << "underflow";
    }

    int& top() {
        return data[count - 1];
    }

    bool isEmpty() {
        return count == 0;
    }

    int size() {
        return count;
    }
};

int prec(char c)
{
    if (c == '/') return 5;
    else if (c == '*') return 4;
    else if (c == '+') return 3;
    else if (c == '-') return 2;
    return 0;
}

string infToPost(string ex)     //12+ 3/2*4
{                     //expected: 12 3 2 / 4 * + 
    string res;     //dry run: 12 3 2 / 4 * +
    chstk Op;
    int length = ex.length();
    for (int i = 0; i < length; i++)
    {

        if (prec(ex[i]))
        {
            res += ' ';
            while (!Op.isEmpty() && prec(Op.top()) >= prec(ex[i]))
            {

                res += Op.top();
                res += ' ';
                Op.pop();
            }
            Op.push(ex[i]);
        }
        else if (ex[i] == ')')
        {
            while (!Op.isEmpty() && Op.top() != '(')
            {
                res += ' ';
                res += Op.top();
                Op.pop();
            }
            Op.pop();
        }
        else if (ex[i] == '(')
        {
            Op.push(ex[i]);
        }
        else
        {
            if (ex[i] != ' ')
                res += ex[i];
        }
    }
    while (!Op.isEmpty())
    {
        res += ' ';
        res += Op.top();
        Op.pop();
    }


    return res;
}

int postfixEvaluator(string s)
{
    intstk stk;
    int size = s.length();
    int num = 0;
    bool in = false;

    for (int i = 0; i < size; i++)
    {
        if (s[i] >= '0' && s[i] <= '9')
        {
            num = num * 10 + (s[i] - '0');
            in = 1;
        }
        else
        {
            if (in)
            {
                stk.push(num);
                num = 0;
                in = false;
            }
            if (s[i] == ' ') continue;

            int a = stk.top(); stk.pop();
            int b = stk.top(); stk.pop();

            if (s[i] == '*') stk.push(b * a);
            else if (s[i] == '-') stk.push(b - a);
            else if (s[i] == '+') stk.push(b + a);
            else if (s[i] == '/') stk.push(b / a);
        }
    }
    if (in) stk.push(num);
    return stk.top();
}

FunctionDef parseDefinitionLine(string line) {
    FunctionDef def;

    // 1. ida naam before '('
    int i = 0;
    while (line[i] != '(') i++;
    def.name = line.substr(0, i);

    // 2. recursion number inside ()'
    int start = i + 1;
    while (line[i] != '{') i++;
    def.recursionExpr = line.substr(start, i - start - 1);  // could be empty string

    // 3. nested calls inside { ... }
    i++; // skip '{'
    start = i;
    while (line[i] != '}') i++;
    string body = line.substr(start, i - start); // e.g. "funB,(true?funC:funD)"
    i++; // skip '}'

    int funcs = 0;
    int size = body.length();
    for (int j = 0; j < size; j++)
    {
        if (body[j] == ',') funcs++;
    }
    if (!body.empty()) funcs++;
    NestedCall* ptr = new NestedCall[funcs];
    int c = 0;
    for (int j = 0; j < size; j++)
    {
        if (body[j] == '(')
        {   //checking till ? , getting true false;
            j++;
            int st = j;
            ptr[c].isTernary = 1;
            while (body[j] != '?') j++;
            string tf = body.substr(st, j - st);
            ptr[c].condition = tf == "true";

            j++;//skipping ?

            st = j; //now the true func
            while (body[j] != ':') j++;
            string left = body.substr(st, j - st);
            ptr[c].ternaryTrue = left;

            j++; // skipping :

            st = j; //now the flase func
            while (body[j] != ')')j++;
            string right = body.substr(st, j - st);
            ptr[c].ternaryFalse = right;
            ptr[c].plainName = "";
            c++;
            j++;
            continue;
        }
        else
        {
            ptr[c].isTernary = 0;
            int st = j;
            while (j < size && body[j] != ',') j++;
            string name = body.substr(st, j - st);
            ptr[c].plainName = name;
            c++;
            continue;
        }


    }
    def.nestedCalls = ptr;
    def.numNested = funcs;
    // 4. requested memory = remaining number after '}'
    string rest = line.substr(i);
    def.requestedMemory = stoi(rest); // stoi skips leading spaces automatically

    return def;
}


FunctionDef* findDef(DefCollection& c, string name) {   //finding a function, (used in nested calls)
    for (int i = 0; i < c.count; i++) {
        if (c.data[i].name == name) return &c.data[i];
    }
    return nullptr;
}


bool isAlpha(char c) {  //is alphabet helper function
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isAlnum(char c) {  //is alphanumerical helper function
    return isAlpha(c) || (c >= '0' && c <= '9');    // alpha + number = holy recursion 💀
}

bool isValidName(string name)
{
    if (name.empty()) return false;
    if (!isAlpha(name[0])) return false;

    for (int i = 1; i < (int)name.length(); i++) {
        if (!isAlnum(name[i])) return false;
    }
    return true;
}

bool hasDuplicate(DefCollection& c, string name)
{
    return findDef(c, name) != nullptr;
}

DefCollection readDefinitions(int N, int M, ifstream  fin)
{
    DefCollection defs = makeCollection();
    for (int k = 0; k < N; k++) {
        string line;
        getline(fin, line);
        FunctionDef def = parseDefinitionLine(line);

        if (!isValidName(def.name)) {
            cout << "Error: Invalid function name " << def.name << endl;
            continue;
        }
        if (hasDuplicate(defs, def.name)) {
            cout << "Error: Duplicate definition of " << def.name << endl;
            continue;
        }
        addDef(defs, def);
    }

    return defs;
}

void checkUndefined(DefCollection& defs) {
    for (int i = 0; i < defs.count; i++) {
        FunctionDef& def = defs.data[i];
        for (int j = 0; j < def.numNested; j++) {
            NestedCall& nc = def.nestedCalls[j];
            if (nc.isTernary) {
                if (findDef(defs, nc.ternaryTrue) == nullptr)
                    cout << "Error: Undefined function " << nc.ternaryTrue << " called by " << def.name << endl;
                if (findDef(defs, nc.ternaryFalse) == nullptr)
                    cout << "Error: Undefined function " << nc.ternaryFalse << " called by " << def.name << endl;
            }
            else {
                if (findDef(defs, nc.plainName) == nullptr)
                    cout << "Error: Undefined function " << nc.plainName << " called by " << def.name << endl;
            }
        }
    }
}

int main()
{
    int N, M, S;
    ifstream fin("input.txt");
    int testCaseNum = 1;
    while (fin >> N >> M >> S) {
        fin.ignore();


        cout << "========== Test Case " << testCaseNum << " ==========" << endl;

        DefCollection defs = makeCollection();
        for (int k = 0; k < N; k++) {
            string line;
            getline(fin, line);
            // skip blank lines if any slip in
            if (line.empty()) { k--; continue; }
            FunctionDef def = parseDefinitionLine(line);
        }

        string* topLevelCalls = new string[M];
        for (int k = 0; k < M; k++) {
            getline(fin, topLevelCalls[k]);
        }

        // ... run checkUndefined, cycle detection, execution, etc. on defs/topLevelCalls here ...

        // now consume the "###" line (or hit EOF if this was the last test case)
        string sep;
        getline(fin, sep);
        if (sep != "###") return 0;
        // if sep isn't "###", you're either at EOF or something's misformatted

        testCaseNum++;
        delete[] topLevelCalls;
        // also free defs.data and every def's nestedCalls array once you're done with this test case,
        // since "state must not leak" includes memory, not just logical values
    }

}