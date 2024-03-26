#pragma once
#include "Node_c.hpp"

class Expr : public Node_c {
public:
    virtual bool isConstant() const = 0;

    virtual int getConstant() const = 0;
};
