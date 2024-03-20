#pragma once
#include <string>
// import joosc.ir.visit.InsnMapsBuilder;

/**
 * An intermediate representation for naming a memory address
 */
class Label : public Stmt {
private:
    std::string name;

public:
    /**
     *
     * @param name name of this memory address
     */
    Label(const std::string& name);

    std::string name() const;

    std::string label() const override;

    InsnMapsBuilder buildInsnMapsEnter(InsnMapsBuilder& v) override;
}
