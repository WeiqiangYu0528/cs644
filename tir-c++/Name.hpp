#include "Expr.hpp"
#include <string>

class Name : public Expr_c {
private:
    std::string name;

public:
    Name(const std::string& name);

    std::string name() const;

    virtual std::string label() const override;
};