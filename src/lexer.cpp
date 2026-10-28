#include "../include/lexer.hpp"
#include <cctype>

char VerilogLexer::peek() const { return pos < src.size() ? src[pos] : '\0'; }
char VerilogLexer::advance() {
    char c = peek();
    if (c == '\n') line++;
    pos++;
    return c;
}

void VerilogLexer::skipWhitespaceAndComments() {
    while (pos < src.size()) {
        char c = peek();
        if (std::isspace(c)) {
            advance();
        } else if (c == '/' && pos + 1 < src.size() && src[pos + 1] == '/') {
            while (peek() != '\n' && peek() != '\0') advance();
        } else {
            break;
        }
    }
}

VerilogLexer::VerilogLexer(std::string source) : src(std::move(source)) {}

std::vector<Token> VerilogLexer::tokenize() {
    std::vector<Token> tokens;
    while (pos < src.size()) {
        skipWhitespaceAndComments();
        if (pos >= src.size()) break;
        char c = peek();

        if (std::isalpha(c) || c == '_') {
            size_t start = pos;
            while (std::isalnum(peek()) || peek() == '_' || peek() == '$') advance();
            std::string word = src.substr(start, pos - start);
            TokenType type = TokenType::IDENTIFIER;
            if (word == "module") type = TokenType::MODULE;
            else if (word == "endmodule") type = TokenType::ENDMODULE;
            else if (word == "input") type = TokenType::INPUT;
            else if (word == "output") type = TokenType::OUTPUT;
            else if (word == "wire") type = TokenType::WIRE;
            else if (word == "assign") type = TokenType::ASSIGN;
            tokens.push_back({type, word, line});
            continue;
        }

        if (std::string("=;,()&|^~").find(c) != std::string::npos) {
            tokens.push_back({TokenType::SYMBOL, std::string(1, c), line});
            advance();
            continue;
        }
        advance();
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;
}