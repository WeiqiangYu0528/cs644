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

OBJ_DIR=build
SRC_DIRS=src src/ast src/tir-c++
SRC_FILES=$(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJ_FILES=$(SRC_FILES:src/%.cpp=$(OBJ_DIR)/%.o) $(OBJ_DIR)/parser/parser.tab.o $(OBJ_DIR)/lexer/lex.yy.o

TARGET_DIR=.
TARGET=$(TARGET_DIR)/joosc

all: $(TARGET)


DIRS := $(OBJ_DIR) $(OBJ_DIR)/parser $(OBJ_DIR)/lexer $(OBJ_DIR)/ast $(OBJ_DIR)/tir-c++

$(DIRS):
	mkdir -p $(DIRS)

$(OBJ_FILES): | $(DIRS)

$(OBJ_DIR)/%.o: src/%.cpp $(BISON_HEADER)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/parser/parser.tab.o: $(BISON_OUT)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/lexer/lex.yy.o: $(FLEX_OUT)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BISON_OUT) $(BISON_HEADER): $(BISON_SRC)
	$(BISON) $(BISON_OPTS) -o $(BISON_OUT) $<

$(FLEX_OUT): $(FLEX_SRC) $(BISON_HEADER)
	$(FLEX) $(FLEX_OPTS) -o $@ $<

$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf $(TARGET_DIR)/joosc $(OBJ_DIR) $(BISON_OUT) $(BISON_HEADER) $(BISON_LOCATION_HEADER) $(FLEX_OUT)

parser: $(BISON_OUT) $(FLEX_OUT)

.PHONY: all clean parser
