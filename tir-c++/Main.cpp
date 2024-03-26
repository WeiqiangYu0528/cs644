#include <iostream>
#include <string>
//package joosc.ir.interpret;

//import joosc.ir.ast.*;
#include "BinOp.hpp"
#include "Call.hpp"
#include "CJump.hpp"
#include "CompUnit.hpp"
#include "Const.hpp"
#include "ESeq.hpp"
#include "Exp.hpp"
#include "Expr_c.hpp"
#include "Expr.hpp"
#include "FuncDecl.hpp"
#include "Jump.hpp"
#include "Label.hpp"
#include "Mem.hpp"
#include "Move.hpp"
#include "Name.hpp"
#include "Node_c.hpp"
#include "Node.hpp"
#include "NodeFactory_c.hpp"
#include "NodeFactory.hpp"
#include "Return.hpp"
#include "Seq.hpp"
#include "Stmt.hpp"
#include "Temp.hpp"
//import joosc.ir.visit.CheckCanonicalIRVisitor;
#include "CheckCanonicalIRVisitor.hpp"



int main(int argc, char* argv[]) {
        // Runs a simple program in the interpreter

        // IR roughly corresponds to the following:
        //     int a(int i, int j) {
        //         return i + 2 * j;
        //     }
        //     int b(int i, int j) {
        //         int x = a(i, j) + 1;
        //         return x + 20 * 5;
        //     }

    std::string arg0 = Configuration.ABSTRACT_ARG_PREFIX + 0;
    std::string arg1 = Configuration.ABSTRACT_ARG_PREFIX + 1;

    Stmt aBody = Seq(Move(Temp("i"), Temp(arg0)),
                                 Move( Temp("j"), Temp(arg1)),
                                  Return( BinOp(BinOp.OpType.ADD,
                                          Temp("i"),
                                          BinOp(BinOp.OpType.MUL,
                                                  Const(2),
                                                  Temp("j")))));
    FuncDecl aFunc("a", 2, aBody);

    Stmt bBody =  Seq( Move( Temp("i"),  Temp(arg0)),
                           Move( Temp("j"),  Temp(arg1)),
                           Move( Temp("x"),
                                   BinOp(BinOp.OpType.ADD,
                                           Call( Name("a"),
                                                   Temp("i"),
                                                   Temp("j")),
                                           Const(1))),
                           Return( BinOp(BinOp.OpType.ADD,
                                   Temp("x"),
                                   BinOp(BinOp.OpType.MUL,
                                           Const(20),
                                           Const(5)))));
    FuncDecl bFunc("b", 2, bBody);

    CompUnit compUnit("test");
    compUnit.appendFunc(aFunc);
    compUnit.appendFunc(bFunc);

    // IR interpreter demo
    {
        Simulator sim(compUnit);
        long result = sim.call("b", -1, 1);
        std::cout << "b(-1,1) evaluates to " << result << std::endl;
    }

    // IR canonical checker demo
    {
        CheckCanonicalIRVisitor cv();
        std::cout << "Canonical? " << cv.visit(compUnit) << std::endl;
    }
}

