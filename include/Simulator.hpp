
#pragma once
#include <iostream>
#include <memory>
#include <random>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include "Configuration.hpp"

class CompUnit;
class Simulator;

/**
 * Holds the instruction pointer and temporary registers
 * within an execution frame.
 */
class ExecutionFrame : public std::enable_shared_from_this<ExecutionFrame> {
public:
    /** instruction pointer */
    int ip;

    /** return value from this frame */
    int ret;

private:
    /** local registers (register name -> value) */
    std::unordered_map<std::string, int> regs;
    Simulator& simulator;

public:
    ExecutionFrame(int ip, Simulator& simulator);

    /**
     * Fetch the value at the given register
     * @param tempName name of the register
     * @return the value at the given register
     */
    int get(const std::string& tempName);

    /**
     * Store a value into the given register
     * @param tempName name of the register
     * @param value value to be stored
     */
    void put(const std::string& tempName, int value);

    /**
     * Advance the instruction pointer. Since we're dealing with a tree,
     * this is postorder traversal, one step at a time, modulo jumps.
     */
    bool advance();

    void setIP(int ip);

    std::shared_ptr<Node> getCurrentInsn();
};

class StackItem {
public:
    enum class Kind {
        COMPUTED, MEM, TEMP, NAME
    };

    Kind type;
    int value;
    int addr;
    std::string temp;
    std::string name;

    StackItem(int value);

    StackItem(int value, int addr);

    StackItem(Kind type, int value, const std::string& string);
};

/**
 * While traversing the IR tree, we require a stack in order to hold
 * a number of single-word values (e.g., to evaluate binary expressions).
 * This also keeps track of whether a value was created by a TEMP
 * or MEM, or NAME reference, which is useful when executing moves.
 */
class ExprStack {
private:
    std::stack<std::shared_ptr<StackItem>> stack;

public:
    int popValue();

    std::shared_ptr<StackItem> pop();

    void pushAddr(int value, int addr);

    void pushTemp(int value, const std::string& temp);

    void pushName(int value, const std::string& name);

    void pushValue(int value);
};

/**
 * A simple IR interpreter
 */
class Simulator {
private:
    /** compilation unit to be interpreted */
    std::shared_ptr<CompUnit> compUnit;
    /** map from labeled name to address */
    std::unordered_map<std::string, int> nameToIndex;
    /** heap */
    std::vector<int> mem;
    /** heap size maximum **/
    int heapSizeMax;
    std::shared_ptr<ExprStack> exprStack;
    std::set<std::string> libraryFunctions;

public:
    constexpr static int DEFAULT_HEAP_SIZE = 2048;
    /** map from address to instruction */
    std::unordered_map<int, std::shared_ptr<Node>> indexToInsn;

    /**
     * Construct an IR interpreter with a default heap size
     * @param compUnit the compilation unit to be interpreted
     */
    Simulator(std::shared_ptr<CompUnit> compUnit);

    /**
     * Construct an IR interpreter
     * @param compUnit the compilation unit to be interpreted
     * @param heapSize the heap size
     */
    Simulator(std::shared_ptr<CompUnit> compUnit, int heapSize);

    /**
     * Allocate a specified amount of bytes on the heap
     * @param size the number of bytes to be allocated
     * @return the starting address of the allocated region on the heap
     */
    int malloc(int size);

    /**
     * Allocate a specified amount of bytes on the heap and initialize them to 0.
     * @param size the number of bytes to be allocated
     * @return the starting address of the allocated region on the heap
     */
    int calloc(int size);

    /**
     * Read a value at the specified location on the heap
     * @param addr the address to be read
     * @return the value at {@code addr}
     */
    int read(int addr);

    /**
     * Write a value at the specified location on the heap
     * @param addr the address to be written
     * @param value the value to be written
     */
    void store(int addr, int value);

    int getMemoryIndex(int addr);

    /**
     * Simulate a function call
     * All arguments to the function call are passed via registers with prefix
     * {@link Configuration#ABSTRACT_ARG_PREFIX} and indices starting from 0.
     * @param name name of the function call
     * @param args arguments to the function call
     * @return the value that would be in register
     *          {@link Configuration#ABSTRACT_RET}
     */
    template<typename... Args>
    int call(const std::string& name, Args... args) {
        std::vector<int> arguments = {args...};
        return call(std::make_shared<ExecutionFrame>(-1, *this), name, arguments);
    }

    /**
     * Simulate a function call.
     * All arguments to the function call are passed via registers with prefix
     * {@link Configuration#ABSTRACT_ARG_PREFIX} and indices starting from 0.
     * The function call should return the result via register
     * {@link Configuration#ABSTRACT_RET}.
     * @param parent parent call frame to write _RET values to
     * @param name name of the function call
     * @param args arguments to the function call
     * @return the value of register
     *          {@link Configuration#ABSTRACT_RET}
     */
    int call(std::shared_ptr<ExecutionFrame> parent, const std::string& name, std::vector<int>& args);

    /**
     * Simulate a library function call, returning the list of returned values
     * @param name name of the function call
     * @param args arguments to the function call, which may include
     *          the pointer to the location of multiple results
     */
    int libraryCall(const std::string& name, std::vector<int>& args);

    void leave(std::shared_ptr<ExecutionFrame> frame);
    /**
     *
     * @param name name of the label
     * @return the IR node at the named label
     */
    int findLabel(const std::string& name);
};
