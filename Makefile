# Owner: M3
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Iinclude
TARGET   := bin/registration

SRCS := $(shell find src -name '*.cpp')
OBJS := $(patsubst src/%.cpp,build/%.o,$(SRCS))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build bin
