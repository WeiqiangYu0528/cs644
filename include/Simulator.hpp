
#pragma once
#include <iostream>
#include <memory>
#include <random>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include "CompUnit.hpp"
#include "Configuration.hpp"

class Simulator;

std::random_device rd;  // Obtain a random number from hardware
std::mt19937 gen(rd()); // Seed the generator
std::uniform_int_distribution<> distr(0, INT_MAX); // Define the range
int debugLevel = 0;

/**
 * Holds the instruction pointer and temporary registers
 * within an execution frame.
 */
class ExecutionFrame : public std::enable_shared_from_this<ExecutionFrame> {
protected:
    /** instruction pointer */
    int ip;

    /** return value from this frame */
    int ret;

private:
    /** local registers (register name -> value) */
    std::unordered_map<std::string, int> regs;
    Simulator& simulator;

public:
    ExecutionFrame(int ip, Simulator& simulator) : ip(ip), simulator(simulator), ret(distr(gen)) {
    }

    /**
     * Fetch the value at the given register
     * @param tempName name of the register
     * @return the value at the given register
     */
    int get(const std::string& tempName) {
        if (!regs.contains(tempName)) {
            /* Referencing a temp before having written to it - initialize
                with garbage */
            put(tempName, distr(gen));
        }
        return regs[tempName];
    }

    /**
     * Store a value into the given register
     * @param tempName name of the register
     * @param value value to be stored
     */
    void put(const std::string& tempName, int value) {
        regs[tempName] = value;
    }

    /**
     * Advance the instruction pointer. Since we're dealing with a tree,
     * this is postorder traversal, one step at a time, modulo jumps.
     */
    bool advance() {
        // Time out if necessary.
        // if (Thread.currentThread().isInterrupted()) return false;

        if (debugLevel > 1)
            std::cout << "Evaluating " << getCurrentInsn()->getLabel() << std::endl;
        int backupIP = ip;
        simulator.leave(shared_from_this());

        if (ip == -1) return false; /* RETURN */

        if (ip != backupIP) /* A jump was performed */
            return true;

        ip++;
        return true;
    }

    void setIP(int ip) {
        this->ip = ip;
        if (debugLevel > 1) {
            if (ip == -1)
                std::cout << "Returning" << std::endl;
            else std::cout << "Jumping to " << getCurrentInsn()->getLabel() << std::endl;
        }
    }

    std::shared_ptr<Node> getCurrentInsn() {
        if (!indexToInsn.contains(ip))
            throw std::runtime_error("No next instruction.  Forgot RETURN?");
        return indexToInsn[ip];
    }
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

    StackItem(int value) : type(Kind::COMPUTED), value(value) {
    }

    StackItem(int value, int addr) : type(Kind::MEM), value(value), addr(addr) {
    }

    StackItem(Kind type, int value, String string) : type(type), value(value) {
        if (type == Kind::TEMP)
            temp = string;
        else if (type == Kind::NAME)
            name = string;
        else
            throw std::runtime_error("Unknown StackItem Kind");
    }
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
    int popValue() {
        int value = stack.top()->value;
        stack.pop();
        if (debugLevel > 1) std::cout << "Popping value " << value << std::endl;
        return value;
    }

    std::shared_ptr<StackItem> pop() {
        std::shared_ptr<StackItem> item = stack.top();
        stack.pop();
        return item;
    }

    void pushAddr(int value, int addr) {
        if (debugLevel > 1)
            std::cout << "Pushing MEM " << value << " (" << addr << ")" << std::endl;
        stack.push(std::make_shared(value, addr));
    }

    void pushTemp(int value, const std::string& temp) {
        if (debugLevel > 1)
            std::cout << "Pushing TEMP " << value << " (" << temp << ")" << std::endl;
        stack.push(std::make_shared(StackItem::Kind::TEMP, value, temp));
    }

    void pushName(int value, const std::string& name) {
        if (debugLevel > 1)
            std::cout << "Pushing NAME " << value << " (" << name << ")" << std::endl;
        stack.push(std::make_shared(StackItem::Kind::NAME, value, name));
    }

    void pushValue(int value) {
        if (debugLevel > 1) std::cout << "Pushing value " << value << std::endl;
        stack.push(std::make_shared(value));
    }
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

protected:
    /** map from address to instruction */
    std::unordered_map<int, std::shared_ptr<Node>> indexToInsn;

public:
    constexpr int DEFAULT_HEAP_SIZE = 2048;

    /**
     * Construct an IR interpreter with a default heap size
     * @param compUnit the compilation unit to be interpreted
     */
    Simulator(std::shared_ptr<CompUnit> compUnit) : Simulator(compUnit, DEFAULT_HEAP_SIZE) {
    }

    /**
     * Construct an IR interpreter
     * @param compUnit the compilation unit to be interpreted
     * @param heapSize the heap size
     */
    Simulator(std::shared_ptr<CompUnit> compUnit, int heapSize) : compUnit(compUnit), heapSizeMax(heapSize), exprStack(std::make_shared<ExprStack>()) {
        libraryFunctions.insert("__malloc");
        libraryFunctions.insert("__debexit");
        libraryFunctions.insert("__exception");
        libraryFunctions.insert("NATIVEjava.io.OutputStream.nativeWrite");

        InsnMapsBuilder imb;
        // Do we need to reassign compunit??
        compUnit = std::dynamic_pointer_cast<CompUnit>(imb.visit(compUnit));
        indexToInsn = imb.indexToInsn();
        nameToIndex = imb.nameToIndex();
    }

    /**
     * Allocate a specified amount of bytes on the heap
     * @param size the number of bytes to be allocated
     * @return the starting address of the allocated region on the heap
     */
    int malloc(int size) {
        if (size < 0) throw std::runtime_error("Invalid size");
        if (size % Configuration::WORD_SIZE != 0)
            throw std::runtime_error("Can only allocate in chunks of "
                    + Configuration::WORD_SIZE + " bytes!");

        int retval = mem.size();
        if (retval + size > heapSizeMax) throw std::runtime_error("Out of heap!");
        for (int i = 0; i < size; i++) {
            mem.push_back(distr(gen));
        }
        return retval;
    }

    /**
     * Allocate a specified amount of bytes on the heap and initialize them to 0.
     * @param size the number of bytes to be allocated
     * @return the starting address of the allocated region on the heap
     */
    int calloc(int size) {
        int retval = malloc(size);
        for (int i = (int) retval; i < retval + size; i++) {
            mem[i] = 0;
        }
        return retval;
    }

    /**
     * Read a value at the specified location on the heap
     * @param addr the address to be read
     * @return the value at {@code addr}
     */
    int read(int addr) {
        int i = getMemoryIndex(addr);
        if (i >= mem.size())
            throw std::runtime_error("Attempting to read past end of heap");
        return mem[i];
    }

    /**
     * Write a value at the specified location on the heap
     * @param addr the address to be written
     * @param value the value to be written
     */
    void store(int addr, int value) {
        int i = getMemoryIndex(addr);
        if (i >= mem.size())
            throw std::runtime_error("Attempting to store past end of heap");
        mem[i] = value;
    }

    int getMemoryIndex(int addr) {
        if (addr % Configuration::WORD_SIZE != 0)
            throw std::runtime_error("Unaligned memory access: " + addr + " (word size="
                    + Configuration::WORD_SIZE + ")");
        return addr / Configuration::WORD_SIZE;
    }

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
    template<typename... Args>
    int call(std::shared_ptr<ExecutionFrame> parent, const std::string& name, std::vector<int>& args) {
        int ret;
        // Catch standard library calls.
        if (libraryFunctions.contains(name)) {
            ret = libraryCall(name, args);
        } else {
            std::shared_ptr<FuncDecl> fDecl = compUnit->getFunction(name);
            if (!fDecl)
                throw std::runtime_error("Tried to call an unknown function: '"
                        + name + "'");

            // Create a new stack frame.
            int ip = findLabel(name);
            std::shared_ptr<ExecutionFrame> frame = std::make_shared<ExecutionFrame>(ip, *this);

            // Pass the remaining arguments into registers.
            for (int i = 0; i < args.size(); ++i)
                frame->put(std::string(Configuration::ABSTRACT_ARG_PREFIX) + std::to_string(i), args[i]);

            // Simulate!
            while (frame->advance()) ;

            ret = frame->ret;
        }

        parent->put(Configuration::ABSTRACT_RET, ret);
        return ret;
    }

    /**
     * Simulate a library function call, returning the list of returned values
     * @param name name of the function call
     * @param args arguments to the function call, which may include
     *          the pointer to the location of multiple results
     */
    int libraryCall(const std::string& name, vector<int>& args) {
        int ws = Configuration::WORD_SIZE;
        int ret;
        if (name == "NATIVEjava.io.OutputStream.nativeWrite") {
            std::cout << static_cast<char>(args[0]) << std::endl;
            ret = 0;
        }
        else if (name == "__malloc") {
            ret = malloc(args[0]);
        }
        else if (name == "__debexit") {
            std::exit(args[0]);
            ret = args[0];
        }
        else if (name == "__exception") {
            std::exit(13);
            ret = 13;
        }
        else {
            throw std::runtime_error("Unsupported library function: "
                    + name);
        }
        return ret;
    }

    void leave(std::shared_ptr<ExecutionFrame> frame) {
        std::shared_ptr<Node> insn = frame->getCurrentInsn();
        if (auto temp = std::dynamic_pointer_cast<Const>(insn))
            exprStack->pushValue(temp->getValue());
        else if (auto temp = std::dynamic_pointer_cast<Temp>(insn)) {
            std::string tempName = temp->getName();
            exprStack->pushTemp(frame->get(tempName), tempName);
        }
        else if (auto temp = std::dynamic_pointer_cast<BinOp>(insn)) {
            int r = exprStack->popValue();
            int l = exprStack->popValue();
            int result;
            switch (temp->getOpType()) {
            using enum OpType;
            case ADD:
                result = l + r;
                break;
            case SUB:
                result = l - r;
                break;
            case MUL:
                result = l * r;
                break;
            case DIV:
                if (r == 0) throw std::runtime_error("Division by zero!");
                result = l / r;
                break;
            case MOD:
                if (r == 0) throw std::runtime_error("Division by zero!");
                result = l % r;
                break;
            case AND:
                result = l & r;
                break;
            case OR:
                result = l | r;
                break;
            case XOR:
                result = l ^ r;
                break;
            case LSHIFT:
                result = l << r;
                break;
            case RSHIFT:
                result = l >>> r;
                break;
            case ARSHIFT:
                result = l >> r;
                break;
            case EQ:
                result = l == r ? 1 : 0;
                break;
            case NEQ:
                result = l != r ? 1 : 0;
                break;
            case LT:
                result = l < r ? 1 : 0;
                break;
            case GT:
                result = l > r ? 1 : 0;
                break;
            case LEQ:
                result = l <= r ? 1 : 0;
                break;
            case GEQ:
                result = l >= r ? 1 : 0;
                break;
            default:
                throw std::runtime_error("Invalid binary operation");
            }
            exprStack->pushValue(result);
        }
        else if (auto temp = std::dynamic_pointer_cast<Mem>(insn)) {
            int addr = exprStack->popValue();
            exprStack->pushAddr(read(addr), addr);
        }
        else if (auto temp = std::dynamic_pointer_cast<Call>(insn)) {
            int argsCount = temp->getNumArgs();
            std::vector<int> args(argsCount);
            for (int i = argsCount - 1; i >= 0; --i)
                args[i] = exprStack->popValue();
            std::shared_ptr<StackItem> target = exprStack->pop();
            std::string targetName;
            if (target->type == StackItem::Kind::NAME)
                targetName = target->name;
            else if (indexToInsn.contains(target->value)) {
                std::shared_ptr<Node> node = indexToInsn[target->value];
                if (auto func = std::dynamic_pointer_cast<FuncDecl>(node))
                    targetName = func->getName();
                else throw std::runtime_error("Call to a non-function instruction!");
            }
            else throw std::runtime_error("Invalid function call '"
                    + insn + "' (target '" + target->value + "' is unknown)!");

            int retVal = call(frame, targetName, args);
            exprStack->pushValue(retVal);
        }
        else if (auto temp = std::dynamic_pointer_cast<Name>(insn)) {
            std::string name = temp->getName();
            exprStack->pushName(libraryFunctions.contains(name)
                    ? -1 : findLabel(name), name);
        }
        else if (auto temp = std::dynamic_pointer_cast<Move>(insn)) {
            int r = exprStack->popValue();
            std::shared_ptr<StackItem> stackItem = exprStack->pop();
            switch (stackItem->type) {
            using enum StackItem::Kind;
            case MEM:
                if (debugLevel > 0)
                    std::cout << "mem[" << stackItem->addr << "]=" << r << std::endl;
                store(stackItem->addr, r);
                break;
            case TEMP:
                if (debugLevel > 0)
                    std::cout << "temp[" << stackItem->temp << "]=" << r << std::endl;
                frame->put(stackItem->temp, r);
                break;
            default:
                throw std::runtime_error("Invalid MOVE!");
            }
        }
        else if (auto temp = std::dynamic_pointer_cast<Exp>(insn)) {
            // Discard result.
            exprStack->pop();
        }
        else if (auto temp = std::dynamic_pointer_cast<Jump>(insn))
            frame->setIP(exprStack->popValue());
        else if (auto temp = std::dynamic_pointer_cast<CJump>(insn)) {
            int top = exprStack->popValue();
            std::string label;
            if (top == 0)
                label = temp->getFalseLabel();
            else if (top == 1)
                label = temp->getTrueLabel();
            else throw std::runtime_error("Invalid value in CJUMP: expected 0/1, got "
                    + top);
            if (label) frame->setIP(findLabel(label));
        }
        else if (auto temp = std::dynamic_pointer_cast<Return>(insn)) {
            frame->ret = exprStack->popValue();
            frame->setIP(-1);
        }
    }

    /**
     *
     * @param name name of the label
     * @return the IR node at the named label
     */
    int findLabel(const std::string& name) {
        if (!nameToIndex.contains(name))
            throw std::runtime_error("Could not find label '" + name + "'!");
        return nameToIndex[name];
    }
};
