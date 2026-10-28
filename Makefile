CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra

SRC = src/main.cpp src/file_reader.cpp src/lexer.cpp src/parser.cpp src/mapper.cpp src/optimizer.cpp
TARGET = output_test

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)