CXX=g++
CXXFLAGS=-Iinclude -Wall -std=c++20 -g
LIBS=

BISON=bison
BISON_SRC=src/parser/parser.yy
BISON_OUT=src/parser/parser.tab.cpp
BISON_HEADER=include/parser.h
BISON_LOCATION_HEADER=include/location.h
BISON_OPTS=-d

FLEX=flex
FLEX_SRC=src/lexer/lexer.ll
FLEX_OUT=src/lexer/lex.yy.cpp
FLEX_OPTS=

AST_OUT=src/ast/ast.cpp
VISITOR_OUT=src/ast/visitor.cpp

SRC_FILES=$(wildcard src/*.cpp) $(wildcard src/ast/*.cpp) $(BISON_OUT) $(FLEX_OUT)

BUILD_DIR=.

TARGET=$(BUILD_DIR)/joosc

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(BISON_OUT): $(BISON_SRC)
	$(BISON) $(BISON_OPTS) -o $@ $<

$(FLEX_OUT): $(FLEX_SRC) $(BISON_HEADER)
	$(FLEX) $(FLEX_OPTS) -o $@ $<

clean:
	rm $(BUILD_DIR)/joosc $(BISON_OUT) $(BISON_HEADER) $(BISON_LOCATION_HEADER) $(FLEX_OUT)

parser: $(BISON_OUT)

.PHONY: all clean parser
