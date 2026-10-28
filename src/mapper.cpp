#include "../include/mapper.hpp"
#include <memory>

// Recursive helper function to rewrite gates into NAND and NOT primitives
std::shared_ptr<GateNode> canonicalizeNode(const std::shared_ptr<GateNode>& node, int& id_counter) {
    if (!node) return nullptr;

    // Pass through inputs/constants directly
    if (node->type == GateType::INPUT || node->type == GateType::CONST0 || node->type == GateType::CONST1) {
        return std::make_shared<GateNode>(id_counter++, node->type, node->name);
    }

    // Pass through structural roots (ASSIGN / OUTPUT)
    if (node->type == GateType::ASSIGN || node->type == GateType::OUTPUT) {
        auto newNode = std::make_shared<GateNode>(id_counter++, node->type, node->name);
        if (!node->inputs.empty()) {
            newNode->addFanin(canonicalizeNode(node->inputs[0], id_counter));
        }
        return newNode;
    }

    // Inverter remains an Inverter (NOT)
    if (node->type == GateType::NOT) {
        auto operand = canonicalizeNode(node->inputs[0], id_counter);
        auto notNode = std::make_shared<GateNode>(id_counter++, GateType::NOT, "not");
        if (operand) notNode->addFanin(operand);
        return notNode;
    }

    // Rewrite AND(A, B) into NOT(NAND(A, B))
    if (node->type == GateType::AND) {
        auto left = canonicalizeNode(node->inputs[0], id_counter);
        auto right = canonicalizeNode(node->inputs[1], id_counter);

        auto nandNode = std::make_shared<GateNode>(id_counter++, GateType::NAND, "nand");
        if (left) nandNode->addFanin(left);
        if (right) nandNode->addFanin(right);

        auto notNode = std::make_shared<GateNode>(id_counter++, GateType::NOT, "not");
        notNode->addFanin(nandNode);
        return notNode;
    }

    // Rewrite OR(A, B) into NAND(NOT(A), NOT(B)) using De Morgan's Law
    if (node->type == GateType::OR) {
        auto left = canonicalizeNode(node->inputs[0], id_counter);
        auto right = canonicalizeNode(node->inputs[1], id_counter);

        auto notA = std::make_shared<GateNode>(id_counter++, GateType::NOT, "not");
        if (left) notA->addFanin(left);

        auto notB = std::make_shared<GateNode>(id_counter++, GateType::NOT, "not");
        if (right) notB->addFanin(right);

        auto nandNode = std::make_shared<GateNode>(id_counter++, GateType::NAND, "nand");
        nandNode->addFanin(notA);
        nandNode->addFanin(notB);

        return nandNode;
    }
    if (node->type == GateType::XOR) {
    auto A = canonicalizeNode(node->inputs[0], id_counter);
    auto B = canonicalizeNode(node->inputs[1], id_counter);

    // 4-NAND implementation of XOR:
    // T1 = NAND(A, B)
    auto t1 = std::make_shared<GateNode>(id_counter++, GateType::NAND, "nand");
    if (A) t1->addFanin(A);
    if (B) t1->addFanin(B);

    // T2 = NAND(A, T1)
    auto t2 = std::make_shared<GateNode>(id_counter++, GateType::NAND, "nand");
    if (A) t2->addFanin(A);
    t2->addFanin(t1);

    // T3 = NAND(B, T1)
    auto t3 = std::make_shared<GateNode>(id_counter++, GateType::NAND, "nand");
    if (B) t3->addFanin(B);
    t3->addFanin(t1);

    // Final = NAND(T2, T3)
    auto xorNand = std::make_shared<GateNode>(id_counter++, GateType::NAND, "nand");
    xorNand->addFanin(t2);
    xorNand->addFanin(t3);

    return xorNand;
    }

    // Fallback for any other node type
    return std::make_shared<GateNode>(id_counter++, node->type, node->name);
}

GateForest TechnologyMapper::mapToNandInverter(const GateForest& forest) {
    GateForest mappedForest;
    int id_counter = 5000; // Unique ID offset for mapped canonical nodes
    for (const auto& root : forest) {
        mappedForest.push_back(canonicalizeNode(root, id_counter));
    }
    return mappedForest;
}