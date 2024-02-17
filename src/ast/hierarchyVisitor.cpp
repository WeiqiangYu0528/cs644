#include <unordered_set>
#include <cassert>
#include "hierarchyVisitor.hpp"

HierarchyVisitor::HierarchyVisitor(std::shared_ptr<SymbolTable> st, std::unordered_map<std::string, 
std::unordered_map<std::string, std::shared_ptr<SymbolTable>>> t) : symbolTable(st), tables(t), error(false) {

    std::unordered_map<std::string, int> methodCount;
    for (const auto& pair : symbolTable->mtable) {
        std::string methodName = pair.first;
        std::cout << methodName << std::endl;
        methodCount[methodName]++;

        if (methodCount[methodName] > 1) {
            error = true;
            std::cerr << "Error: Duplicate method name '" << methodName << "' found." << std::endl;
            // break; 
        }
    }

    std::unordered_map<std::string, int> constructorCount;
    for (const auto& pair : symbolTable->ctable) {
        std::string cName = pair.first;
        std::cout << cName << std::endl;
        constructorCount[cName]++;

        if (constructorCount[cName] > 1) {
            error = true;
            std::cerr << "Error: Duplicate constructor name '" << cName << "' found." << std::endl;
            // break; 
        }
    }
}

std::shared_ptr<SymbolTable> HierarchyVisitor::getSymbolTable() const {
    return symbolTable;
}

std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<SymbolTable>>> HierarchyVisitor::getTables() const {
    return tables;
}

bool HierarchyVisitor::isError() const {
    return error;
}

// helper function
bool areFormalParametersEqual(const std::vector<std::shared_ptr<FormalParameter>>& params1,
                              const std::vector<std::shared_ptr<FormalParameter>>& params2) {
    if (params1.size() != params2.size()) return false;

    std::vector<bool> matched(params2.size(), false); // Track matched parameters in params2

    for (const auto& param1 : params1) {
        bool foundMatch = false;
        for (size_t j = 0; j < params2.size(); ++j) {
            if (!matched[j] && param1 && params2[j] && param1->isEqual(*params2[j])) {
                matched[j] = true; // Mark as matched
                foundMatch = true;
                break;
            }
        }
        if (!foundMatch) {
            return false; // No match found for param1 in params2
        }
    }
    // All parameters in params1 have a match in params2
    return true;
}

Modifiers stringToModifier(const std::string& str) {
    if (str == "PUBLIC") return Modifiers::PUBLIC;
    else if (str == "PROTECTED") return Modifiers::PROTECTED;
    else if (str == "FINAL") return Modifiers::FINAL;
    else if (str == "ABSTRACT") return Modifiers::ABSTRACT;
    else if (str == "STATIC") return Modifiers::STATIC;
    else if (str == "NATIVE") return Modifiers::NATIVE;
    // else throw std::invalid_argument("Unknown modifier: " + str);
}

std::ostream& operator<<(std::ostream& os, const Modifiers modifier) {
    switch (modifier) {
        case Modifiers::PUBLIC:
            os << "PUBLIC";
            break;
        case Modifiers::PROTECTED:
            os << "PROTECTED";
            break;
        case Modifiers::FINAL:
            os << "FINAL";
            break;
        case Modifiers::ABSTRACT:
            os << "ABSTRACT";
            break;
        case Modifiers::STATIC:
            os << "STATIC";
            break;
        case Modifiers::NATIVE:
            os << "NATIVE";
            break;
        default:
            os << "Unknown Modifier";
            break;
    }
    return os;
}

void HierarchyVisitor::visit(std::shared_ptr<Program> n) {
    if (n->package) {
        this->currentPackageName = n->package->id->name;
    } else {
        this->currentPackageName = ""; // Or some default handling if the package is not specified
    }
    n->classOrInterfaceDecl->accept(this);
    Visitor::visit(n);
}

void HierarchyVisitor::visit(std::shared_ptr<ClassDecl> n) {
    this->classModifer = n-> modifier;
    //Rule #1: Class must not extend an interface
    //Rule #4: Class must not extend final class
    for (std::shared_ptr<IdentifierType> ext : n->extended) {
        for (const auto& entry : tables) {
            if (entry.second.contains(ext->id->name)) {
                //Rule #1
                if (std::dynamic_pointer_cast<InterfaceDecl>((entry.second).at(ext->id->name)->getAst()->classOrInterfaceDecl)) {
                    std::cerr << "Error: Class " << n->id->name << " extends Interface " << ext->id->name << std::endl;
                    error = true;
                    return;
                }
                //Rule #4
                std::shared_ptr<ClassDecl> classDecl = std::dynamic_pointer_cast<ClassDecl>(entry.second.at(ext->id->name)->getAst()->classOrInterfaceDecl);
                assert(classDecl != nullptr); //Rule #1 already ensures that you can't extend an interface, so this cast should succeed
                if (classDecl->modifier == "final") {
                    std::cerr << "Error: Class " << n->id->name << " extends final Class " << classDecl->id->name << std::endl;
                    error = true;
                    return;
                }

            }
        }
    }

    //Rule #2: Class must not implement a class
    //Rule #3: An interface must not be repeated in a class's implements clause
    std::unordered_set<std::string> implementedInterfaces;
    for (std::shared_ptr<IdentifierType> impl : n->implemented) {
        //Rule #2
        for (const auto& entry : tables) {
            if (entry.second.contains(impl->id->name)) {
                //cast to ClassDecl
                if (std::dynamic_pointer_cast<ClassDecl>((entry.second).at(impl->id->name)->getAst()->classOrInterfaceDecl)) {
                    std::cerr << "Error: Class " << n->id->name << " implements Class " << impl->id->name << std::endl;
                    error = true;
                    return;
                }
            }
        }
        //Rule #3
        if (implementedInterfaces.contains(impl->id->name)) {
            std::cerr << "Error: Class " << n->id->name << " implements Interface " << impl->id->name << " more than once" << std::endl;
            error = true;
            return;    
        } else {
            implementedInterfaces.insert(impl->id->name);
        }
    }
    // std::unordered_map<std::string, int> methodCount;

    for (auto m : n->declarations[1]) {
        // std::string methodName = m->getName();
        // methodCount[methodName]++;
        // if (methodCount[methodName] > 1) {
        //     error = true;
        //     std::cerr << "Error: Duplicate method name '" << methodName << "' found." << std::endl;
        //     break;
        // } else {
            m->accept(this);
        // }
    }    
}

void HierarchyVisitor::visit(std::shared_ptr<InterfaceDecl> n) {
    //Rule #3: Interfaces must not be repeated in an interface's extends clause
    //Rule #5: Interface must not extend a class
    std::unordered_set<std::string> extendedInterfaces;
    for (std::shared_ptr<IdentifierType> ext : n->extended) {
        //Rule #5
        for (const auto& entry : tables) {
            if (entry.second.contains(ext->id->name)) {
                //try to cast to ClassDecl
                if (std::dynamic_pointer_cast<ClassDecl>((entry.second).at(ext->id->name)->getAst()->classOrInterfaceDecl)) {
                    std::cerr << "Error: Interface " << n->id->name << " extends Class " << ext->id->name << std::endl;
                    error = true;
                    return; 
                }
            }
        }
        //Rule #3
        if (extendedInterfaces.contains(ext->id->name)) {
            std::cerr << "Error: Interface " << n->id->name << " extends Interface " << ext->id->name << " more than once" << std::endl;
            error = true;
            return; 
        } else {
            extendedInterfaces.insert(ext->id->name);
        }
    }
    for (auto& m : n->methods) {
        m->accept(this);
    }
}


void HierarchyVisitor::visit(std::shared_ptr<Method> n) 
{
    const std::string key {n->methodName->name};
    if (currentPackageName == "java.lang") {
        return;
    }
    // Rule 10
    for (auto currentModifier : n->modifiers) {
        if (currentModifier == Modifiers::ABSTRACT && stringToModifier(this->classModifer) != Modifiers::ABSTRACT) {
            std::cerr << "Error: No abstract method in no abstract class" << std::endl;
            error = true;
            break;
        }
    }
    for (const auto& packageEntry : tables) {
        const std::string& packageName = packageEntry.first;
        const auto& classes = packageEntry.second;
        for (const auto& classEntry : classes) {
            const std::string& className = classEntry.first;
            const std::shared_ptr<SymbolTable>& symbolTablehere = classEntry.second;

            if (symbolTablehere->getMethod(key) && packageName != currentPackageName) {
                auto methodNode = std::dynamic_pointer_cast<Method>(symbolTablehere->mtable[key]);
                for (auto modifier : methodNode->modifiers) {
                    // Rule 15
                    if (modifier == Modifiers::FINAL && areFormalParametersEqual(n->formalParameters, methodNode->formalParameters)) {
                        std::cerr << "Error: Method " << key << " can not override final method" << std::endl;
                        error = true;
                        break;
                    }
                    // Rule 14
                    if (modifier == Modifiers::PUBLIC) {
                        for (auto currentModifier : n->modifiers) {
                            if ( currentModifier == Modifiers::PROTECTED) {
                                std::cerr << "Error: PROTECTED Method " << key << "can not override PUBLIC method" << std::endl;
                                error = true;
                                break;
                            }
                        }
                    }
                }
            } 
        }
    }

    Visitor::visit(n);
}