#pragma once

#include <iosfwd>
#include <string>

#include "../parser/parser.h"

class ARMGenerator
{
public:
    std::string generate(const ProgramNode &program);
    void generate(const ProgramNode &program, std::ostream &output);
};
