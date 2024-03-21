#pragma once
#include <memory>
#include <string>

/**
 * An intermediate representation for naming a memory address
 */
class Label : public Stmt, std::enable_shared_from_this<Label> {
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
