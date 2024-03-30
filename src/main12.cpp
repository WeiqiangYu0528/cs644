// #include <iostream>
// #include <memory>
// #include <string>
// #include "IRAst.hpp"
// #include "Configuration.hpp"
// #include "CheckCanonicalIRVisitor.hpp"
// #include "Simulator.hpp"

// using namespace TIR;

// int main() {
//     // Runs a simple program in the interpreter

//     // IR roughly corresponds to the following:
//     //     int a(int i, int j) {
//     //         return i + 2 * j;
//     //     }
//     //     int b(int i, int j) {
//     //         int x = a(i, j) + 1;
//     //         return x + 20 * 5;
//     //     }
//     std::string arg0 = std::string(Configuration::ABSTRACT_ARG_PREFIX) + "0";
//     std::string arg1 = std::string(Configuration::ABSTRACT_ARG_PREFIX) + "1";

//     auto aBody = std::make_shared<Seq>(std::vector<std::shared_ptr<Stmt>>{
//         std::make_shared<Move>(std::make_shared<Temp>("i"), std::make_shared<Temp>(arg0)),
//         std::make_shared<Move>(std::make_shared<Temp>("j"), std::make_shared<Temp>(arg1)),
//         std::make_shared<Return>(std::make_shared<BinOp>(BinOp::OpType::ADD,
//             std::make_shared<Temp>("i"),
//             std::make_shared<BinOp>(BinOp::OpType::MUL,
//                 std::make_shared<Const>(2),
//                 std::make_shared<Temp>("j"))))
//     });
//     std::shared_ptr<FuncDecl> aFunc = std::make_shared<FuncDecl>("a", 2, aBody);

//     auto bBody = std::make_shared<Seq>(std::vector<std::shared_ptr<Stmt>>{
//         std::make_shared<Move>(std::make_shared<Temp>("i"), std::make_shared<Temp>(arg0)),
//         std::make_shared<Move>(std::make_shared<Temp>("j"), std::make_shared<Temp>(arg1)),
//         std::make_shared<Move>(std::make_shared<Temp>("x"),
//             std::make_shared<BinOp>(BinOp::OpType::ADD,
//                 std::make_shared<Call>(std::make_shared<Name>("a"),
//                     std::vector<std::shared_ptr<Expr>>{std::make_shared<Temp>("i"), std::make_shared<Temp>("j")}),
//                 std::make_shared<Const>(1))),
//         std::make_shared<Return>(std::make_shared<BinOp>(BinOp::OpType::ADD,
//             std::make_shared<Temp>("x"),
//             std::make_shared<BinOp>(BinOp::OpType::MUL,
//                 std::make_shared<Const>(20),
//                 std::make_shared<Const>(5))))
//     });
//     std::shared_ptr<FuncDecl> bFunc = std::make_shared<FuncDecl>("b", 2, bBody);

//     std::shared_ptr<CompUnit> compUnit = std::make_shared<CompUnit>("test");
//     compUnit->appendFunc(aFunc);
//     compUnit->appendFunc(bFunc);

//     // IR interpreter demo
//     {
//         Simulator sim(compUnit);
//         long result = sim.call("b", -1, 1);
//         std::cout << "b(-1,1) evaluates to " << result << std::endl;
//     }

//     // IR canonical checker demo
//     {
//         std::shared_ptr<CheckCanonicalIRVisitor> cv = std::make_shared<CheckCanonicalIRVisitor>();
//         std::cout << "Canonical? " << (cv->visit(compUnit) ? "Yes" : "No") << std::endl;
//     }

//     return 0;
// }
