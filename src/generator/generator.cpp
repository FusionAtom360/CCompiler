#include "./generator.h"

#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace
{

    struct Variable
    {
        int offset;
        bool global;
    };

    class Emitter
    {
    public:
        std::string generate(const ProgramNode &program)
        {
            collectGlobals(program);

            for (const ASTNode *node : program.children)
            {
                if (const auto *function =
                        dynamic_cast<const FunctionNode *>(node))
                {
                    emitFunction(*function);
                }
            }

            return output.str();
        }

    private:
        std::ostringstream output;

        std::unordered_map<std::string, Variable> variables;
        std::unordered_map<std::string, const DeclarationNode *> globals;

        int labelNumber = 0;
        std::string returnLabel;

        void emit(const std::string &line)
        {
            output << line << '\n';
        }

        std::string newLabel(const std::string &name)
        {
            return ".L" + name + std::to_string(labelNumber++);
        }

        void collectGlobals(const ProgramNode &program)
        {
            for (const ASTNode *node : program.children)
            {
                if (const auto *declaration =
                        dynamic_cast<const DeclarationNode *>(node))
                {
                    globals[declaration->name] = declaration;
                }
            }
        }

        void emitFunction(const FunctionNode &function)
        {
            variables.clear();
            returnLabel = newLabel("return_");

            emit(function.name + ":");
            emit("    stp x29, x30, [sp, #-16]!");
            emit("    mov x29, sp");

            const int variableBytes =
                4 * (static_cast<int>(function.parameters.size()) +
                     countDeclarations(function.body));
            const int frameBytes = (variableBytes + 15) & ~15;
            if (frameBytes != 0)
                emit("    sub sp, sp, #" + std::to_string(frameBytes));

            for (size_t i = 0; i < function.parameters.size(); ++i)
            {
                const auto *parameter =
                    dynamic_cast<const DeclarationNode *>(function.parameters[i]);

                if (parameter == nullptr)
                    throw std::runtime_error("invalid function parameter");

                int offset = -4 * (variables.size() + 1);
                variables[parameter->name] = {offset, false};

                if (i < 4)
                {
                    emit("    str w" + std::to_string(i) +
                         ", [x29, #" + std::to_string(offset) + "]");
                }
            }

            emitStatements(function.body);

            emit("    mov w0, #0");
            emit(returnLabel + ":");
            emit("    mov sp, x29");
            emit("    ldp x29, x30, [sp], #16");
            emit("    ret");
        }

        int countDeclarations(const std::vector<ASTNode *> &statements) const
        {
            int count = 0;

            for (const ASTNode *node : statements)
            {
                if (dynamic_cast<const DeclarationNode *>(node) != nullptr)
                    ++count;

                if (const auto *ifNode = dynamic_cast<const IfNode *>(node))
                {
                    count += countDeclarations(ifNode->thenBranch);
                    count += countDeclarations(ifNode->elseBranch);
                }
                else if (const auto *whileNode =
                             dynamic_cast<const WhileNode *>(node))
                {
                    count += countDeclarations(whileNode->body);
                }
                else if (const auto *forNode =
                             dynamic_cast<const ForNode *>(node))
                {
                    count += countDeclarations(forNode->initialization);
                    count += countDeclarations(forNode->increment);
                    count += countDeclarations(forNode->body);
                }
            }

            return count;
        }

        void emitStatements(const std::vector<ASTNode *> &statements)
        {
            for (const ASTNode *node : statements)
            {
                if (const auto *declaration =
                        dynamic_cast<const DeclarationNode *>(node))
                {
                    emitDeclaration(*declaration);
                }
                else if (const auto *assignment =
                             dynamic_cast<const AssignmentNode *>(node))
                {
                    emitAssignment(*assignment);
                }
                else if (const auto *returnNode =
                             dynamic_cast<const ReturnNode *>(node))
                {
                    emitReturn(*returnNode);
                }
                else if (const auto *ifNode =
                             dynamic_cast<const IfNode *>(node))
                {
                    emitIf(*ifNode);
                }
                else if (const auto *whileNode =
                             dynamic_cast<const WhileNode *>(node))
                {
                    emitWhile(*whileNode);
                }
                else if (const auto *forNode =
                             dynamic_cast<const ForNode *>(node))
                {
                    emitFor(*forNode);
                }
            }
        }

        void emitDeclaration(const DeclarationNode &declaration)
        {
            if (variables.find(declaration.name) == variables.end())
            {
                int offset = -4 * (variables.size() + 1);
                variables[declaration.name] = {offset, false};
            }

            if (!declaration.initializer.empty())
            {
                emitExpression(declaration.initializer.front());
                emitStore(variables.at(declaration.name), "w0");
            }
        }

        void emitAssignment(const AssignmentNode &assignment)
        {
            if (assignment.expression.empty())
                return;

            emitExpression(assignment.expression.front());
            emitStore(lookup(assignment.variableName), "w0");
        }

        void emitReturn(const ReturnNode &returnNode)
        {
            if (!returnNode.expression.empty())
            {
                emitExpression(returnNode.expression.front());
            }
            else
            {
                emit("    mov w0, #0");
            }

            emit("    b " + returnLabel);
        }

        void emitIf(const IfNode &node)
        {
            const std::string falseLabel = newLabel("if_false_");
            const std::string endLabel = newLabel("if_end_");

            emitCondition(node.condition, falseLabel);

            emitStatements(node.thenBranch);

            if (!node.elseBranch.empty())
            {
                emit("    b " + endLabel);
                emit(falseLabel + ":");

                emitStatements(node.elseBranch);

                emit(endLabel + ":");
            }
            else
            {
                emit(falseLabel + ":");
            }
        }

        void emitWhile(const WhileNode &node)
        {
            const std::string condition = newLabel("while_");
            const std::string end = newLabel("while_end_");

            emit(condition + ":");

            emitCondition(node.condition, end);
            emitStatements(node.body);

            emit("    b " + condition);
            emit(end + ":");
        }

        void emitFor(const ForNode &node)
        {
            emitStatements(node.initialization);

            const std::string condition = newLabel("for_");
            const std::string end = newLabel("for_end_");

            emit(condition + ":");

            emitCondition(node.condition, end);
            emitStatements(node.body);
            emitStatements(node.increment);

            emit("    b " + condition);
            emit(end + ":");
        }

        void emitCondition(
            const std::vector<ASTNode *> &condition,
            const std::string &falseLabel)
        {
            if (condition.empty())
                return;

            emitExpression(condition.front());

            emit("    cmp w0, #0");
            emit("    beq " + falseLabel);
        }

        void emitExpression(const ASTNode *node)
        {
            if (node == nullptr)
                throw std::runtime_error("null expression");

            if (const auto *expression =
                    dynamic_cast<const ExpressionNode *>(node))
            {
                if (expression->type == "IDENTIFIER")
                {
                    emitLoad(lookup(expression->value), "w0");
                }
                else
                {
                    emit("    mov w0, #" + expression->value);
                }

                return;
            }

            if (node->type == "FUNCTION_CALL")
            {
                emitFunctionCall(*node);
                return;
            }

            if (node->children.size() == 2)
            {
                emitExpression(node->children[0]);
                emit("    str w0, [sp, #-16]!");

                emitExpression(node->children[1]);
                emit("    mov w1, w0");

                emit("    ldr w0, [sp], #16");

                emitBinaryOperator(node->type);
                return;
            }

            throw std::runtime_error(
                "unsupported expression: " + node->type);
        }

        void emitFunctionCall(const ASTNode &node)
        {
            if (node.children.empty())
                throw std::runtime_error("function call has no name");

            const auto *name =
                dynamic_cast<const ExpressionNode *>(
                    node.children.front());

            if (name == nullptr)
                throw std::runtime_error(
                    "function call has invalid name");

            for (size_t i = 1; i < node.children.size() && i <= 4; ++i)
            {
                emitExpression(node.children[i]);

                if (i - 1 != 0)
                {
                    emit("    mov w" +
                         std::to_string(i - 1) +
                         ", w0");
                }
            }

            emit("    bl " + name->value);
        }

        void emitBinaryOperator(const std::string &type)
        {
            if (type == "ADDITION")
                emit("    add w0, w0, w1");

            else if (type == "SUBTRACTION")
                emit("    sub w0, w0, w1");

            else if (type == "MULTIPLICATION")
                emit("    mul w0, w0, w1");

            else if (type == "DIVISION")
                emit("    sdiv w0, w0, w1");

            else if (type == "BITWISE_AND")
                emit("    and w0, w0, w1");

            else if (type == "BITWISE_OR")
                emit("    orr w0, w0, w1");

            else if (type == "LESS_THAN" ||
                     type == "LESS_THAN_OR_EQUAL" ||
                     type == "GREATER_THAN" ||
                     type == "GREATER_THAN_OR_EQUAL" ||
                     type == "EQUAL" ||
                     type == "NOT_EQUAL")
            {
                emitComparison(type);
            }

            else
            {
                throw std::runtime_error(
                    "unsupported operator: " + type);
            }
        }

        void emitComparison(const std::string &type)
        {
            const std::string trueLabel = newLabel("true_");
            const std::string endLabel = newLabel("compare_end_");

            emit("    cmp w0, w1");
            emit("    mov w0, #0");

            std::string condition;

            if (type == "LESS_THAN")
                condition = "lt";
            else if (type == "LESS_THAN_OR_EQUAL")
                condition = "le";
            else if (type == "GREATER_THAN")
                condition = "gt";
            else if (type == "GREATER_THAN_OR_EQUAL")
                condition = "ge";
            else if (type == "EQUAL")
                condition = "eq";
            else
                condition = "ne";

            emit("    b" + condition + " " + trueLabel);
            emit("    b " + endLabel);

            emit(trueLabel + ":");
            emit("    mov w0, #1");

            emit(endLabel + ":");
        }

        Variable lookup(const std::string &name) const
        {
            auto local = variables.find(name);

            if (local != variables.end())
                return local->second;

            auto global = globals.find(name);

            if (global != globals.end())
                return {0, true};

            throw std::runtime_error(
                "unknown variable: " + name);
        }

        void emitLoad(
            const Variable &variable,
            const std::string &reg)
        {
            if (variable.global)
            {
                emit("    ldr x3, =" + variableName(variable));
                emit("    ldr " + reg + ", [x3]");
            }
            else
            {
                emit("    ldr " + reg +
                     ", [x29, #" +
                     std::to_string(variable.offset) +
                     "]");
            }
        }

        void emitStore(
            const Variable &variable,
            const std::string &reg)
        {
            if (variable.global)
            {
                emit("    ldr x3, =" + variableName(variable));
                emit("    str " + reg + ", [x3]");
            }
            else
            {
                emit("    str " + reg +
                     ", [x29, #" +
                     std::to_string(variable.offset) +
                     "]");
            }
        }

        std::string variableName(const Variable &variable) const
        {
            for (const auto &[name, value] : globals)
            {
                if (value != nullptr &&
                    value->name == name)
                {
                    return name;
                }
            }

            throw std::runtime_error("invalid global variable");
        }
    };

}

std::string ARMGenerator::generate(const ProgramNode &program)
{
    return Emitter().generate(program);
}

void ARMGenerator::generate(
    const ProgramNode &program,
    std::ostream &output)
{
    output << generate(program);
}
