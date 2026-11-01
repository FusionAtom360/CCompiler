#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "./lexer/lexer.h"
#include "./parser/parser.h"
#include "./generator/generator.h"

namespace
{
    std::string readFileContents(const std::string &path)
    {
        std::ifstream inputFile(path);
        if (!inputFile)
        {
            throw std::runtime_error("could not open input file: " + path);
        }

        return std::string(
            (std::istreambuf_iterator<char>(inputFile)),
            std::istreambuf_iterator<char>());
    }

    void printTokens(const std::vector<Token> &tokens)
    {
        for (const auto &token : tokens)
        {
            std::cout << "Token: " << token.getValue() << "\t\tType: " << token.getType() << std::endl;
        }

        std::cout << "All token values: ";
        for (const auto &token : tokens)
        {
            std::cout << token.getValue() << " ";
        }
        std::cout << std::endl;
    }

    void printUsage(const char *programName)
    {
        std::cout << "Usage: " << programName << " [input-file] [output-file] [--debug]" << std::endl;
    }
}

int main(int argc, char *argv[])
{
    const std::string defaultInputPath = "C:/Users/Joseph/Desktop/Code/CCompiler2/src/tests/test-5.c";
    const std::string defaultOutputPath = "C:/Users/Joseph/Desktop/Code/CCompiler2/output.s";

    std::string inputPath = defaultInputPath;
    std::string outputPath = defaultOutputPath;
    bool debug = true;

    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];

        if (arg == "--help" || arg == "-h")
        {
            printUsage(argv[0]);
            return 0;
        }

        if (arg == "--debug")
        {
            debug = true;
            continue;
        }

        if (arg == "-o" && i + 1 < argc)
        {
            outputPath = argv[++i];
            continue;
        }

        if (inputPath == defaultInputPath)
        {
            inputPath = arg;
            continue;
        }

        outputPath = arg;
    }

    try
    {
        const std::string source = readFileContents(inputPath);
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        if (debug)
        {
            printTokens(tokens);
        }

        Parser parser(tokens);
        ProgramNode *ast = parser.parseProgram();
        if (ast == nullptr)
        {
            std::cerr << "Error: parsing failed." << std::endl;
            return 1;
        }

        if (debug)
        {
            std::cout << "AST:\n";
            printAST(ast, 0);
        }

        std::ofstream assemblyFile(outputPath);
        if (!assemblyFile)
        {
            std::cerr << "Error: could not open assembly output file: " << outputPath << std::endl;
            return 1;
        }

        ARMGenerator generator;
        generator.generate(*ast, assemblyFile);
        return 0;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}
