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
    if (c == '*' || c == '/') return 2;
    if (c == '+' || c == '-') return 1;
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


int evalRecursionCount(string expr)
{
    if (expr.empty()) return 1;
    return postfixEvaluator(infToPost(expr));
}

FunctionDef parseDefinitionLine(string line) {
    FunctionDef def;
    int len = line.length();

    // 1. ida naam before '('
    int i = 0;
    while (i < len && line[i] != '(') i++;
    if (i >= len) { cout << "Malformed definition line: [" << line << "]" << endl; def.name = ""; def.numNested = 0; def.nestedCalls = nullptr; def.requestedMemory = 0; return def; }
    def.name = line.substr(0, i);

    // 2. recursion number inside ()'
    int start = i + 1;
    while (line[i] != '{') i++;
    if (i < len && i >= len) { cout << "Malformed definition line: [" << line << "]" << endl; def.name = ""; def.numNested = 0; def.nestedCalls = nullptr; def.requestedMemory = 0; return def; }
    def.recursionExpr = line.substr(start, i - start - 1);  // could be empty string

    // 3. nested calls inside { ... }
    i++; // skip '{'
    start = i;
    while (i < len && line[i] != '}') i++;
    if (i >= len) { cout << "Malformed definition line: [" << line << "]" << endl; def.name = ""; def.numNested = 0; def.nestedCalls = nullptr; def.requestedMemory = 0; return def; }
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
int findDefIndex(DefCollection& c, string name) {
    for (int i = 0; i < c.count; i++) {
        if (c.data[i].name == name) return i;
    }
    return -1;
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

bool checkUndefined(DefCollection& defs) {
    bool found = false;
    for (int i = 0; i < defs.count; i++) {
        FunctionDef& def = defs.data[i];
        for (int j = 0; j < def.numNested; j++) {
            NestedCall& nc = def.nestedCalls[j];
            if (nc.isTernary) {
                if (findDef(defs, nc.ternaryTrue) == nullptr) {
                    cout << "Error: Undefined function " << nc.ternaryTrue << " called by " << def.name << endl;
                    found = true;
                }
                if (findDef(defs, nc.ternaryFalse) == nullptr) {
                    cout << "Error: Undefined function " << nc.ternaryFalse << " called by " << def.name << endl;
                    found = true;
                }
            }
            else if (findDef(defs, nc.plainName) == nullptr) {
                cout << "Error: Undefined function " << nc.plainName << " called by " << def.name << endl;
                found = true;
            }
        }
    }
    return found;
}


bool dfsCycle(DefCollection& defs, int idx, int* path, int& pathLen,
    bool* inPath, bool* visited, int* cycleOut, int& cycleLen) {
    path[pathLen] = idx;
    pathLen++;
    inPath[idx] = true;

    FunctionDef& def = defs.data[idx];
    for (int j = 0; j < def.numNested; j++) {
        NestedCall& nc = def.nestedCalls[j];

        string neighborNames[2];
        int neighborCount;
        if (nc.isTernary) {
            neighborNames[0] = nc.ternaryTrue;
            neighborNames[1] = nc.ternaryFalse;
            neighborCount = 2;
        }
        else {
            neighborNames[0] = nc.plainName;
            neighborCount = 1;
        }

        for (int n = 0; n < neighborCount; n++) {
            int nbIdx = findDefIndex(defs, neighborNames[n]);
            if (nbIdx == -1) continue; // undefined; checkUndefined reports it separately

            if (inPath[nbIdx]) {
                int startPos = -1;
                for (int p = 0; p < pathLen; p++) {
                    if (path[p] == nbIdx) { startPos = p; break; }
                }
                cycleLen = 0;
                for (int p = startPos; p < pathLen; p++) cycleOut[cycleLen++] = path[p];
                cycleOut[cycleLen++] = nbIdx;
                return true;
            }
            if (!visited[nbIdx]) {
                if (dfsCycle(defs, nbIdx, path, pathLen, inPath, visited, cycleOut, cycleLen))
                    return true;
            }
        }
    }

    pathLen--;
    inPath[idx] = false;
    visited[idx] = true;
    return false;
}

bool findCycle(DefCollection& defs, int* cycleOut, int& cycleLen) {
    bool* visited = new bool[defs.count]();
    bool* inPath = new bool[defs.count]();
    int* path = new int[defs.count];
    int pathLen = 0;
    bool found = false;

    for (int i = 0; i < defs.count && !found; i++) {
        if (!visited[i]) {
            if (dfsCycle(defs, i, path, pathLen, inPath, visited, cycleOut, cycleLen))
                found = true;
        }
    }

    delete[] visited;
    delete[] inPath;
    delete[] path;
    return found;
}

void printCycle(DefCollection& defs, int* cycleOut, int cycleLen) {
    cout << "Error: Circular Dependency: ";
    for (int i = 0; i < cycleLen; i++) {
        cout << defs.data[cycleOut[i]].name;
        if (i != cycleLen - 1) cout << " -> ";
    }
    cout << endl;
}


int align4(int bytes) {
    return ((bytes + 3) / 4) * 4;
}



struct Stats {
    int totalAttempts = 0;
    int successfulCalls = 0;
    int skippedOverflow = 0;
    int maxDepthReached = 0;
    int maxMemUsed = 0;
};

void printStackStatus(CallStack& stack, int used, int S) {
    cout << "Stack: ";
    if (stack.isEmpty()) {
        cout << "EMPTY";
    }
    else {
        for (int i = 0; i < stack.size(); i++) {
            cout << "[" << stack.at(i).funcName << ":" << stack.at(i).allocatedSize << "]";
            if (i != stack.size() - 1) cout << " -> ";
        }
        cout << " <- TOP";
    }
    cout << endl;
    cout << "Memory: " << used << "/" << S << " B" << endl;
}

// ===================== NEW: Execution engine =====================
// Forward declarations (executeFunction and executeInvocation call each other)
void executeFunction(DefCollection& defs, string name, CallStack& stack,
    int& used, int S, Stats& stats, int* callCounts);

void executeInvocation(DefCollection& defs, FunctionDef& def, int totalCount, int depth,
    CallStack& stack, int& used, int S, Stats& stats, int* callCounts) {
    stats.totalAttempts++;
    int aligned = align4(def.requestedMemory);

    if (used + aligned > S) {
        cout << "Error: Stack overflow while calling " << def.name << endl;
        stats.skippedOverflow++;
        return; // this invocation never pushes, never recurses further, never runs its body
    }

    used += aligned;
    Frame f;
    f.funcName = def.name;
    f.allocatedSize = aligned;
    stack.push(f);

    stats.successfulCalls++;
    int idx = findDefIndex(defs, def.name);
    if (idx != -1) callCounts[idx]++;

    if (stack.size() > stats.maxDepthReached) stats.maxDepthReached = stack.size();
    if (used > stats.maxMemUsed) stats.maxMemUsed = used;

    cout << def.name << " called" << endl;
    printStackStatus(stack, used, S);

    // recursion happens before this invocation's own body
    if (depth < totalCount) {
        executeInvocation(defs, def, totalCount, depth + 1, stack, used, S, stats, callCounts);
    }

    // this invocation's own nested-call body, left to right
    for (int j = 0; j < def.numNested; j++) {
        NestedCall& nc = def.nestedCalls[j];
        string selected;
        if (nc.isTernary) {
            selected = nc.condition ? nc.ternaryTrue : nc.ternaryFalse;
        }
        else {
            selected = nc.plainName;
        }
        executeFunction(defs, selected, stack, used, S, stats, callCounts);
    }

    stack.pop();
    used -= aligned;
    cout << def.name << " finished" << endl;
    printStackStatus(stack, used, S);
}

void executeFunction(DefCollection& defs, string name, CallStack& stack,
    int& used, int S, Stats& stats, int* callCounts) {
    FunctionDef* def = findDef(defs, name);
    if (def == nullptr) return; // should already be caught by static validation

    int recursionCount = evalRecursionCount(def->recursionExpr);
    if (recursionCount < 1) recursionCount = 1; // defensive; spec assumes valid positive counts

    executeInvocation(defs, *def, recursionCount, 1, stack, used, S, stats, callCounts);
}

void printSummary(DefCollection& defs, Stats& stats, int S, int* callCounts) {
    cout << "Execution Summary" << endl;
    cout << "-----------------" << endl;
    cout << "Total call attempts: " << stats.totalAttempts << endl;
    cout << "Successful calls: " << stats.successfulCalls << endl;
    cout << "Skipped due to stack overflow: " << stats.skippedOverflow << endl;
    cout << "Maximum stack depth reached: " << stats.maxDepthReached << endl;
    cout << "Maximum stack memory used: " << stats.maxMemUsed << " B" << endl;
    cout << "Total stack capacity: " << S << " B" << endl;

    int bestIdx = -1;
    for (int i = 0; i < defs.count; i++) {
        if (bestIdx == -1 || callCounts[i] > callCounts[bestIdx]) {
            bestIdx = i;
        }
        // strictly '>' preserves first-definition-order on ties, since we never overwrite on equal counts
    }
    if (bestIdx != -1) {
        cout << "Most frequently called function: " << defs.data[bestIdx].name << endl;
    }
}

bool isSpaceChar(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

string trim(string s) {
    int start = 0;
    while (start < (int)s.length() && isSpaceChar(s[start])) start++;
    int end = (int)s.length();
    while (end > start && isSpaceChar(s[end - 1])) end--;
    return s.substr(start, end - start);
}
int main() {
    int N, M, S;
    ifstream fin("input.txt");
    int testCaseNum = 1;

    while (fin >> N >> M >> S) {
        fin.ignore();
        cout << "========== Test Case " << testCaseNum << " ==========" << endl;

        DefCollection defs = makeCollection();
        bool staticErrorFound = false;

        for (int k = 0; k < N; k++) {
            string line;
            getline(fin, line);
            line = trim(line);
            if (line.empty()) { k--; continue; }


            FunctionDef def = parseDefinitionLine(line);

            if (!isValidName(def.name)) {
                cout << "Error: Invalid function name " << def.name << endl;
                staticErrorFound = true;
                continue;
            }
            if (hasDuplicate(defs, def.name)) {
                cout << "Error: Duplicate definition of " << def.name << endl;
                staticErrorFound = true;
                continue;
            }
            addDef(defs, def);
        }

        string* topLevelCalls = new string[M];
        for (int k = 0; k < M; k++) {
            string line;
            while (getline(fin, line)) {
                line = trim(line);
                if (!line.empty()) break;
            }
            topLevelCalls[k] = line;

        }




        // undefined-reference checks (nested calls + top-level calls)
        int beforeCount = staticErrorFound ? 1 : 0; // just a flag holder, see note below
        if (checkUndefined(defs)) staticErrorFound = true; // NOTE: currently prints but doesn't set staticErrorFound — see note after code
        for (int k = 0; k < M; k++) {
            if (findDef(defs, topLevelCalls[k]) == nullptr) {
                cout << "Error: Undefined top-level function " << topLevelCalls[k] << endl;
                staticErrorFound = true;
            }
        }

        // cycle detection
        int* cycleArr = new int[defs.count + 1];
        int cycleLen = 0;
        if (findCycle(defs, cycleArr, cycleLen)) {
            printCycle(defs, cycleArr, cycleLen);
            staticErrorFound = true;
        }
        delete[] cycleArr;

        if (!staticErrorFound) {
            CallStack stack;
            int used = 0;
            Stats stats;
            int* callCounts = new int[defs.count]();

            for (int k = 0; k < M; k++) {
                executeFunction(defs, topLevelCalls[k], stack, used, S, stats, callCounts);
            }

            cout << endl;
            printSummary(defs, stats, S, callCounts);
            delete[] callCounts;
        }

        string sep;
        while (getline(fin, sep)) {
            sep = trim(sep);
            if (!sep.empty()) break;
        }// consume "###" if present; no error if this is the last test case

        testCaseNum++;
        delete[] topLevelCalls;

        for (int i = 0; i < defs.count; i++) delete[] defs.data[i].nestedCalls;
        delete[] defs.data;

        cout << endl;
    }

    return 0;
}
