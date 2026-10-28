#include "../include/file_reader.hpp"
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/mapper.hpp"
#include "../include/optimizer.hpp"
#include <iostream>
#include <memory>

void printGate(const std::shared_ptr<GateNode>& node, int depth = 0) {
    if (!node) return;
    
    std::string indent(depth * 2, ' ');
    std::string typeStr;
    
    switch (node->type) {
    case GateType::INPUT:  typeStr = "INPUT (" + node->name + ")"; break;
    case GateType::OUTPUT: typeStr = "OUTPUT (" + node->name + ")"; break;
    case GateType::ASSIGN: typeStr = "ASSIGN (" + node->name + ")"; break;
    case GateType::AND:    typeStr = "AND"; break;
    case GateType::OR:     typeStr = "OR"; break;
    case GateType::NOT:    typeStr = "NOT"; break;
    case GateType::NAND:   typeStr = "NAND"; break;
    case GateType::NOR:    typeStr = "NOR"; break;
    case GateType::XOR:    typeStr = "XOR"; break;
    case GateType::XNOR:   typeStr = "XNOR"; break;
    default:               typeStr = "GATE"; break;
}

    std::cout << indent << "- " << typeStr << " [id: " << node->id << "]\n";
    
    for (const auto& fanin : node->inputs) {
        printGate(fanin, depth + 1);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./verilog_compiler <source.v>\n";
        return 1;
    }

    try {
        std::string source = readVerilogFile(argv[1]);
        VerilogLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        VerilogParser parser(tokens);
        GateForest forest = parser.parse();

        std::cout << "\n=== Original AST Gate Forest ===\n";
        for (size_t i = 0; i < forest.size(); ++i) {
            std::cout << "Root [" << i << "]:\n";
            printGate(forest[i], 1);
        }

        // Run Technology Mapping to NAND + Inverter Form
        GateForest mappedForest = TechnologyMapper::mapToNandInverter(forest);

        std::cout << "\n=== Mapped Canonical NAND + Inverter Netlist ===\n";
        for (size_t i = 0; i < mappedForest.size(); ++i) {
            std::cout << "Mapped Root [" << i << "]:\n";
            printGate(mappedForest[i], 1);
        }

    } catch (const std::exception& e) {
        std::cerr << "Compilation error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}