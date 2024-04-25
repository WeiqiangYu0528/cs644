CXX=g++
CXXFLAGS=-Iinclude -Wall -std=c++20
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
# STD_LIB=/u/cs444/pub/stdlib/6.1
STD_LIB=JSL_6.1
BENCH_DIR=benchmarks/opt-reg-only
OUTPUT_DIR=output

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


bench: $(TARGET)
	@echo "" > benchmarks/results.csv
	@for bench_file in $(BENCH_DIR)/*.java; do \
		benchmark_name=$$(basename $$bench_file); \
		echo "Running with optimization for $$benchmark_name..."; \
		rm -rf $(OUTPUT_DIR)/*; \
		files=""; \
		for std_file in $$(find $(STD_LIB) -type f -name '*.java'); do \
			files="$$files $$std_file "; \
		done; \
		./$(TARGET) $$bench_file $$files; \
		cp $(STD_LIB)/runtime.s $(OUTPUT_DIR)/; \
		for file in $(OUTPUT_DIR)/*.s; do \
			nasm -O1 -f elf -g -F dwarf $$file; \
		done; \
		ld -melf_i386 -o $(OUTPUT_DIR)/main_opt $(OUTPUT_DIR)/*.o; \
		time_opt=$$({ time -p $(OUTPUT_DIR)/main_opt; } 2>&1 | grep real | awk '{print $$2 * 1000}'); \
		echo "Running without optimization for $$benchmark_name..."; \
		rm -rf $(OUTPUT_DIR)/*; \
		./$(TARGET) --opt-none $$bench_file $$files; \
		cp $(STD_LIB)/runtime.s $(OUTPUT_DIR)/; \
		for file in $(OUTPUT_DIR)/*.s; do \
			nasm -O1 -f elf -g -F dwarf $$file; \
		done; \
		ld -melf_i386 -o $(OUTPUT_DIR)/main_no_opt $(OUTPUT_DIR)/*.o; \
		time_no_opt=$$({ time -p $(OUTPUT_DIR)/main_no_opt; } 2>&1 | grep real | awk '{print $$2 * 1000}'); \
		speedup_ratio=$$((100000 * time_no_opt / time_opt)); \
		speedup=$$(echo $$speedup_ratio | awk '{print $$1/100000}'); \
		echo "opt-reg-only, $$benchmark_name, time in $${time_opt}ms w/ opt, time in $${time_no_opt}ms w/o opt, $${speedup}x speedup"; \
		echo "opt-reg-only, $$benchmark_name, time in $${time_opt}ms w/ opt, time in $${time_no_opt}ms w/o opt, $${speedup}x speedup" >> benchmarks/results.csv ; \
	done

.PHONY: all clean parser bench