// #include <iostream>
// #include <memory>
// #include <string>
// #include "IRAst.hpp"
// #include "Configuration.hpp"
// #include "CheckCanonicalIRVisitor.hpp"
// #include "Simulator.hpp"
// #include "Tiling.hpp"
// #include <fstream>

// using namespace TIR;

// void createAndWriteAssemblyFile(const std::string& directory, const std::string& filename, const std::vector<std::string>& assemblyInstructions) {
//     std::string filePath = directory + "/" + filename;
//     std::ofstream assemblyFile(filePath);
//     if (!assemblyFile.is_open()) {
//         throw std::runtime_error("Unable to create the file: " + filePath);
//     }
//     assemblyFile << "global _start\n";
//     assemblyFile << "_start:\n";

//     for (const std::string& instruction : assemblyInstructions) {
//         assemblyFile << instruction << "\n";
//     }

//     assemblyFile.close();
// }

// int main() {
//     // Runs a simple program in the interpreter

//     // IR roughly corresponds to the following:
//     //     int a(int i, int j) {
//     //         return i + 2 * j;
//     //     }
//         // int b(int i, int j) {
//         //     int x = a(i, j) + 1;
//         //     return x + 20 * 5;
//         // }
//     std::string arg0 = std::string(Configuration::ABSTRACT_ARG_PREFIX) + "0";
//     std::string arg1 = std::string(Configuration::ABSTRACT_ARG_PREFIX) + "1";

//     auto aBody = std::make_shared<Seq>(std::vector<std::shared_ptr<Stmt>>{
//         // std::make_shared<Move>(std::make_shared<Temp>("i"), std::make_shared<Temp>(arg0)),
//         // std::make_shared<Move>(std::make_shared<Temp>("j"), std::make_shared<Temp>(arg1)),
//         std::make_shared<Return>(std::make_shared<Call>(std::make_shared<Const>(2), std::make_shared<Const>(6)))
//             // std::make_shared<Return>(
//             //     std::make_shared<BinOp>(BinOp::OpType::ADD,
//             //         std::make_shared<Temp>("i"),
//             //         std::make_shared<BinOp>(BinOp::OpType::MUL,
//             //             std::make_shared<Const>(2),
//             //             std::make_shared<Temp>("j"))))
//         // std::make_shared<Return>(std::make_shared<BinOp>(BinOp::OpType::ADD,
//         //     std::make_shared<Temp>("i"),
//         //     std::make_shared<BinOp>(BinOp::OpType::MUL,
//         //         std::make_shared<Const>(2),
//         //         std::make_shared<Temp>("j"))))
//     });
//     // int add(int a, int b) {
//     //     return a + b;
//     // }
//     auto add = std::make_shared<FuncDecl>("add", 2, std::make_shared<Seq>(std::vector<std::shared_ptr<Stmt>>{
//         std::make_shared<Return>(std::make_shared<BinOp>(BinOp::OpType::ADD, std::make_shared<Temp>("a"), std::make_shared<Temp>("b")))
//     }));

//     std::shared_ptr<FuncDecl> aFunc = std::make_shared<FuncDecl>("a", 0, aBody);

//     // auto bBody = std::make_shared<Seq>(std::vector<std::shared_ptr<Stmt>>{
//     //     std::make_shared<Move>(std::make_shared<Temp>("i"), std::make_shared<Temp>(arg0)),
//     //     std::make_shared<Move>(std::make_shared<Temp>("j"), std::make_shared<Temp>(arg1)),
//     //     std::make_shared<Move>(std::make_shared<Temp>("x"),
//     //         std::make_shared<BinOp>(BinOp::OpType::ADD,
//     //             std::make_shared<Call>(std::make_shared<Name>("a"),
//     //                 std::vector<std::shared_ptr<Expr>>{std::make_shared<Temp>("i"), std::make_shared<Temp>("j")}),
//     //             std::make_shared<Const>(1))),
//     //     std::make_shared<Return>(std::make_shared<BinOp>(BinOp::OpType::ADD,
//     //         std::make_shared<Temp>("x"),
//     //         std::make_shared<BinOp>(BinOp::OpType::MUL,
//     //             std::make_shared<Const>(20),
//     //             std::make_shared<Const>(5))))
//     // });
//     // std::shared_ptr<FuncDecl> bFunc = std::make_shared<FuncDecl>("b", 2, bBody);

//     std::shared_ptr<CompUnit> compUnit = std::make_shared<CompUnit>("test");
//     compUnit->appendFunc(aFunc);
//     compUnit->appendFunc(add);

//     // IR interpreter demo
//     {
//         Simulator sim(compUnit);
//         long result = sim.call("a");
//         std::cout << "b(-1,1) evaluates to " << result << std::endl;
//     }

//     // IR canonical checker demo
//     {
//         std::shared_ptr<CheckCanonicalIRVisitor> cv = std::make_shared<CheckCanonicalIRVisitor>();
//         std::cout << "Canonical? " << (cv->visit(compUnit) ? "Yes" : "No") << std::endl;
//     }

//     {
//         Tiling tiler;
//         bool firstFunction = true;
//         for (auto& [funcName, funcDecl] : compUnit->getFunctions()) {
//             std::vector<std::string> assemblyInstructions;
//             if (firstFunction) {
//                 assemblyInstructions.push_back("_start:"); // Entry point label
//                 firstFunction = false;
//             } else {
//                 assemblyInstructions.push_back(funcName + ":");
//             }
//             for (auto stmt : std::dynamic_pointer_cast<Seq>(funcDecl->getBody())->getStmts()) {
//                 if (std::dynamic_pointer_cast<Call_s>(stmt) != nullptr) {
//                     std::cout << "Call statement found" << std::endl;
//                     assemblyInstructions.push_back("call " + std::dynamic_pointer_cast<Call_s>(stmt)->getTarget()->getName());
//                 }
//                 tiler.tileStmt(stmt, assemblyInstructions);
//             }
//             for (auto instr : assemblyInstructions) {
//                 std::cout << instr << std::endl;
//             }
//         }
//     }

//     return 0;
// }