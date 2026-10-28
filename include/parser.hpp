#pragma once
#include "lexer.hpp"
#include "gate_forest.hpp"
#include <vector>

class VerilogParser {
    std::vector<Token> tokens;
    size_t index = 0;

public:
    Token peek() const;
    Token advance();
    bool match(TokenType type);

    explicit VerilogParser(std::vector<Token> t);
    GateForest parse();
};