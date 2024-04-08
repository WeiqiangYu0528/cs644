#include "IRAst.hpp"
#include "Simulator.hpp"

using namespace TIR;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distr(0, INT_MAX);
int debugLevel = 2;

ExecutionFrame::ExecutionFrame(int ip, Simulator& simulator) : ip(ip), ret(distr(gen)), simulator(simulator) {
}

int ExecutionFrame::get(const std::string& tempName) {
    if (simulator.staticFields.contains(tempName))
        return simulator.staticFields[tempName];
    if (!regs.contains(tempName)) {
        /* Referencing a temp before having written to it - initialize
            with garbage */
        if (simulator.isStaticInit) put(tempName, 0);
        else put(tempName, distr(gen));
    }
    return regs[tempName];
}

void ExecutionFrame::put(const std::string& tempName, int value) {
    regs[tempName] = value;
}

bool ExecutionFrame::advance() {
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

void ExecutionFrame::setIP(int ip) {
    this->ip = ip;
    if (debugLevel > 1) {
        if (ip == -1)
            std::cout << "Returning" << std::endl;
        else std::cout << "Jumping to " << getCurrentInsn()->getLabel() << std::endl;
    }
}

std::shared_ptr<Node> ExecutionFrame::getCurrentInsn() {
    if (!simulator.indexToInsn.contains(ip))
        throw std::runtime_error("No next instruction.  Forgot RETURN?");
    return simulator.indexToInsn[ip];
}

StackItem::StackItem(int value) : type(Kind::COMPUTED), value(value) {
}

StackItem::StackItem(int value, int addr) : type(Kind::MEM), value(value), addr(addr) {
}

StackItem::StackItem(Kind type, int value, const std::string& string) : type(type), value(value) {
    if (type == Kind::TEMP)
        temp = string;
    else if (type == Kind::NAME)
        name = string;
    else
        throw std::runtime_error("Unknown StackItem Kind");
}

int ExprStack::popValue() {
    int value = stack.top()->value;
    stack.pop();
    if (debugLevel > 1) std::cout << "Popping value " << value << std::endl;
    return value;
}

std::shared_ptr<StackItem> ExprStack::pop() {
    std::shared_ptr<StackItem> item = stack.top();
    stack.pop();
    return item;
}

void ExprStack::pushAddr(int value, int addr) {
    if (debugLevel > 1)
        std::cout << "Pushing MEM " << value << " (" << addr << ")" << std::endl;
    stack.push(std::make_shared<StackItem>(value, addr));
}

void ExprStack::pushTemp(int value, const std::string& temp) {
    if (debugLevel > 1)
        std::cout << "Pushing TEMP " << value << " (" << temp << ")" << std::endl;
    stack.push(std::make_shared<StackItem>(StackItem::Kind::TEMP, value, temp));
}

void ExprStack::pushName(int value, const std::string& name) {
    if (debugLevel > 1)
        std::cout << "Pushing NAME " << value << " (" << name << ")" << std::endl;
    stack.push(std::make_shared<StackItem>(StackItem::Kind::NAME, value, name));
}

void ExprStack::pushValue(int value) {
    if (debugLevel > 1) std::cout << "Pushing value " << value << std::endl;
    stack.push(std::make_shared<StackItem>(value));
}

Simulator::Simulator(std::shared_ptr<CompUnit> compUnit) : Simulator(compUnit, DEFAULT_HEAP_SIZE) {
}

Simulator::Simulator(std::shared_ptr<CompUnit> compUnit, int heapSize) : compUnit(compUnit), heapSizeMax(heapSize), exprStack(std::make_shared<ExprStack>()), isStaticInit(false) {
    libraryFunctions.insert("__malloc");
    libraryFunctions.insert("__debexit");
    libraryFunctions.insert("__exception");
    libraryFunctions.insert("NATIVEjava.io.OutputStream.nativeWrite");

    std::shared_ptr<InsnMapsBuilder> imb = std::make_shared<InsnMapsBuilder>();
    compUnit = std::dynamic_pointer_cast<CompUnit>(imb->visit(compUnit));
    indexToInsn = imb->indexToInsn();
    nameToIndex = imb->nameToIndex();
}

int Simulator::malloc(int size) {
    if (size < 0) throw std::runtime_error("Invalid size");
    // if (size % Configuration::WORD_SIZE != 0)
    //     throw std::runtime_error("Can only allocate in chunks of "
    //             + std::to_string(Configuration::WORD_SIZE) + " bytes!");

    int retval = mem.size();
    if (retval + size > heapSizeMax) throw std::runtime_error("Out of heap!");
    for (int i = 0; i < size; i++) {
        mem.push_back(0);
    }
    return retval;
}

int Simulator::calloc(int size) {
    int retval = malloc(size);
    for (int i = (int) retval; i < retval + size; i++) {
        mem[i] = 0;
    }
    return retval;
}

int Simulator::read(int addr) {
    size_t i = getMemoryIndex(addr);
    if (i >= mem.size())
        throw std::runtime_error("Attempting to read past end of heap");
    return mem[i];
}

void Simulator::store(int addr, int value) {
    size_t i = getMemoryIndex(addr);
    if (i >= mem.size())
        throw std::runtime_error("Attempting to store past end of heap");
    mem[i] = value;
}

int Simulator::getMemoryIndex(int addr) {
    // if (addr % Configuration::WORD_SIZE != 0)
    //     throw std::runtime_error("Unaligned memory access: " + std::to_string(addr) + " (word size="
    //             + std::to_string(Configuration::WORD_SIZE) + ")");
    return addr;
    // return addr / Configuration::WORD_SIZE;
}

int Simulator::call(std::shared_ptr<ExecutionFrame> parent, const std::string& name, std::vector<int>& args) {
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
        for (size_t i = 0; i < args.size(); ++i)
            frame->put(std::string(Configuration::ABSTRACT_ARG_PREFIX) + std::to_string(i), args[i]);

        // Simulate!
        while (frame->advance()) ;

        ret = frame->ret;
    }

    parent->put(Configuration::ABSTRACT_RET, ret);
    return ret;
}

int Simulator::libraryCall(const std::string& name, std::vector<int>& args) {
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

void Simulator::leave(std::shared_ptr<ExecutionFrame> frame) {
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
        using enum BinOp::OpType;
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
            result = l >> r;
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
        case LTU:
            result = (unsigned) l < (unsigned) r ? 1 : 0;
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
    else if (auto temp = std::dynamic_pointer_cast<Call_s>(insn)) {
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
        else throw std::runtime_error("Invalid function call (target '" + std::to_string(target->value) + "' is unknown)!");

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
        if (!label.empty()) frame->setIP(findLabel(label));
    }
    else if (auto temp = std::dynamic_pointer_cast<Return>(insn)) {
        if (temp->getRet()) frame->ret = exprStack->popValue();
        if (isStaticInit) {
            for (auto& [name, value] : staticFields)
                staticFields[name] = frame->regs[name];
        }
        frame->setIP(-1);
    }
}

int Simulator::findLabel(const std::string& name) {
    if (!nameToIndex.contains(name))
        throw std::runtime_error("Could not find label '" + name + "'!");
    return nameToIndex[name];
}

void Simulator::initStaticFields() {
    isStaticInit = true;
    call(Configuration::STATIC_INIT_FUNC);
    isStaticInit = false;
}
