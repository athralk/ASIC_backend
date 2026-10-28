#pragma once
#include "gate_forest.hpp"

class TechnologyMapper {
public:
    // Transforms an arbitrary GateForest DAG into a canonical NAND + Inverter form
    static GateForest mapToNandInverter(const GateForest& forest);
};