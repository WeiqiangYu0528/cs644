#include "Configuration.hpp"
#include "FuncDecl.hpp"
#include "Stmt.hpp"
#include "Temp.hpp"
#include "BinOp.hpp"
#include "Call.hpp"
#include "Name.hpp"
#include "Const.hpp"
#include "CompUnit.hpp"
#include "Simulator.hpp"
#include "CheckCanonicalIRVisitor.hpp"
#include <iostream>
#include <memory>
#include <string>

int main() {
    // IR roughly corresponds to the Java example
    std::string arg0 = Configuration::ABSTRACT_ARG_PREFIX + "0";
    std::string arg1 = Configuration::ABSTRACT_ARG_PREFIX + "1";

    auto aBody = std::make_shared<Seq>(std::vector<std::shared_ptr<Stmt>>{
        std::make_shared<Move>(std::make_shared<Temp>("i"), std::make_shared<Temp>(arg0)),
        std::make_shared<Move>(std::make_shared<Temp>("j"), std::make_shared<Temp>(arg1)),
        std::make_shared<Return>(std::make_shared<BinOp>(BinOp::OpType::ADD,
            std::make_shared<Temp>("i"),
            std::make_shared<BinOp>(BinOp::OpType::MUL,
                std::make_shared<Const>(2),
                std::make_shared<Temp>("j"))))
    });
    FuncDecl aFunc("a", 2, aBody);

    auto bBody = std::make_shared<Seq>(std::vector<std::shared_ptr<Stmt>>{
        std::make_shared<Move>(std::make_shared<Temp>("i"), std::make_shared<Temp>(arg0)),
        std::make_shared<Move>(std::make_shared<Temp>("j"), std::make_shared<Temp>(arg1)),
        std::make_shared<Move>(std::make_shared<Temp>("x"),
            std::make_shared<BinOp>(BinOp::OpType::ADD,
                std::make_shared<Call>(std::make_shared<Name>("a"),
                    std::vector<std::shared_ptr<Expr>>{std::make_shared<Temp>("i"), std::make_shared<Temp>("j")}),
                std::make_shared<Const>(1))),
        std::make_shared<Return>(std::make_shared<BinOp>(BinOp::OpType::ADD,
            std::make_shared<Temp>("x"),
            std::make_shared<BinOp>(BinOp::OpType::MUL,
                std::make_shared<Const>(20),
                std::make_shared<Const>(5))))
    });
    FuncDecl bFunc("b", 2, bBody);

    CompUnit compUnit("test");
    compUnit.appendFunc(aFunc);
    compUnit.appendFunc(bFunc);

    // IR interpreter demo
    {
        Simulator sim(compUnit);
        long result = sim.call("b", {-1, 1});
        std::cout << "b(-1,1) evaluates to " << result << std::endl;
    }

    // IR canonical checker demo
    {
        CheckCanonicalIRVisitor cv;
        std::cout << "Canonical? " << (cv.visit(compUnit) ? "Yes" : "No") << std::endl;
    }

    return 0;
}
