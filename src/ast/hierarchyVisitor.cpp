#include "hierarchyVisitor.hpp"

HierarchyVisitor::HierarchyVisitor(std::shared_ptr<SymbolTable> st, std::unordered_map<std::string, 
std::unordered_map<std::string, std::shared_ptr<SymbolTable>>> t) : symbolTable(st), tables(t) {
}

std::shared_ptr<SymbolTable> HierarchyVisitor::getSymbolTable() const {
    return symbolTable;
}
std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<SymbolTable>>> HierarchyVisitor::getTables() const {
    return tables;
}



void HierarchyVisitor::visit(std::shared_ptr<Program> n) {
    std::cout << "start of hierarchy visiting Program" << std::endl;
    n->classOrInterfaceDecl->accept(this);
}

void HierarchyVisitor::visit(std::shared_ptr<ClassDecl> n) {
    std::cout << "start of hierarchy visiting ClassDecl" << std::endl;
    std::vector<std::string> packageIDs;
    std::shared_ptr<Program> program = getSymbolTable()->getAst();
    if (program && program->package && program->package->id)
        packageIDs.push_back(program->package->id->name);
    for (std::shared_ptr<ImportStatement> is : program->importStatements->stmts) {
        packageIDs.push_back(is->id->name);
    }
    for (std::shared_ptr<IdentifierType>& e : n->extended) {
        for (std::string pkg : packageIDs) {
            if (tables[pkg].contains(e->id->name)) {
                //Attempt to cast to InterfaceDecl
                if (nullptr != std::dynamic_pointer_cast<InterfaceDecl>(tables[pkg][e->id->name]->getAst()->classOrInterfaceDecl)) {
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

