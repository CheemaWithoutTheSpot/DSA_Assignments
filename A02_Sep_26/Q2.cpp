#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
using namespace std;

// lexical states as plain constants, haan na deveo sanu enum hor vekho
const int STATE_NORMAL = 0; //basically tells the state u are in, like ts normal code
const int STATE_STRING = 1;    //ts inside string
const int STATE_SINGLE_COMMENT = 3;
const int STATE_MULTI_COMMENT = 4;
const int STATE_CHAR = 2;


struct BracketFrame {
    char bracket;
    int line;
    int col;
};

//dynamic array that doubles capacity when full
class BracketStack {
    BracketFrame* arr;  //arr of bracket frame, containing the line number col and bracket
    int capacity;
    int count;
public:
    BracketStack() {
        capacity = 8;
        count = 0;
        arr = new BracketFrame[capacity];
    }
    ~BracketStack() { delete[] arr; }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }

    void push(char b, int line, int col) {
        if (count == capacity) {
            capacity *= 2;
            BracketFrame* bigger = new BracketFrame[capacity];
            for (int i = 0; i < count; i++) bigger[i] = arr[i];
            delete[] arr;
            arr = bigger;
        }
        arr[count].bracket = b;
        arr[count].line = line;
        arr[count].col = col;
        count++;
    }

    BracketFrame pop() {
        count--;
        return arr[count];
    }

    BracketFrame top() const {
        return arr[count - 1];
    }
};

char matchingClose(char open) { //return karda si the matching closed brackets
    if (open == '(') return ')';
    if (open == '{') return '}';
    return ']';
}

// processes one source line, updating lexical state / stack / stats across the whole call
void processLine(const string& sourceLine, int lineNum, BracketStack& stack, int& state,
    int& maxDepth, int& matchedPairs, bool& errorFound)
{

    if (state == STATE_NORMAL)
    {
        int idx = 0;
        while (idx < sourceLine.size() && (sourceLine[idx] == ' ' || sourceLine[idx] == '\t')) idx++;   //skip tabs and spaces
        if (idx < sourceLine.size() && sourceLine[idx] == '#') return;  //if # ignore whole line
    }

    int len = sourceLine.size();
    for (int i = 0; i < len; i++)
    { //ts goes char by char
        char c = sourceLine[i];
        int col = i + 1;

        if (state == STATE_NORMAL) {
            if (c == '"') state = STATE_STRING; //setting states
            else if (c == '\'') state = STATE_CHAR;
            else if (c == '/' && i + 1 < len && sourceLine[i + 1] == '/') { state = STATE_SINGLE_COMMENT; i++; }
            else if (c == '/' && i + 1 < len && sourceLine[i + 1] == '*') { state = STATE_MULTI_COMMENT; i++; }
            else if (c == '(' || c == '{' || c == '[') {
                stack.push(c, lineNum, col);
                if (stack.size() > maxDepth) maxDepth = stack.size();
            }
            else if (c == ')' || c == '}' || c == ']') {
                if (stack.isEmpty())
                {
                    cout << "INVALID\n";
                    cout << "Error at Line " << lineNum << ", Column " << col
                        << ": Unexpected '" << c << "'\n";
                    errorFound = true;
                    return;
                }   //error checking for bracket case
                BracketFrame topFrame = stack.top();
                char expected = matchingClose(topFrame.bracket);
                if (c != expected) //compare the expected closing of the bracket of the top of the stack with c
                {
                    cout << "INVALID\n";
                    cout << "Error at Line " << lineNum << ", Column " << col
                        << ": Expected '" << expected << "' but found '" << c << "'\n";
                    errorFound = true;
                    return;
                }
                stack.pop();
                matchedPairs++;
            }
        }
        else if (state == STATE_STRING) {
            // a backslash escapes whatever character follows, so that character can never terminate the string, even if it's a quote

            if (c == '\\') i++;
            else if (c == '"') state = STATE_NORMAL;
        }
        else if (state == STATE_CHAR) {
            if (c == '\\') i++;
            else if (c == '\'') state = STATE_NORMAL;
        }
        else if (state == STATE_MULTI_COMMENT) {    //ignore everything untill you find */
            if (c == '*' && i + 1 < len && sourceLine[i + 1] == '/') { state = STATE_NORMAL; i++; }
        }
        // STATE_SINGLE_COMMENT: every remaining character on the line is ignored
    }

    if (state == STATE_SINGLE_COMMENT) state = STATE_NORMAL;
}

int main() {
    ifstream file("input.txt");
    if (!file.is_open()) {
        cout << "Could not open input.txt" << endl;
        return 1;
    }

    int testCaseNum = 0;
    string currentLine;
    bool haveLine = (bool)getline(file, currentLine);

    while (haveLine) //this while for each test case
    {
        testCaseNum++;
        cout << "========== Test Case " << testCaseNum << " ==========\n";

        BracketStack stack;
        int state = STATE_NORMAL;
        int maxDepth = 0;
        int matchedPairs = 0;
        bool errorFound = false;
        int lineNum = 0;

        while (haveLine) //this one for each line
        {
            if (currentLine == "###") {
                haveLine = (bool)getline(file, currentLine);
                break;
            }

            lineNum++;
            if (!errorFound) {
                processLine(currentLine, lineNum, stack, state, maxDepth, matchedPairs, errorFound);
            }

            haveLine = (bool)getline(file, currentLine);
        }

        if (!errorFound) {
            if (!stack.isEmpty()) {
                BracketFrame topFrame = stack.top();
                cout << "INVALID\n";
                cout << "Error: '" << topFrame.bracket << "' opened at Line " << topFrame.line
                    << ", Column " << topFrame.col << " was never closed\n";
            }
            else {
                cout << "VALID\n";
                cout << "Maximum Nesting Depth: " << maxDepth << "\n";
                cout << "Total Matched Pairs: " << matchedPairs << "\n";
            }
        }
    }

    file.close();
    return 0;
}