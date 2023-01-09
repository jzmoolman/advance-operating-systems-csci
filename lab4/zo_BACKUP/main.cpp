#include <iostream>
#include <iterator>
#include <list>
#include <string>

using namespace std;

#define TOKEN_NOP 0
#define TOKEN_INTERNAL 1
#define TOKEN_PARAM 2

typedef struct token {
    int token;
    string value;
} token_t;

#define TEST_CASE_1 "echo param1"

list<token_t> parseLine(char* line) {
    list<token_t> result;
    char buffer[1024] = {0};
    int start = 0;
    int offset = 0;
    token_t t;
    for (int i = 0;  *(line+i)  !='\0'; i++) {
        char c = *(line+i);
        switch (c) {
        case ' ': {
            if ( start == 0) {
                // eat up space
            } else { 
                t.token = TOKEN_INTERNAL;
                t.value = buffer;
                result.push_back(t);
                memset(buffer, 0, 1024);
            }
            start++;
            offset = 0;
            break;
        }        
        default:
            *(buffer+start+offset) = *(line+i);
            break;
        } 

    }
    if (result.size() == 0 ) {
        t.token = TOKEN_INTERNAL;
        t.value = "";
        result.push_back(t);
    }
    return result;
}

void printList(list<string> l) {
 list<string>::iterator it;
    for (it = l.begin(); it != l.end(); ++it) {
        cout << *it << endl;
    }
}

void printTokens(list<token_t> l) {

    list<token_t>::iterator it;
        for (it = l.begin(); it != l.end(); ++it) {
            cout << it->token << " " << it->value << endl;
        }
}

int main() {
    printTokens(parseLine(TEST_CASE_1));
    return 0;
}