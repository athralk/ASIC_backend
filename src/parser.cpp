#include "../include/parser.hpp"
#include "../include/gate_forest.hpp"
#include <stdexcept>
#include <memory>

Token VerilogParser::peek() const { 
    if (index >= tokens.size()) {
        return {TokenType::END_OF_FILE, "", 0};
    }
    return tokens[index]; 
}

Token VerilogParser::advance() { return tokens[index++]; }

bool VerilogParser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

VerilogParser::VerilogParser(std::vector<Token> t) : tokens(std::move(t)) {}

// Forward declarations for recursive descent AST building
std::shared_ptr<GateNode> parseExpression(const std::vector<Token>& tokens, size_t& idx, int& id_counter, const VerilogParser& parser);

//recursive for not
std::shared_ptr<GateNode> parsePrimary(const std::vector<Token>& tokens, size_t& idx, int& id_counter, const VerilogParser& parser) {
    if (idx >= tokens.size()) return nullptr;
    Token t = tokens[idx];
    if (t.value == "(") {
        idx++; 
        auto node = parseExpression(tokens, idx, id_counter, parser);
        if (idx < tokens.size() && tokens[idx].value == ")") idx++; 
        return node;
    } else if (t.value == "~") {
        idx++; 
        auto operand = parsePrimary(tokens, idx, id_counter, parser);
        auto notNode = std::make_shared<GateNode>(id_counter++, GateType::NOT, "not");
        if (operand) notNode->addFanin(operand);
        return notNode;
    } else {
        idx++; 
        return std::make_shared<GateNode>(id_counter++, GateType::INPUT, t.value);
    }
}
//recursive for and
std::shared_ptr<GateNode> parseTerm(const std::vector<Token>& tokens, size_t& idx, int& id_counter, const VerilogParser& parser) {
    auto left = parsePrimary(tokens, idx, id_counter, parser);
    while (idx < tokens.size() && tokens[idx].value == "&") {
        idx++; 
        auto right = parsePrimary(tokens, idx, id_counter, parser);
        auto andNode = std::make_shared<GateNode>(id_counter++, GateType::AND, "and");
        if (left) andNode->addFanin(left);
        if (right) andNode->addFanin(right);
        left = andNode;
    }
    return left;
}
//recursive for xor
std::shared_ptr<GateNode> parseXor(const std::vector<Token>& tokens, size_t& idx, int& id_counter, const VerilogParser& parser) {
    auto left = parseTerm(tokens, idx, id_counter, parser);
    while (idx < tokens.size() && tokens[idx].value == "^") {
        idx++; // consume '^'
        auto right = parseTerm(tokens, idx, id_counter, parser);
        auto xorNode = std::make_shared<GateNode>(id_counter++, GateType::XOR, "xor");
        if (left) xorNode->addFanin(left);
        if (right) xorNode->addFanin(right);
        left = xorNode;
    }
    return left;
}
//recursive for or
std::shared_ptr<GateNode> parseExpression(const std::vector<Token>& tokens, size_t& idx, int& id_counter, const VerilogParser& parser) {
    auto left = parseXor(tokens, idx, id_counter, parser);
    while (idx < tokens.size() && tokens[idx].value == "|") {
        idx++; 
        auto right = parseXor(tokens, idx, id_counter, parser);
        auto orNode = std::make_shared<GateNode>(id_counter++, GateType::OR, "or");
        if (left) orNode->addFanin(left);
        if (right) orNode->addFanin(right);
        left = orNode;
    }
    return left;
}

GateForest VerilogParser::parse() {
    GateForest forest;
    int next_id = 0;

    while (peek().type != TokenType::END_OF_FILE) {
        if (match(TokenType::ASSIGN)) {
            Token lhs = advance(); 
            if (peek().value == "=") advance(); 

            auto rhs_root = parseExpression(tokens, index, next_id, *this);

            auto assignNode = std::make_shared<GateNode>(next_id++, GateType::ASSIGN, lhs.value);
            if (rhs_root) assignNode->addFanin(rhs_root);

            if (peek().value == ";") advance(); 
            forest.push_back(assignNode);
        } else {
            advance();
        }
    }
    return forest;
}