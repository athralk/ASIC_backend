#pragma once
#include <string>
#include <vector>

enum class TokenType {
    MODULE, ENDMODULE, INPUT, OUTPUT, WIRE, ASSIGN,
    IDENTIFIER, SYMBOL, END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
};

class VerilogLexer {
    std::string src;
    size_t pos = 0;
    size_t line = 1;

    char peek() const;
    char advance();
    void skipWhitespaceAndComments();

public:
    explicit VerilogLexer(std::string source);
    std::vector<Token> tokenize();
};