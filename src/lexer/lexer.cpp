#include <cctype>
#include <string>
#include <vector>
#include "./lexer.h"

Lexer::Lexer(const std::string &input)
{
    this->input = input;
}

std::vector<Token> Lexer::tokenize()
{
    tokens.clear();
    size_t index = 0;
    bool ppdFlag = false;
    while (index < input.length())
    {
        std::string word = getNextWord(index);
        if (word == "int")
        {
            tokens.push_back(Token(word, "DATA_TYPE_KEYWORD"));
        }
        else if (word == "char")
        {
            tokens.push_back(Token(word, "DATA_TYPE_KEYWORD"));
        }
        else if (word == "float")
        {
            tokens.push_back(Token(word, "DATA_TYPE_KEYWORD"));
        }
        else if (word == "double")
        {
            tokens.push_back(Token(word, "DATA_TYPE_KEYWORD"));
        }
        else if (word == "bool")
        {
            tokens.push_back(Token(word, "DATA_TYPE_KEYWORD"));
        }
        else if (word == "void")
        {
            tokens.push_back(Token(word, "DATA_TYPE_KEYWORD"));
        }
        else if (word == "short")
        {
            tokens.push_back(Token(word, "DATA_TYPE_KEYWORD"));
        }
        else if (word == "long")
        {
            tokens.push_back(Token(word, "DATA_TYPE_KEYWORD"));
        }
        else if (word == "signed")
        {
            tokens.push_back(Token(word, "DATA_TYPE_KEYWORD"));
        }
        else if (word == "unsigned")
        {
            tokens.push_back(Token(word, "DATA_TYPE_KEYWORD"));
        }
        else if (word == "if")
        {
            tokens.push_back(Token(word, "CONTROL_FLOW_KEYWORD"));
        }
        else if (word == "else")
        {
            tokens.push_back(Token(word, "CONTROL_FLOW_KEYWORD"));
        }
        else if (word == "switch")
        {
            tokens.push_back(Token(word, "CONTROL_FLOW_KEYWORD"));
        }
        else if (word == "case")
        {
            tokens.push_back(Token(word, "CONTROL_FLOW_KEYWORD"));
        }
        else if (word == "default")
        {
            tokens.push_back(Token(word, "CONTROL_FLOW_KEYWORD"));
        }
        else if (word == "for")
        {
            tokens.push_back(Token(word, "CONTROL_FLOW_KEYWORD"));
        }
        else if (word == "while")
        {
            tokens.push_back(Token(word, "CONTROL_FLOW_KEYWORD"));
        }
        else if (word == "do")
        {
            tokens.push_back(Token(word, "CONTROL_FLOW_KEYWORD"));
        }
        else if (word == "break")
        {
            tokens.push_back(Token(word, "CONTROL_FLOW_KEYWORD"));
        }
        else if (word == "continue")
        {
            tokens.push_back(Token(word, "CONTROL_FLOW_KEYWORD"));
        }
        else if (word == "goto")
        {
            tokens.push_back(Token(word, "CONTROL_FLOW_KEYWORD"));
        }
        else if (word == "return")
        {
            tokens.push_back(Token(word, "CONTROL_FLOW_KEYWORD"));
        }
        else if (word == "auto")
        {
            tokens.push_back(Token(word, "STORAGE_CLASS_KEYWORD"));
        }
        else if (word == "register")
        {
            tokens.push_back(Token(word, "STORAGE_CLASS_KEYWORD"));
        }
        else if (word == "static")
        {
            tokens.push_back(Token(word, "STORAGE_CLASS_KEYWORD"));
        }
        else if (word == "extern")
        {
            tokens.push_back(Token(word, "STORAGE_CLASS_KEYWORD"));
        }
        else if (word == "struct")
        {
            tokens.push_back(Token(word, "USER_DEFINED_TYPE_KEYWORD"));
        }
        else if (word == "union")
        {
            tokens.push_back(Token(word, "USER_DEFINED_TYPE_KEYWORD"));
        }
        else if (word == "enum")
        {
            tokens.push_back(Token(word, "USER_DEFINED_TYPE_KEYWORD"));
        }
        else if (word == "typedef")
        {
            tokens.push_back(Token(word, "USER_DEFINED_TYPE_KEYWORD"));
        }
        else if (word == "const")
        {
            tokens.push_back(Token(word, "QUALIFIER_KEYWORD"));
        }
        else if (word == "volatile")
        {
            tokens.push_back(Token(word, "QUALIFIER_KEYWORD"));
        }
        else if (word == "restrict")
        {
            tokens.push_back(Token(word, "QUALIFIER_KEYWORD"));
        }
        else if (word == "sizeof")
        {
            tokens.push_back(Token(word, "QUALIFIER_KEYWORD"));
        }
        else if (word == "true")
        {
            tokens.push_back(Token(word, "BOOLEAN_LITERAL"));
        }
        else if (word == "false")
        {
            tokens.push_back(Token(word, "BOOLEAN_LITERAL"));
        }
        else if (!word.empty())
        {
            tokens.push_back(Token(word, "IDENTIFIER"));
        }
        else
        {
            std::string nextNums = getNextNums(index);
            if (!nextNums.empty())
            {
                if (lookAhead(index) == '.')
                {
                    index++;
                    std::string decimalPart = getNextNums(index);
                    if (!decimalPart.empty())
                    {
                        tokens.push_back(Token(nextNums + "." + decimalPart, "FLOAT_LITERAL"));
                    }
                    else
                    {
                        tokens.push_back(Token(nextNums + ".", "FLOAT_LITERAL"));
                    }
                }
                else
                {
                    tokens.push_back(Token(nextNums, "INTEGER_LITERAL"));
                }
            }
            char nextChar = getNextChar(index);
            switch (nextChar)
            {
            case '+':
                switch (lookAhead(index))
                {
                case '+':
                    tokens.push_back(Token("++", "OPERATOR"));
                    index++;
                    break;
                case '=':
                    tokens.push_back(Token("+=", "OPERATOR"));
                    index++;
                    break;
                default:
                    tokens.push_back(Token("+", "OPERATOR"));
                    break;
                }
                break;
            case '-':
                switch (lookAhead(index))
                {
                case '-':
                    tokens.push_back(Token("--", "OPERATOR"));
                    index++;
                    break;
                case '=':
                    tokens.push_back(Token("-=", "OPERATOR"));
                    index++;
                    break;
                default:
                    tokens.push_back(Token("-", "OPERATOR"));
                    break;
                }
                break;
            case '*':
                tokens.push_back(Token(std::string(1, nextChar), "OPERATOR"));
                break;
            case '/':
            {
                if (lookAhead(index) == '/')
                {
                    while (index < input.length() && input[index] != '\n')
                    {
                        index++;
                    }
                }
                else if (lookAhead(index) == '*')
                {
                    while (index < input.length())
                    {
                        size_t nextIndex = index + 1;
                        if (input[index] == '*' && getNextChar(nextIndex) == '/')
                        {
                            index += 2;
                            break;
                        }
                        index++;
                    }
                }
                else
                {
                    tokens.push_back(Token(std::string(1, nextChar), "OPERATOR"));
                }
                break;
            }
            case '%':
                tokens.push_back(Token(std::string(1, nextChar), "OPERATOR"));
                break;
            case '#':
            {
                ppdFlag = true;
                std::string nextWord = getNextWord(index);
                if (nextWord == "include")
                {
                    tokens.push_back(Token("#include", "PREPROCESSOR_DIRECTIVE"));
                }
                else if (nextWord == "define")
                {
                    tokens.push_back(Token("#define", "PREPROCESSOR_DIRECTIVE"));
                }
                break;
            }
            case '<':
                if (ppdFlag)
                {
                    std::string headerFile;
                    while (index < input.length() && input[index] != '>')
                    {
                        headerFile += input[index++];
                    }
                    if (index < input.length() && input[index] == '>')
                    {
                        index++;
                        tokens.push_back(Token("<" + headerFile + ">", "HEADER_FILE"));
                        break;
                    }
                }
                else
                {
                    if (lookAhead(index) == '=')
                    {
                        tokens.push_back(Token("<=", "OPERATOR"));
                        index++;
                        break;
                    }
                    else
                    {
                        tokens.push_back(Token("<", "OPERATOR"));
                        break;
                    }
                }
            case '>':
                if (lookAhead(index) == '=')
                {
                    tokens.push_back(Token(">=", "OPERATOR"));
                    index++;
                    break;
                }
                else
                {
                    tokens.push_back(Token(">", "OPERATOR"));
                    break;
                }
            case '=':
                if (lookAhead(index) == '=')
                {
                    tokens.push_back(Token("==", "OPERATOR"));
                    index++;
                    break;
                }
                else
                {
                    tokens.push_back(Token("=", "OPERATOR"));
                    break;
                }
            case '!':
                if (lookAhead(index) == '=')
                {
                    tokens.push_back(Token("!=", "OPERATOR"));
                    index++;
                    break;
                }
                else
                {
                    tokens.push_back(Token("!", "OPERATOR"));
                    break;
                }
            case '&':
                if (lookAhead(index) == '&')
                {
                    tokens.push_back(Token("&&", "OPERATOR"));
                    index++;
                    break;
                }
                else
                {
                    tokens.push_back(Token("&", "OPERATOR"));
                    break;
                }
            case '|':
                if (lookAhead(index) == '|')
                {
                    tokens.push_back(Token("||", "OPERATOR"));
                    index++;
                    break;
                }
                else
                {
                    tokens.push_back(Token("|", "OPERATOR"));
                    break;
                }
            case '.':
                tokens.push_back(Token(std::string(1, nextChar), "PUNCTUATOR"));
                break;
            case ';':
                tokens.push_back(Token(std::string(1, nextChar), "PUNCTUATOR"));
                break;
            case ',':
                tokens.push_back(Token(std::string(1, nextChar), "PUNCTUATOR"));
                break;
            case '(':
                tokens.push_back(Token(std::string(1, nextChar), "PUNCTUATOR"));
                break;
            case ')':
                tokens.push_back(Token(std::string(1, nextChar), "PUNCTUATOR"));
                break;
            case '{':
                tokens.push_back(Token(std::string(1, nextChar), "PUNCTUATOR"));
                break;
            case '}':
                tokens.push_back(Token(std::string(1, nextChar), "PUNCTUATOR"));
                break;
            case '[':
                tokens.push_back(Token(std::string(1, nextChar), "PUNCTUATOR"));
                break;
            case ']':
                tokens.push_back(Token(std::string(1, nextChar), "PUNCTUATOR"));
                break;
            case '\'':
            {
                size_t beginIndex = index - 1;
                while (index < input.length())
                {
                    char c = getNextChar(index);
                    if (c == '\'' and input[index - 2] != '\\')
                    {
                        tokens.push_back(Token(input.substr(beginIndex, index - beginIndex), "CHAR_LITERAL"));
                        break;
                    }
                }
                break;
            }
            case '\"':
            {
                size_t beginIndex = index - 1;
                while (index < input.length())
                {
                    char c = getNextChar(index);
                    if (c == '\"' && input[index - 2] != '\\')
                    {
                        if (ppdFlag)
                        {
                            tokens.push_back(Token(input.substr(beginIndex, index - beginIndex), "HEADER_FILE"));
                        }
                        else
                        {
                            tokens.push_back(Token(input.substr(beginIndex, index - beginIndex), "STRING_LITERAL"));
                        }
                        break;
                    }
                }
                break;
            }
            case '\n':
                if (ppdFlag)
                {
                    tokens.push_back(Token("", "EOPPD"));
                }
                ppdFlag = false;
                break;
            }
        }
    }
    tokens.push_back(Token("", "EOF"));
    return tokens;
}

char Lexer::getNextChar(size_t &index)
{
    if (index < input.length())
    {
        return input[index++];
    }
    return '\0';
}

char Lexer::lookAhead(size_t index)
{
    if (index < input.length())
    {
        return input[index];
    }
    return '\0';
}

std::string Lexer::getNextWord(size_t &index)
{
    std::string word;
    if (index < input.length() && (std::isalpha(input[index])))
    {
        word += input[index++];
        while (index < input.length() && (std::isalnum(input[index]) || input[index] == '_'))
        {
            word += input[index++];
        }
    }
    return word;
}

std::string Lexer::getNextNums(size_t &index)
{
    std::string nums;
    while (index < input.length() && std::isdigit(input[index]))
    {
        nums += input[index++];
    }
    return nums;
}
