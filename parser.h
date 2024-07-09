#ifndef __PARSER__H__
#define __PARSER__H__

#include <vector>
#include <string>

#define KEYWORDS_COUNT 11
#define RESERVED_COUNT 35

using std::string;

const string reserved[RESERVED_COUNT] = {
    "END_OF_FILE", "INT", "REAL", "BOOL",
    "TRUE", "FALSE", "IF", "WHILE", "SWITCH",
    "CASE", "PUBLIC", "PRIVATE", "NUM",
    "REALNUM", "NOT", "PLUS", "MINUS", "MULT",
    "DIV", "GTEQ", "GREATER", "LTEQ", "NOTEQUAL",
    "LESS", "LPAREN", "RPAREN", "EQUAL", "COLON",
    "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "ID",
    "ERROR" // TODO: Add labels for new token types here (as string)
};

const string keyword[KEYWORDS_COUNT] = {
    "int", "real", "bool", "true",
    "false", "if", "while", "switch",
    "case", "public", "private"};

struct sTableEntry
{
    string name;
    int lineNo;
    int type;
    int printed;
};

struct sTable
{
    sTableEntry *item;
    sTable *prev;
    sTable *next;
};

#endif //__PARSER__H__
