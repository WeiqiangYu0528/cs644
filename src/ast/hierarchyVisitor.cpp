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
    for (std::shared_ptr<IdentifierType> e : n->extended) {
        for (const auto& entry : tables) {
            if (entry.second.contains(e->id->name)) {
                //cast to interface
                if (std::dynamic_pointer_cast<InterfaceDecl>((entry.second).at(e->id->name)->getAst()->classOrInterfaceDecl)) {
                    std::cerr << "Error: Class " << n->id->name << " extends interface " << e->id->name << std::endl;
                    error = true;
                }

            }
        }
    }
}

void HierarchyVisitor::visit(std::shared_ptr<InterfaceDecl> n) {
    std::cout << "HV visiting InterfaceDecl, not yet implemented" << std::endl;
}

