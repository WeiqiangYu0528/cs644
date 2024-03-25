#pragma once
#include <memory>
#include <string>
#include "InsnMapsBuilder.hpp"

/**
 * An intermediate representation for naming a memory address
 */
class Label : public Stmt, public std::enable_shared_from_this<Label> {
private:
    std::string name;

public:
    /**
     *
     * @param name name of this memory address
     */
    Label(const std::string& name);

    std::string getName() const;

    std::string getLabel() const override;

    std::shared_ptr<InsnMapsBuilder> buildInsnMapsEnter(std::shared_ptr<InsnMapsBuilder> v) override;
}
