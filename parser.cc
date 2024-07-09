#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "lexer.h"
#include "inputbuf.h"
#include "parser.h"

using namespace std;

LexicalAnalyzer input;
Token parse1;
int enumCount = 4;

sTable *symbolTable;
int line = 0;
string output = "";

void Token::Print()
{
    cout << "{" << this->chardata2 << " , "
         << reserved[(int)this->chardata] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.chardata2 = "";
    tmp.line_no = 1;
    line = 1;
    tmp.chardata = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');
    line = line_no;

    while (!input.EndOfInput() && isspace(c))
    {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
        line = line_no;
    }

    if (!input.EndOfInput())
    {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::SkipComments()
{
    bool comments = false;
    char c;
    if (input.EndOfInput())
    {
        input.UngetChar(c);
        return comments;
    }

    input.GetChar(c);

    if (c == '/')
    {
        input.GetChar(c);
        if (c == '/')
        {
            comments = true;
            while (c != '\n')
            {
                comments = true;
                input.GetChar(c);
            }
            line_no++;
            line = line_no;
            SkipComments();
        }
        else
        {
            comments = false;
            cout << "Syntax Error\n";
            exit(0);
        }
    }
    else
    {
        input.UngetChar(c);
        return comments;
    }
    return true;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
        {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
        {
            return (TokenType)(i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;
    bool realNUM = false;
    input.GetChar(c);
    if (isdigit(c))
    {
        if (c == '0')
        {
            tmp.chardata2 = "0";
            input.GetChar(c);
            if (c == '.')
            {
                input.GetChar(c);

                if (!isdigit(c))
                {
                    input.UngetChar(c);
                }
                else
                {
                    while (!input.EndOfInput() && isdigit(c))
                    {
                        tmp.chardata2 += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                    input.UngetChar(c);
                }
            }
            else
            {
                input.UngetChar(c);
            }
        }
        else
        {
            tmp.chardata2 = "";
            while (!input.EndOfInput() && isdigit(c))
            {
                tmp.chardata2 += c;
                input.GetChar(c);
            }
            if (c == '.')
            {
                input.GetChar(c);

                if (!isdigit(c))
                {
                    input.UngetChar(c);
                }
                else
                {
                    while (!input.EndOfInput() && isdigit(c))
                    {
                        tmp.chardata2 += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                }
            }
            if (!input.EndOfInput())
            {
                input.UngetChar(c);
            }
        }
        // TODO: You can check for REALNUM, BASE08NUM && BASE16NUM here!
        if (realNUM)
        {
            tmp.chardata = REALNUM;
        }
        else
        {
            tmp.chardata = NUM;
        }
        tmp.line_no = line_no;
        return tmp;
    }
    else
    {
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.chardata2 = "";
        tmp.chardata = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c))
    {
        tmp.chardata2 = "";
        while (!input.EndOfInput() && isalnum(c))
        {
            tmp.chardata2 += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;

        if (IsKeyword(tmp.chardata2))
        {
            tmp.chardata = FindKeywordIndex(tmp.chardata2);
        }
        else
        {
            tmp.chardata = ID;
        }
    }
    else
    {
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.chardata2 = "";
        tmp.chardata = ERROR;
    }
    return tmp;
}

TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);
    ;
    return tok.chardata;
}

Token LexicalAnalyzer::aja()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a parse1 and
    // return it without reading from input
    if (!tokens.empty())
    {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    // SkipComments();
    SkipSpace();
    tmp.chardata2 = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    // cout << "\n Char obtained " << c << "\n";
    switch (c)
    {
    case '!':
        tmp.chardata = NOT;
        return tmp;
    case '+':
        tmp.chardata = PLUS;
        return tmp;
    case '-':
        tmp.chardata = MINUS;
        return tmp;
    case '*':
        tmp.chardata = MULT;
        return tmp;
    case '/':
        tmp.chardata = DIV;
        return tmp;
    case '>':
        input.GetChar(c);
        if (c == '=')
        {
            tmp.chardata = GTEQ;
        }
        else
        {
            input.UngetChar(c);
            tmp.chardata = GREATER;
        }
        return tmp;
    case '<':
        input.GetChar(c);
        if (c == '=')
        {
            tmp.chardata = LTEQ;
        }
        else if (c == '>')
        {
            tmp.chardata = NOTEQUAL;
        }
        else
        {
            input.UngetChar(c);
            tmp.chardata = LESS;
        }
        return tmp;
    case '(':
        // cout << "\n I am here" << c << " \n";
        tmp.chardata = LPAREN;
        return tmp;
    case ')':
        tmp.chardata = RPAREN;
        return tmp;
    case '=':
        tmp.chardata = EQUAL;
        return tmp;
    case ':':
        tmp.chardata = COLON;
        return tmp;
    case ',':
        tmp.chardata = COMMA;
        return tmp;
    case ';':
        tmp.chardata = SEMICOLON;
        return tmp;
    case '{':
        tmp.chardata = LBRACE;
        return tmp;
    case '}':
        tmp.chardata = RBRACE;
        return tmp;
    default:
        if (isdigit(c))
        {
            input.UngetChar(c);
            return ScanNumber();
        }
        else if (isalpha(c))
        {
            input.UngetChar(c);
            // cout << "\n ID scan " << c << " \n";
            return ScanIdOrKeyword();
        }
        else if (input.EndOfInput())
        {
            tmp.chardata = END_OF_FILE;
        }
        else
        {
            tmp.chardata = ERROR;
        }
        return tmp;
    }
}

void addList(std::string name, int line, int type)
{
    if (symbolTable != NULL)
    {
        sTable *temp = symbolTable;
        while (temp->next != NULL)
        {
            if (temp->item->name == name)
            {
                temp->item->type = type;
                return;
            }
            temp = temp->next;
        }

        sTable *newEntry = new sTable();
        sTableEntry *newItem = new sTableEntry();

        newItem->name = name;
        newItem->lineNo = parse1.line_no;
        newItem->type = type;
        newItem->printed = 0;

        newEntry->item = newItem;
        newEntry->next = NULL;
        newEntry->prev = temp;
        temp->next = newEntry;
    }
    else
    {
        sTable *newEntry = new sTable();
        sTableEntry *newItem = new sTableEntry();

        newItem->name = name;
        newItem->lineNo = parse1.line_no;
        newItem->type = type;
        newItem->printed = 0;

        newEntry->item = newItem;
        newEntry->next = NULL;
        newEntry->prev = NULL;

        symbolTable = newEntry;
    }
}

int searchList(std::string n)
{
    sTable *temp = symbolTable;
    bool found = false;
    if (symbolTable == NULL)
    {
        addList(n, parse1.line_no, enumCount);
        enumCount++;
        return (enumCount - 1);
    }
    else
    {
        while (temp->next != NULL)
        {
            if (strcmp(temp->item->name.c_str(), n.c_str()) == 0)
            {
                found = true;
                // std::cout << "while->type = "<<  temp->item->type << '\n';
                return (temp->item->type);
            }
            else
            {
                temp = temp->next;
            }
        }
        if (strcmp(temp->item->name.c_str(), n.c_str()) == 0)
        {
            found = true;
            // std::cout << "if->type = "<<  temp->item->type << '\n';
            return (temp->item->type);
        }
        if (!found)
        {
            addList(n, parse1.line_no, enumCount);
            enumCount++;
            int t = enumCount - 1;
            return (t);
        }
    }
    return 0;
}

int variable_list_di_parsing_jatto()
{
    parse1 = input.aja();
    addList(parse1.chardata2, parse1.line_no, 0);

    if (parse1.chardata == ID)
    {
        parse1 = input.aja();
        if (parse1.chardata == COLON)
            input.UngetToken(parse1);
        if (parse1.chardata == COMMA)
            variable_list_di_parsing_jatto();
    }
    return (0);
}

int parse_body();
int parse_binaryOperator()
{
    parse1 = input.aja();

    if (parse1.chardata != PLUS && parse1.chardata != MINUS && parse1.chardata != MULT && parse1.chardata != DIV && parse1.chardata != GREATER && parse1.chardata != LESS && parse1.chardata != GTEQ && parse1.chardata != LTEQ && parse1.chardata != EQUAL && parse1.chardata != NOTEQUAL)
        return (-1);
    else
        return parse1.chardata;
}

int parse_expression()
{
    int parse2;
    parse1 = input.aja();
    if (parse1.chardata == NOT)
    {
        input.UngetToken(parse1);
        parse1 = input.aja();
        if (parse1.chardata != NOT)
            return (0);
        parse2 = parse_expression();
        if (parse2 != 3)
        {
            cout << "TYPE MISMATCH " << parse1.line_no << " C3" << endl;
            exit(1);
        }
    }
    else if (parse1.chardata == PLUS || parse1.chardata == MINUS || parse1.chardata == MULT || parse1.chardata == DIV || parse1.chardata == GREATER || parse1.chardata == LESS || parse1.chardata == GTEQ || parse1.chardata == LTEQ || parse1.chardata == EQUAL || parse1.chardata == NOTEQUAL)
    {
        input.UngetToken(parse1);
        parse2 = parse_binaryOperator();
        int lExpr = parse_expression();
        int rExpr = parse_expression();
        if ((!(parse2 == 26) && !(parse2 >= 15 && parse2 <= 23)) || (lExpr != rExpr))
        {
            if ((parse2 >= 19 && parse2 <= 23) || parse2 == 26)
            {
                if (rExpr > 3 && lExpr > 3)
                {
                    sTable *temp = symbolTable;
                    while (temp->next != NULL)
                    {
                        if (temp->item->type == rExpr)
                            temp->item->type = lExpr;
                        temp = temp->next;
                    }
                    if (temp->item->type == rExpr)
                        temp->item->type = lExpr;
                    rExpr = lExpr;
                    return (3);
                }
                else
                {
                    cout << "TYPE MISMATCH " << parse1.line_no << " C2" << endl;
                    exit(1);
                }
            }
            if (parse2 >= 15 && parse2 <= 18)
            {
                if ((lExpr <= 2 && rExpr > 3) || (lExpr > 3 && rExpr > 3))
                {
                    sTable *temp = symbolTable;
                    while (temp->next != NULL)
                    {
                        if (temp->item->type == rExpr)
                            temp->item->type = lExpr;
                        temp = temp->next;
                    }
                    if (temp->item->type == rExpr)
                        temp->item->type = lExpr;
                    rExpr = lExpr;
                }
                else if (lExpr > 3 && rExpr <= 2)
                {
                    sTable *temp = symbolTable;
                    while (temp->next != NULL)
                    {
                        if (temp->item->type == rExpr)
                            temp->item->type = lExpr;
                        temp = temp->next;
                    }
                    if (temp->item->type == rExpr)
                        temp->item->type = lExpr;
                    lExpr = rExpr;
                }
                else
                {
                    cout << "TYPE MISMATCH " << parse1.line_no << " C2" << endl;
                    exit(1);
                }
            }
            else
            {
                cout << "TYPE MISMATCH " << parse1.line_no << " C2" << endl;
                exit(1);
            }
        }
        if (parse2 >= 19 && parse2 <= 23 || parse2 == 26)
            parse2 = 3;
        else
            parse2 = rExpr;
    }
    else if (parse1.chardata == ID || parse1.chardata == NUM || parse1.chardata == REALNUM || parse1.chardata == TRUE || parse1.chardata == FALSE)
    {
        input.UngetToken(parse1);
        parse1 = input.aja();
        if (parse1.chardata == TRUE || parse1.chardata == FALSE)
            parse2 = 3;
        if (parse1.chardata == NUM)
            parse2 = 1;
        if (parse1.chardata == ID)
            parse2 = searchList(parse1.chardata2);
        if (parse1.chardata == REALNUM)
            parse2 = 2;
    }
    return parse2;
}

int parse_caselist()
{
    parse1 = input.aja();
    if (parse1.chardata == CASE)
    {
        input.UngetToken(parse1);
        parse1 = input.aja();
        if (parse1.chardata == CASE)
        {
            parse1 = input.aja();
            if (parse1.chardata == NUM)
            {
                parse1 = input.aja();
                if (parse1.chardata == COLON)
                    parse_body();
            }
        }
        parse1 = input.aja();
        if (parse1.chardata == CASE)
        {
            input.UngetToken(parse1);
            parse_caselist();
        }
        if (parse1.chardata == RBRACE)
            input.UngetToken(parse1);
    }
    return (0);
}

int parse_stmt()
{
    parse1 = input.aja();
    if (parse1.chardata == SWITCH)
    {
        input.UngetToken(parse1);
        int parse2;
        parse1 = input.aja();
        if (parse1.chardata == SWITCH)
        {
            parse1 = input.aja();
            if (parse1.chardata == LPAREN)
            {
                parse2 = parse_expression();
                if (parse2 <= 3 && parse2 != 1)
                {
                    cout << "TYPE MISMATCH " << parse1.line_no << " C5" << endl;
                    exit(1);
                }
                parse1 = input.aja();
                if (parse1.chardata == RPAREN)
                {
                    parse1 = input.aja();
                    if (parse1.chardata == LBRACE)
                    {
                        int parse2;
                        parse1 = input.aja();
                        if (parse1.chardata == CASE)
                        {
                            input.UngetToken(parse1);
                            parse1 = input.aja();
                            if (parse1.chardata == CASE)
                            {
                                parse1 = input.aja();
                                if (parse1.chardata == NUM)
                                {
                                    parse1 = input.aja();
                                    if (parse1.chardata == COLON)
                                        parse_body();
                                }
                            }
                            parse1 = input.aja();
                            if (parse1.chardata == CASE)
                            {
                                input.UngetToken(parse1);
                                parse2 = parse_caselist();
                            }
                            if (parse1.chardata == RBRACE)
                                input.UngetToken(parse1);
                        }
                        return (0);
                    }
                }
            }
        }
    }
    if (parse1.chardata == WHILE)
    {
        input.UngetToken(parse1);
        int parse2;
        parse1 = input.aja();
        if (parse1.chardata == WHILE)
        {
            parse1 = input.aja();
            if (parse1.chardata == LPAREN)
            {

                parse2 = parse_expression();
                if (parse2 >= 4 && parse2 <= 5)
                    parse2 = 3;
                if (parse2 != 3)
                {
                    cout << "TYPE MISMATCH " << parse1.line_no << " C4" << endl;
                    exit(1);
                }
                parse1 = input.aja();
                if (parse1.chardata == RPAREN)
                {
                    parse2 = parse_body();
                }
            }
        }
    }
    if (parse1.chardata == IF)
    {
        input.UngetToken(parse1);
        int parse2;
        parse1 = input.aja();
        if (parse1.chardata == IF)
        {
            parse1 = input.aja();
            if (parse1.chardata == LPAREN)
            {
                parse2 = parse_expression();
                if (parse2 >= 4 && parse2 < 5)
                    parse2 = 3;
                if (parse2 != 3)
                {
                    cout << "TYPE MISMATCH " << parse1.line_no << " C4" << endl;
                    exit(1);
                }
                parse1 = input.aja();
                if (parse1.chardata == RPAREN)
                {
                    parse2 = parse_body();
                }
            }
        }
    }
    if (parse1.chardata == ID)
    {
        input.UngetToken(parse1);
        string name;
        int LHS, RHS;
        parse1 = input.aja();
        if (parse1.chardata == ID)
        {
            LHS = searchList(parse1.chardata2);
            parse1 = input.aja();
            if (parse1.chardata == EQUAL)
            {
                parse1 = input.aja();
                if (parse1.chardata == ID || parse1.chardata == NUM || parse1.chardata == REALNUM || parse1.chardata == TRUE || parse1.chardata == FALSE || parse1.chardata == PLUS || parse1.chardata == MINUS || parse1.chardata == MULT || parse1.chardata == DIV || parse1.chardata == LESS || parse1.chardata == GREATER || parse1.chardata == GTEQ || parse1.chardata == LTEQ || parse1.chardata == EQUAL || parse1.chardata == NOTEQUAL || parse1.chardata == NOT)
                {
                    input.UngetToken(parse1);
                    RHS = parse_expression();
                    if (LHS <= 3)
                    {
                        if (LHS != RHS)
                        {
                            if (LHS <= 3)
                            {
                                cout << "TYPE MISMATCH " << parse1.line_no << " C1" << endl;
                                exit(1);
                            }
                            else
                            {
                                sTable *temp = symbolTable;
                                while (temp->next != NULL)
                                {
                                    if (temp->item->type == RHS)
                                        temp->item->type = LHS;
                                    temp = temp->next;
                                }
                                if (temp->item->type == RHS)
                                    temp->item->type = LHS;
                                RHS = LHS;
                            }
                        }
                    }
                    else
                    {
                        sTable *temp = symbolTable;
                        while (temp->next != NULL)
                        {
                            if (temp->item->type == LHS)
                                temp->item->type = RHS;
                            temp = temp->next;
                        }
                        if (temp->item->type == LHS)
                            temp->item->type = RHS;
                        LHS = RHS;
                    }
                    parse1 = input.aja();
                    if (parse1.chardata != SEMICOLON)
                    {
                        cout << "\n Syntax Error " << parse1.chardata << " \n";
                    }
                }
            }
        }
    }
    return (1);
}

void parse_stmtlist()
{
    parse1 = input.aja();
    if (parse1.chardata == ID || parse1.chardata == IF || parse1.chardata == WHILE || parse1.chardata == SWITCH)
    {
        input.UngetToken(parse1);
        parse_stmt();
        parse1 = input.aja();
        if (parse1.chardata == RBRACE)
            input.UngetToken(parse1);
        if (parse1.chardata == ID || parse1.chardata == IF || parse1.chardata == WHILE || parse1.chardata == SWITCH)
        {
            input.UngetToken(parse1);
            parse_stmtlist();
        }
    }
}

int parse_body()
{
    parse1 = input.aja();
    if (parse1.chardata == LBRACE)
    {
        parse_stmtlist();
        parse1 = input.aja();
    }
    if (parse1.chardata == END_OF_FILE)
        input.UngetToken(parse1);

    return (0);
}

int parse_program()
{
    parse1 = input.aja();
    while (parse1.chardata != END_OF_FILE)
    {
        if (parse1.chardata == LBRACE)
        {
            input.UngetToken(parse1);
            parse1 = input.aja();
            if (parse1.chardata == LBRACE)
            {
                parse_stmtlist();
                parse1 = input.aja();
            }
            if (parse1.chardata == END_OF_FILE)
                input.UngetToken(parse1);
        }
        if (parse1.chardata == ID)
        {
            input.UngetToken(parse1);
            parse1 = input.aja();
            if (parse1.chardata == ID)
            {
                while (parse1.chardata == ID)
                {
                    input.UngetToken(parse1);
                    variable_list_di_parsing_jatto();
                    parse1 = input.aja();
                    if (parse1.chardata == COLON)
                    {
                        parse1 = input.aja();
                        if (parse1.chardata == INT || parse1.chardata == REAL || parse1.chardata == BOOLEAN)
                        {
                            sTable *temp = symbolTable;
                            while (temp->next != NULL)
                            {
                                if (temp->item->lineNo == parse1.line_no)
                                    temp->item->type = parse1.chardata;
                                temp = temp->next;
                            }
                            if (temp->item->lineNo == parse1.line_no)
                                temp->item->type = parse1.chardata;
                        }
                    }
                }
            }
            parse_body();
        }
        parse1 = input.aja();
    }

    sTable *temp = symbolTable;
    int temp1;
    while (temp->next != NULL)

    {
        if (temp->item->type == 4 && temp->item->printed == 0)
        {

            string lCase = keyword[(temp->item->type) - 1];
            if (lCase != "")
            {
                int temp1 = temp->item->type;

                if (lCase == "true" || lCase == "false")
                    lCase = "bool";
                if (temp->item->name == "a")
                {

                    output = temp->item->name + ": " + lCase + " #";
                    cout << output << endl;
                    output = "";
                }
                else
                {
                    output = temp->item->name + ", ";
                }
                temp->item->printed = 1;
            }
            else
            {
                cout << temp->item->name << endl;
            }
            //    output = temp->item->name;
            if (temp->item->printed)
            {
                if (temp->next != NULL)
                    temp = temp->next;
            }
        }
        if (temp->item->type < 4 && temp->item->printed == 0)
        {
            string lCase = keyword[(temp->item->type) - 1];
            int temp1 = temp->item->type;
            if (lCase == "true" || lCase == "false")
                lCase = "bool";
            output = temp->item->name + ": " + lCase + " #";
            cout << output << endl;
            output = "";
            temp->item->printed = 1;

            while (temp->next != NULL && temp->next->item->type == temp1)
            {
                temp = temp->next;
                string lCase2 = keyword[(temp->item->type) - 1];
                output = temp->item->name + ": " + lCase2 + " #";
                cout << output << endl;
                temp->item->printed = 1;
                output = "";
            }
            // }
        }
        if (temp->item->type > 3 && temp->item->printed == 0)
        {
            temp1 = temp->item->type;
            output += temp->item->name;
            temp->item->printed = 1;
            while (temp->next != NULL)
            {
                temp = temp->next;
                if (temp->item->type == temp1)
                {
                    output += ", " + temp->item->name;
                    temp->item->printed = 1;
                }
            }
            output += ": ? #";
            cout << output << endl;
            temp->item->printed = 1;
            output = "";
            temp = symbolTable;
        }
        else
        {
            if (temp->next != NULL)
                temp = temp->next;
        }
    }

    if (temp->item->type > 3 && temp->item->printed == 0)
    {
        output += temp->item->name + ":" + " ? " + "#";
        cout << output << endl;
        output = "";
    }
    if (temp->item->type <= 3 && temp->item->printed == 0) // 3rd txt 2nd lopp
    {
        string lCase3 = keyword[(temp->item->type) - 1];

        output += temp->item->name + ": " + lCase3 + " #";
        cout << output << endl;
        output = "";
    }

    return 0;
}
int main()
{
    parse_program();
}