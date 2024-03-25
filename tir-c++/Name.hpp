#pragma once
#include <string>
#include "Expr_c.hpp"

class Name : public Expr_c, public std::enable_shared_from_this<Name> {
private:
    std::string name;

public:
    Name(const std::string& name);

    std::string getName() const;

    virtual std::string getLabel() const override;
};