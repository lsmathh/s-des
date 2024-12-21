# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Targets and sources
TARGET = sdes
SRC = main.cpp sdes.cpp sdes_demo.cpp
HEADERS = sdes.h sdes_demo.h

# Rules
all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
