#pragma once
#include "Expr_c.hpp"
class Temp : public Expr_c {
private:
    std::string name;

public:
    Temp(const std::string& name);

    std::string getName() const;

    virtual std::string getLabel() const override;
};