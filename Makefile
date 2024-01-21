CXX=g++
CXXFLAGS=-Iinclude -Isrc/parser -Isrc/lexer -Wall -std=c++11
LIBS=

BISON=bison
BISON_SRC=src/parser/parser.yy
BISON_OUT=src/parser/parser.tab.cpp
BISON_HEADER=src/parser/parser.tab.hpp
BISON_OPTS=-d 

FLEX=flex
FLEX_SRC=src/lexer/lexer.ll
FLEX_OUT=src/lexer/lex.yy.cpp
FLEX_OPTS=


SRC_FILES=$(wildcard src/*.cpp) $(BISON_OUT) $(FLEX_OUT)

BUILD_DIR=build

TARGET=$(BUILD_DIR)/compiler

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(BISON_OUT): $(BISON_SRC)
	$(BISON) $(BISON_OPTS) -o $@ $<

$(FLEX_OUT): $(FLEX_SRC) $(BISON_HEADER)
	$(FLEX) $(FLEX_OPTS) -o $@ $<

clean:
	rm -rf $(BUILD_DIR) $(BISON_OUT) $(BISON_HEADER) $(FLEX_OUT)

parser: $(BISON_OUT)

.PHONY: all clean parser