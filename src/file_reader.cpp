#include "../include/file_reader.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string readVerilogFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open Verilog file: " + filepath);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}