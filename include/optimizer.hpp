#pragma once
#include "gate_forest.hpp"

class NetlistOptimizer {
public:
    // Optimizes and structurally hashes a GateForest to eliminate redundant nodes and double inversions
    static GateForest optimize(const GateForest& forest);
};