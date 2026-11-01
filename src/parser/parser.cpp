#include <iostream>
#include <string>

#include "./parser.h"

namespace
{
    void printASTNode(const ASTNode *node,
                      const std::string &prefix,
                      bool isLast,
                      bool isRoot)
    {
        if (node == nullptr)
        {
            return;
        }

        std::cout << prefix;
        if (!isRoot)
        {
            std::cout << (isLast ? "└── " : "├── ");
        }
        std::cout << node->getType();

        if (const auto *directive = dynamic_cast<const PPDirectiveNode *>(node))
        {
            std::cout << " (" << directive->directive;

            if (!directive->argument.empty())
            {
                std::cout << ": " << directive->argument;
            }

            std::cout << ")";
        }
        else if (const auto *declaration =
                     dynamic_cast<const DeclarationNode *>(node))
        {
            std::cout << " (" << declaration->dataType
                      << " " << declaration->name << ")";
        }
        else if (const auto *function =
                     dynamic_cast<const FunctionNode *>(node))
        {
            std::cout << " (" << function->returnType
                      << " " << function->name << ")";
        }
        else if (const auto *assignment =
                     dynamic_cast<const AssignmentNode *>(node))
        {
            std::cout << " (" << assignment->variableName << ")";
        }
        else if (const auto *expression =
                     dynamic_cast<const ExpressionNode *>(node))
        {
            std::cout << " (" << expression->value << ")";
        }

        std::cout << '\n';

        const std::string childPrefix = isRoot
                                            ? ""
                                            : prefix + (isLast ? "    " : "│   ");

        const auto printSection = [&](const std::string &label,
                                      const std::vector<ASTNode *> &children,
                                      bool sectionIsLast)
        {
            std::cout << childPrefix
                      << (sectionIsLast ? "└── " : "├── ")
                      << label << '\n';

            const std::string sectionPrefix = childPrefix +
                                              (sectionIsLast ? "    " : "│   ");
            for (size_t childIndex = 0; childIndex < children.size(); ++childIndex)
            {
                const bool childIsLast = childIndex + 1 == children.size();
                if (label == "PARAMETERS")
                {
                    const auto *declaration =
                        dynamic_cast<const DeclarationNode *>(children[childIndex]);
                    if (declaration != nullptr)
                    {
                        std::cout << sectionPrefix
                                  << (childIsLast ? "└── " : "├── ")
                                  << "PARAMETER ("
                                  << declaration->dataType << " "
                                  << declaration->name << ")\n";
                        continue;
                    }
                }

                printASTNode(children[childIndex], sectionPrefix, childIsLast, false);
            }
        };

        if (const auto *function = dynamic_cast<const FunctionNode *>(node))
        {
            if (!function->parameters.empty())
            {
                printSection("PARAMETERS", function->parameters,
                             function->body.empty());
            }

            if (!function->body.empty())
            {
                printSection("BODY", function->body, true);
            }

            return;
        }

        if (const auto *forNode = dynamic_cast<const ForNode *>(node))
        {
            printSection("INITIALIZATION", forNode->initialization, false);
            printSection("CONDITION", forNode->condition, false);
            printSection("INCREMENT", forNode->increment, false);
            printSection("BODY", forNode->body, true);

            return;
        }

        if (const auto *declaration =
                dynamic_cast<const DeclarationNode *>(node))
        {
            for (size_t childIndex = 0;
                 childIndex < declaration->initializer.size();
                 ++childIndex)
            {
                printASTNode(declaration->initializer[childIndex], childPrefix,
                             childIndex + 1 == declaration->initializer.size(), false);
            }

            return;
        }

        if (const auto *assignment =
                dynamic_cast<const AssignmentNode *>(node))
        {
            for (size_t childIndex = 0;
                 childIndex < assignment->expression.size();
                 ++childIndex)
            {
                printASTNode(assignment->expression[childIndex], childPrefix,
                             childIndex + 1 == assignment->expression.size(), false);
            }

            return;
        }

        if (const auto *returnNode =
                dynamic_cast<const ReturnNode *>(node))
        {
            for (size_t childIndex = 0;
                 childIndex < returnNode->expression.size();
                 ++childIndex)
            {
                printASTNode(returnNode->expression[childIndex], childPrefix,
                             childIndex + 1 == returnNode->expression.size(), false);
            }

            return;
        }

        if (const auto *ifNode = dynamic_cast<const IfNode *>(node))
        {
            printSection("CONDITION", ifNode->condition, false);
            printSection("THEN", ifNode->thenBranch, ifNode->elseBranch.empty());

            if (!ifNode->elseBranch.empty())
            {
                printSection("ELSE", ifNode->elseBranch, true);
            }

            return;
        }

        if (const auto *whileNode = dynamic_cast<const WhileNode *>(node))
        {
            printSection("CONDITION", whileNode->condition, false);
            printSection("BODY", whileNode->body, true);

            return;
        }

        for (size_t childIndex = 0; childIndex < node->children.size(); ++childIndex)
        {
            printASTNode(node->children[childIndex], childPrefix,
                         childIndex + 1 == node->children.size(), false);
        }
    }

}

void printAST(const ASTNode *node, size_t)
{
    printASTNode(node, "", true, true);
}

ProgramNode *Parser::parseProgram()
{
    ProgramNode *programNode = new ProgramNode();
    programNode->type = "PROGRAM";
    while (lookAhead(index).getType() != "EOF")
    {
        if (lookAhead(index).getType() == "PREPROCESSOR_DIRECTIVE")
        {
            programNode->addChild(parsePPDirective());
        }
        else if (lookAhead(index).getType() == "DATA_TYPE_KEYWORD")
        {
            if (lookAhead(index + 1).getType() == "IDENTIFIER")
            {
                if (lookAhead(index + 2).getValue() == "(")
                {
                    programNode->addChild(parseFunction());
                }
                else if (lookAhead(index + 2).getValue() == ";")
                {
                    programNode->addChild(parseVariableDeclaration());
                }
                else if (lookAhead(index + 2).getValue() == "=" ||
                         lookAhead(index + 2).getValue() == ";")
                {
                    programNode->addChild(parseVariableDeclaration());
                }
                else
                {
                    std::cerr << "Error: Unexpected token after identifier: " << lookAhead(index + 2).getValue() << std::endl;
                    getNextToken(index);
                }
            }
            else
            {
                std::cerr << "Error: Expected identifier after data type, got: " << lookAhead(index + 1).getValue() << std::endl;
                getNextToken(index);
            }
        }
        else
        {
            std::cerr << "Error: Unexpected token: " << lookAhead(index).getValue() << std::endl;
            getNextToken(index);
        }
    }
    return programNode;
}

PPDirectiveNode *Parser::parsePPDirective()
{
    Token token = getNextToken(index);
    if (token.getType() == "PREPROCESSOR_DIRECTIVE")
    {
        PPDirectiveNode *ppDirectiveNode = new PPDirectiveNode();
        ppDirectiveNode->type = "PREPROCESSOR_DIRECTIVE";
        ppDirectiveNode->directive = token.getValue();
        while (lookAhead(index).getType() != "EOF" && lookAhead(index).getType() != "EOPPD")
        {
            ppDirectiveNode->argument += getNextToken(index).getValue() + " ";
        }
        if (lookAhead(index).getType() == "EOPPD")
        {
            getNextToken(index);
        }
        return ppDirectiveNode;
    }
    else
    {
        std::cerr << "Error: Expected preprocessor directive, got: " << token.getValue() << std::endl;
        return nullptr;
    }
}

FunctionNode *Parser::parseFunction()
{
    FunctionNode *functionNode = new FunctionNode();
    functionNode->type = "FUNCTION";
    functionNode->returnType = getNextToken(index).getValue();
    functionNode->name = getNextToken(index).getValue();
    getNextToken(index);

    functionNode->parameters = parseParameterList();

    functionNode->body = parseBlock();

    return functionNode;
}

std::vector<ASTNode *> Parser::parseParameterList()
{
    std::vector<ASTNode *> parameters;
    while (lookAhead(index).getType() != "EOF" && lookAhead(index).getValue() != ")")
    {
        if (lookAhead(index).getType() != "DATA_TYPE_KEYWORD" ||
            lookAhead(index + 1).getType() != "IDENTIFIER")
        {
            getNextToken(index);
            continue;
        }

        DeclarationNode *parameter = new DeclarationNode();
        parameter->type = "PARAMETER";
        parameter->dataType = getNextToken(index).getValue();
        parameter->name = getNextToken(index).getValue();
        parameters.push_back(parameter);

        if (lookAhead(index).getValue() == ",")
        {
            getNextToken(index);
        }
    }
    if (lookAhead(index).getValue() == ")")
    {
        getNextToken(index);
    }
    return parameters;
}

DeclarationNode *Parser::parseVariableDeclaration()
{
    DeclarationNode *declaration = new DeclarationNode();
    declaration->type = "DECLARATION";
    declaration->dataType = getNextToken(index).getValue();
    declaration->name = getNextToken(index).getValue();

    if (lookAhead(index).getValue() == "=")
    {
        getNextToken(index);
        declaration->initializer = parseExpressionUntil(";", true);
    }
    if (lookAhead(index).getValue() == ";")
    {
        getNextToken(index);
    }
    return declaration;
}

AssignmentNode *Parser::parseAssignment()
{
    AssignmentNode *assignment = new AssignmentNode();
    assignment->type = "ASSIGNMENT";
    assignment->variableName = getNextToken(index).getValue();
    if (lookAhead(index).getType() == "OPERATOR")
    {
        getNextToken(index);
    }
    assignment->expression = parseExpressionUntil(";", true);
    return assignment;
}

ASTNode *Parser::parseStatement()
{
    if (lookAhead(index).getType() == "DATA_TYPE_KEYWORD")
    {
        return parseVariableDeclaration();
    }
    if (lookAhead(index).getType() == "CONTROL_FLOW_KEYWORD")
    {
        if (lookAhead(index).getValue() == "if")
        {
            return parseIf();
        }
        if (lookAhead(index).getValue() == "while")
        {
            return parseWhile();
        }
        if (lookAhead(index).getValue() == "for")
        {
            return parseFor();
        }
        if (lookAhead(index).getValue() == "return")
        {
            return parseReturn();
        }
    }
    if (lookAhead(index).getType() == "IDENTIFIER")
    {
        if (lookAhead(index + 1).getType() == "OPERATOR" ||
            lookAhead(index + 1).getValue() == "(")
        {
            return parseAssignment();
        }
    }
    while (lookAhead(index).getType() != "EOF" && lookAhead(index).getValue() != ";" &&
           lookAhead(index).getValue() != "}")
    {
        getNextToken(index);
    }
    if (lookAhead(index).getValue() == ";")
    {
        getNextToken(index);
    }
    return nullptr;
}

std::vector<ASTNode *> Parser::parseExpressionUntil(const std::string &terminator, bool consumeTerminator)
{
    std::vector<ASTNode *> expression;
    if (lookAhead(index).getType() != "EOF" && lookAhead(index).getValue() != terminator)
    {
        ASTNode *root = parseExpression();
        if (root != nullptr)
        {
            expression.push_back(root);
        }
    }
    if (consumeTerminator && lookAhead(index).getValue() == terminator)
    {
        getNextToken(index);
    }
    return expression;
}

ASTNode *Parser::parseExpression(int minimumPrecedence)
{
    ASTNode *left = parsePrimary();
    if (left == nullptr)
    {
        return nullptr;
    }

    while (true)
    {
        const std::string operatorValue = lookAhead(index).getValue();
        const int precedence = getOperatorPrecedence(operatorValue);
        if (precedence < minimumPrecedence)
        {
            break;
        }

        getNextToken(index);
        ASTNode *right = parseExpression(operatorValue == "=" ? precedence : precedence + 1);
        if (right == nullptr)
        {
            break;
        }

        if (operatorValue == "=")
        {
            auto *assignment = new AssignmentNode();
            assignment->type = "ASSIGNMENT";
            if (const auto *identifier = dynamic_cast<const ExpressionNode *>(left))
            {
                assignment->variableName = identifier->value;
            }
            assignment->expression.push_back(right);
            left = assignment;
        }
        else
        {
            ASTNode *operation = new ASTNode();
            operation->type = getOperatorType(operatorValue);
            operation->children.push_back(left);
            operation->children.push_back(right);
            left = operation;
        }
    }
    return left;
}

ASTNode *Parser::parsePrimary()
{
    const Token token = getNextToken(index);
    if (token.getType() == "EOF")
    {
        return nullptr;
    }
    if (token.getValue() == "(")
    {
        ASTNode *expression = parseExpression();
        if (lookAhead(index).getValue() == ")")
        {
            getNextToken(index);
        }
        return expression;
    }

    auto *node = new ExpressionNode();
    node->value = token.getValue();
    node->tokenType = token.getType();
    node->type = token.getType() == "IDENTIFIER" ? "IDENTIFIER" : "LITERAL";

    if (token.getType() == "IDENTIFIER" && lookAhead(index).getValue() == "(")
    {
        getNextToken(index);
        ASTNode *call = new ASTNode();
        call->type = "FUNCTION_CALL";
        call->children.push_back(node);
        while (lookAhead(index).getType() != "EOF" && lookAhead(index).getValue() != ")")
        {
            ASTNode *argument = parseExpression();
            if (argument != nullptr)
            {
                call->children.push_back(argument);
            }
            if (lookAhead(index).getValue() == ",")
            {
                getNextToken(index);
            }
            else if (lookAhead(index).getValue() != ")")
            {
                break;
            }
        }
        if (lookAhead(index).getValue() == ")")
        {
            getNextToken(index);
        }
        return call;
    }
    return node;
}

int Parser::getOperatorPrecedence(const std::string &operatorValue) const
{
    if (operatorValue == "=")
        return 1;
    if (operatorValue == "||")
        return 2;
    if (operatorValue == "&&")
        return 3;
    if (operatorValue == "|")
        return 4;
    if (operatorValue == "&")
        return 5;
    if (operatorValue == "==" || operatorValue == "!=")
        return 6;
    if (operatorValue == "<" || operatorValue == "<=" || operatorValue == ">" || operatorValue == ">=")
        return 7;
    if (operatorValue == "+" || operatorValue == "-")
        return 8;
    if (operatorValue == "*" || operatorValue == "/" || operatorValue == "%")
        return 9;
    return -1;
}

std::string Parser::getOperatorType(const std::string &operatorValue) const
{
    if (operatorValue == "+")
        return "ADDITION";
    if (operatorValue == "-")
        return "SUBTRACTION";
    if (operatorValue == "*")
        return "MULTIPLICATION";
    if (operatorValue == "/")
        return "DIVISION";
    if (operatorValue == "%")
        return "MODULO";
    if (operatorValue == "<")
        return "LESS_THAN";
    if (operatorValue == "<=")
        return "LESS_THAN_OR_EQUAL";
    if (operatorValue == ">")
        return "GREATER_THAN";
    if (operatorValue == ">=")
        return "GREATER_THAN_OR_EQUAL";
    if (operatorValue == "==")
        return "EQUAL";
    if (operatorValue == "!=")
        return "NOT_EQUAL";
    if (operatorValue == "&&")
        return "LOGICAL_AND";
    if (operatorValue == "||")
        return "LOGICAL_OR";
    if (operatorValue == "&")
        return "BITWISE_AND";
    if (operatorValue == "|")
        return "BITWISE_OR";
    return operatorValue;
}

std::vector<ASTNode *> Parser::parseBlock()
{
    std::vector<ASTNode *> statements;
    if (lookAhead(index).getValue() != "{")
    {
        std::cerr << "Error: Expected '{', got: " << lookAhead(index).getValue() << std::endl;
        return statements;
    }
    getNextToken(index);
    while (lookAhead(index).getType() != "EOF" && lookAhead(index).getValue() != "}")
    {
        ASTNode *statement = parseStatement();
        if (statement != nullptr)
        {
            statements.push_back(statement);
        }
    }
    if (lookAhead(index).getValue() == "}")
    {
        getNextToken(index);
    }
    return statements;
}

IfNode *Parser::parseIf()
{
    IfNode *ifNode = new IfNode();
    ifNode->type = "IF";
    getNextToken(index);
    if (lookAhead(index).getValue() == "(")
    {
        getNextToken(index);
        ifNode->condition = parseExpressionUntil(")", true);
    }
    ifNode->thenBranch = parseBlock();
    if (lookAhead(index).getValue() == "else")
    {
        getNextToken(index);
        if (lookAhead(index).getValue() == "if")
        {
            ifNode->elseBranch.push_back(parseIf());
        }
        else
        {
            ifNode->elseBranch = parseBlock();
        }
    }
    return ifNode;
}

WhileNode *Parser::parseWhile()
{
    WhileNode *whileNode = new WhileNode();
    whileNode->type = "WHILE";
    getNextToken(index);
    if (lookAhead(index).getValue() == "(")
    {
        getNextToken(index);
        whileNode->condition = parseExpressionUntil(")", true);
    }
    whileNode->body = parseBlock();
    return whileNode;
}

ForNode *Parser::parseFor()
{
    ForNode *forNode = new ForNode();
    forNode->type = "FOR";
    getNextToken(index);
    if (lookAhead(index).getValue() == "(")
    {
        getNextToken(index);
        if (lookAhead(index).getType() == "DATA_TYPE_KEYWORD")
        {
            forNode->initialization.push_back(parseVariableDeclaration());
        }
        else
        {
            forNode->initialization = parseExpressionUntil(";", true);
        }
        forNode->condition = parseExpressionUntil(";", true);
        forNode->increment = parseExpressionUntil(")", true);
    }
    forNode->body = parseBlock();
    return forNode;
}

ReturnNode *Parser::parseReturn()
{
    ReturnNode *returnNode = new ReturnNode();
    returnNode->type = "RETURN";
    getNextToken(index);
    returnNode->expression = parseExpressionUntil(";", true);
    return returnNode;
}

Token Parser::getNextToken(size_t &index)
{
    if (index < tokens.size())
    {
        return tokens[index++];
    }
    else
    {
        return Token("", "EOF");
    }
}

Token Parser::lookAhead(size_t index)
{
    if (index < tokens.size())
    {
        return tokens[index];
    }
    else
    {
        return Token("", "EOF");
    }
}
