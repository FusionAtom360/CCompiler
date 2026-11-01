#pragma once
#include <string>
#include <vector>

#include "../lexer/lexer.h"

class ASTNode
{
public:
    virtual ~ASTNode() = default;

    std::string type;
    std::vector<ASTNode *> children;

    std::string getType() const
    {
        return type;
    }

    void addChild(ASTNode *child)
    {
        children.push_back(child);
    }
};

struct PPDirectiveNode : public ASTNode
{
    std::string directive;
    std::string argument;
};

struct ProgramNode : public ASTNode
{
};

struct DeclarationNode : public ASTNode
{
    std::string dataType;
    std::string name;
    std::vector<ASTNode *> initializer;
};

struct ExpressionNode : public ASTNode
{
    std::string value;
    std::string tokenType;
};

struct FunctionNode : public ASTNode
{
    std::string returnType;
    std::string name;
    std::vector<ASTNode *> parameters;
    std::vector<ASTNode *> body;
};

struct AssignmentNode : public ASTNode
{
    std::string variableName;
    std::vector<ASTNode *> expression;
};

struct IfNode : public ASTNode
{
    std::vector<ASTNode *> condition;
    std::vector<ASTNode *> thenBranch;
    std::vector<ASTNode *> elseBranch;
};

struct WhileNode : public ASTNode
{
    std::vector<ASTNode *> condition;
    std::vector<ASTNode *> body;
};

struct ForNode : public ASTNode
{
    std::vector<ASTNode *> initialization;
    std::vector<ASTNode *> condition;
    std::vector<ASTNode *> increment;
    std::vector<ASTNode *> body;
};

struct ReturnNode : public ASTNode
{
    std::vector<ASTNode *> expression;
};

void printAST(const ASTNode *node, size_t indentation = 0);

class Parser
{
public:
    Parser(const std::vector<Token> &tokens) : tokens(tokens), index(0) {}
    ProgramNode *parseProgram();
    PPDirectiveNode *parsePPDirective();
    FunctionNode *parseFunction();
    DeclarationNode *parseVariableDeclaration();
    AssignmentNode *parseAssignment();

private:
    std::vector<Token> tokens;
    size_t index;
    Token getNextToken(size_t &index);
    Token lookAhead(size_t index);
    std::vector<ASTNode *> parseParameterList();
    std::vector<ASTNode *> parseExpressionUntil(const std::string &terminator, bool consumeTerminator);
    ASTNode *parseExpression(int minimumPrecedence = 0);
    ASTNode *parsePrimary();
    int getOperatorPrecedence(const std::string &operatorValue) const;
    std::string getOperatorType(const std::string &operatorValue) const;
    std::vector<ASTNode *> parseBlock();
    IfNode *parseIf();
    WhileNode *parseWhile();
    ForNode *parseFor();
    ReturnNode *parseReturn();
    ASTNode *parseStatement();
};
