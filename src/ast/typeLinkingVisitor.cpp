#include "typeLinkingVisitor.hpp"

TypeLinkingVisitor::TypeLinkingVisitor(std::unordered_map<std::string, 
                                       std::unordered_map<std::string, std::shared_ptr<SymbolTable>>>& t) : tables(t), error(false) {
}

bool TypeLinkingVisitor::isError() const {
    return error;
}

void TypeLinkingVisitor::visit(std::shared_ptr<ImportStatements> n) {
    for (auto stmt: n->stmts) {
        std::string pkg = stmt->package->name;
        std::string cdecl = stmt->id->name;
        if (!tables.contains(pkg)) error = true;
        else if (cdecl != "*" && !tables.at(pkg).contains(cdecl)) error = true;
        if (error) std::cerr << "Error: TypeLinkingVisitor: ImportStatements: " << pkg << "." << cdecl << std::endl;

        //add import to current scope
        if (cdecl == "*") {
            for (auto& [k, v] : tables[pkg]) {
                scopes[k] = v;
            }
        } else {
            scopes[cdecl] = tables[pkg][cdecl];
        }

        stmt->accept(this);
    }
}

void TypeLinkingVisitor::visit(std::shared_ptr<FieldAccessExp> n) {
    Visitor::visit(n);
}

void TypeLinkingVisitor::visit(std::shared_ptr<ClassDecl> n) {
    for (auto e : n->extended) {
        if (!scopes.contains(e->id->name)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: Invalid base class: " << e->id->name << " not found" << std::endl;
        }
        e->accept(this);
    }
    for (auto i : n->implemented) {
        if (!scopes.contains(i->id->name)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: Invalid interface: " << i->id->name << " not found" << std::endl;
        }
        i->accept(this);
    }
    for (int i = 0; i < 3; i++) {
        for (auto d : n->declarations[i]) d->accept(this);
    }
}

void TypeLinkingVisitor::visit(std::shared_ptr<InterfaceDecl> n) {
    n->id->accept(this);
    for (auto e : n->extended) {
        if (!scopes.contains(e->id->name)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: Invalid interface: " << e->id->name << " not found" << std::endl;
        }
        e->accept(this);
    }
    for (auto& m : n->methods) {
        m->accept(this);
    }
}

void TypeLinkingVisitor::visit(std::shared_ptr<Program> n) {
    //add all scopes to the symbol table
    if(n->package) {
        std::string pkg = n->package->id->name;
        for (auto& [k, v] : tables[pkg]) {
            scopes[k] = v;
        }
    } else {
        std::string cdecl = n->classOrInterfaceDecl->id->name;
        scopes[""] = tables[""][cdecl];
    }
    Visitor::visit(n);
}
