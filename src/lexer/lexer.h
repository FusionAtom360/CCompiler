#pragma once

#include <string>
#include <vector>

class Token
{
public:
    Token(const std::string &value, const std::string &type)
        : value(value), type(type) {}
    std::string getValue() const { return value; }
    std::string getType() const { return type; }

private:
    std::string value;
    std::string type;
};

class Lexer
{
public:
    Lexer(const std::string &input);
    std::vector<Token> tokenize();

private:
    std::string input;
    std::vector<Token> tokens;
    std::string getNextWord(size_t &index);
    char getNextChar(size_t &index);
    std::string getNextNums(size_t &index);
    char lookAhead(size_t index);
};
