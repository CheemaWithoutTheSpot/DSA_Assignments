#include <iostream>
#include <string>
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

class CallStack {
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

FunctionDef parseDefinitionLine(string line) {
    FunctionDef def;

    // 1. ida naam before '('
    int i = 0;
    while (line[i] != '(') i++;
    def.name = line.substr(0, i);

    // 2. recursion number inside ()'
    int start = i + 1;
    while (line[i] != ')') i++;
    def.recursionExpr = line.substr(start, i - start);  // could be empty string
    i++; // skip ')'

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
            while ( j < size && body[j] != ',' ) j++;
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