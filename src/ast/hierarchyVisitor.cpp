#include <unordered_set>
#include <cassert>
#include "hierarchyVisitor.hpp"

HierarchyVisitor::HierarchyVisitor(std::shared_ptr<SymbolTable> st, std::unordered_map<std::string, 
std::unordered_map<std::string, std::shared_ptr<SymbolTable>>> t) : symbolTable(st), tables(t), error(false) {
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


void HierarchyVisitor::visit(std::shared_ptr<Program> n) {
    n->classOrInterfaceDecl->accept(this);
}

void HierarchyVisitor::visit(std::shared_ptr<ClassDecl> n) {
    //Rule #1: Class must not extend an interface
    for (std::shared_ptr<IdentifierType> ext : n->extended) {
        for (const auto& entry : tables) {
            if (entry.second.contains(ext->id->name)) {
                //cast to InterfaceDecl
                if (std::dynamic_pointer_cast<InterfaceDecl>((entry.second).at(ext->id->name)->getAst()->classOrInterfaceDecl)) {
                    std::cerr << "Error: Class " << n->id->name << " extends Interface " << ext->id->name << std::endl;
                    error = true;
                    return;
                }

            }
        }
    }

    //Rule #4: Class must not extend final class
    for (std::shared_ptr<IdentifierType> ext : n->extended) {
        for (const auto& entry : tables) {
            if (entry.second.contains(ext->id->name)) {
                std::shared_ptr<ClassDecl> classDecl = std::dynamic_pointer_cast<ClassDecl>(entry.second.at(ext->id->name)->getAst()->classOrInterfaceDecl);
                assert(classDecl != nullptr); //Rule #1 already ensures that you can't extend an interface
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
        if (implementedInterfaces.contains(impl->id->name)) {
            std::cerr << "Error: Class " << n->id->name << " implements Interface " << impl->id->name << " more than once" << std::endl;
            error = true;
            return;    
        } else {
            implementedInterfaces.insert(impl->id->name);
        }
    }
}

void HierarchyVisitor::visit(std::shared_ptr<InterfaceDecl> n) {
    //Rule #3: An interface must not be repeated in an interface's extends clause
    std::unordered_set<std::string> extendedInterfaces;
    for (std::shared_ptr<IdentifierType> ext : n->extended) {
        if (extendedInterfaces.contains(ext->id->name)) {
            std::cerr << "Error: Interface " << n->id->name << " extends Interface " << ext->id->name << " more than once" << std::endl;
            error = true;
            return; 
        } else {
            extendedInterfaces.insert(ext->id->name);
        }
    }
}

