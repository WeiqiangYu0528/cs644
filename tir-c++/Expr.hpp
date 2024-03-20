#pragma once

class Expr : public Node {
    virtual bool isConstant() = 0;

    virtual int constant() = 0;
};
