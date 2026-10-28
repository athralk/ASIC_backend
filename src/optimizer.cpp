#include "../include/optimizer.hpp"
#include <unordered_map>
#include <memory>

// Key for Structural Hashing (Strash)
struct NodeKey {
    GateType type;
    int left_id;
    int right_id;

    bool operator==(const NodeKey& other) const {
        return type == other.type && left_id == other.left_id && right_id == other.right_id;
    }
};

struct NodeKeyHash {
    std::size_t operator()(const NodeKey& k) const {
        return std::hash<int>()(static_cast<int>(k.type)) ^ 
               (std::hash<int>()(k.left_id) << 1) ^ 
               (std::hash<int>()(k.right_id) >> 1);
    }
};

class OptimizerPass {
    int next_id;
    std::unordered_map<NodeKey, std::shared_ptr<GateNode>, NodeKeyHash> strash_table;

public:
    OptimizerPass(int start_id) : next_id(start_id) {}

    std::shared_ptr<GateNode> optimizeNode(const std::shared_ptr<GateNode>& node) {
        if (!node) return nullptr;

        // Inputs and constants pass through
        if (node->type == GateType::INPUT || node->type == GateType::CONST0 || node->type == GateType::CONST1) {
            return std::make_shared<GateNode>(next_id++, node->type, node->name);
        }

        // Recursively optimize inputs bottom-up
        std::vector<std::shared_ptr<GateNode>> opt_inputs;
        for (const auto& fanin : node->inputs) {
            opt_inputs.push_back(optimizeNode(fanin));
        }

        // --- Peephole Optimization: Double Inversion Elimination (NOT(NOT(x)) -> x) ---
        if (node->type == GateType::NOT && !opt_inputs.empty() && opt_inputs[0]->type == GateType::NOT) {
            // Bypass both NOT gates and return the inner input directly
            return opt_inputs[0]->inputs[0];
        }

        // Build the candidate optimized node
        auto newNode = std::make_shared<GateNode>(next_id++, node->type, node->name);
        for (const auto& input : opt_inputs) {
            if (input) newNode->addFanin(input);
        }

        // --- Structural Hashing (Strash) ---
        int l_id = newNode->inputs.size() > 0 ? newNode->inputs[0]->id : -1;
        int r_id = newNode->inputs.size() > 1 ? newNode->inputs[1]->id : -1;

        if (newNode->type == GateType::NAND || newNode->type == GateType::AND || newNode->type == GateType::OR || newNode->type == GateType::NOT) {
            NodeKey key{newNode->type, l_id, r_id};
            if (strash_table.find(key) != strash_table.end()) {
                // Equivalent gate already exists in the graph; reuse it!
                return strash_table[key];
            }
            strash_table[key] = newNode;
        }

        return newNode;
    }
};

GateForest NetlistOptimizer::optimize(const GateForest& forest) {
    GateForest optimized_forest;
    int root_id_counter = 7000;

    for (const auto& root : forest) {
        if (!root) continue;
        auto opt_root = std::make_shared<GateNode>(root_id_counter++, root->type, root->name);
        if (!root->inputs.empty()) {
            OptimizerPass sub_pass(root_id_counter);
            auto optimized_expr = sub_pass.optimizeNode(root->inputs[0]);
            if (optimized_expr) opt_root->addFanin(optimized_expr);
        }
        optimized_forest.push_back(opt_root);
    }
    return optimized_forest;
}