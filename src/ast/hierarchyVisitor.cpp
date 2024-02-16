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
    //Class must not extend an interface
    for (std::shared_ptr<IdentifierType> e : n->extended) {
        for (const auto& entry : tables) {
            if (entry.second.contains(e->id->name)) {
                //cast to InterfaceDecl
                if (std::dynamic_pointer_cast<InterfaceDecl>((entry.second).at(e->id->name)->getAst()->classOrInterfaceDecl)) {
                    std::cerr << "Error: Class " << n->id->name << " extends Interface " << e->id->name << std::endl;
                    error = true;
                }

            }
        }
    }
    //Class must not implement a class
    for (std::shared_ptr<IdentifierType> impl : n->implemented) {
        for (const auto& entry : tables) {
            if (entry.second.contains(impl->id->name)) {
                //cast to ClassDecl
                if (std::dynamic_pointer_cast<ClassDecl>((entry.second).at(impl->id->name)->getAst()->classOrInterfaceDecl)) {
                    std::cerr << "Error: Class " << n->id->name << " implements Class " << impl->id->name << std::endl;
                    error = true;
                }

            }
        }
    }
}

void HierarchyVisitor::visit(std::shared_ptr<InterfaceDecl> n) {
    std::cout << "HV visiting InterfaceDecl, not yet implemented" << std::endl;
}

