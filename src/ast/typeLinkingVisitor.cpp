#include "typeLinkingVisitor.hpp"

TypeLinkingVisitor::TypeLinkingVisitor(std::unordered_map<std::string, 
                                       std::unordered_map<std::string, std::shared_ptr<SymbolTable>>>& t) : tables(t), error(false) {
                                       for (auto& [k, v] : tables) {
                                           for (auto& [k2, v2] : v) {
                                                if (k != "")
                                                    scopes[k + "." + k2] = v2;
                                           }
                                       }
}

bool TypeLinkingVisitor::isError() const {
    return error;
}

void TypeLinkingVisitor::visit(std::shared_ptr<ImportStatements> n) {
    for (auto stmt: n->stmts) {
        std::string pkg = stmt->package->name;
        std::string cdecl = stmt->id->name;
        if (!tables.contains(pkg)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: ImportStatements: " << pkg << "." << cdecl << std::endl;
        }
        else if (cdecl != "*" && !tables.at(pkg).contains(cdecl)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: ImportStatements: " << pkg << "." << cdecl << std::endl;
        }

        //add import to current scope
        if (cdecl == "*") {
            for (auto& [k, v] : tables[pkg]) {

                // Check for conflict with java.lang.*
                if (pkg != "java.lang" && tables.at("java.lang").contains(k)) {
                    error = true;
                    std::cerr << "Error: TypeLinkingVisitor: ImportStatements: Class name conflict with java.lang: " << k << std::endl;
                    continue;
                }

                if (scopes.contains(k)) {
                    error = true;
                    std::cerr << "Error: TypeLinkingVisitor: Ambiguous class name"  << std::endl;
                }
                scopes[k] = v;
            }
        } else {
            if (scopes.contains(cdecl)) {
                error = true;
                std::cerr << "Error: TypeLinkingVisitor: Ambiguous class name"  << std::endl;
            }
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
    std::string cdecl {n->classOrInterfaceDecl->id->name};
    if(n->package) {
        std::string pkg = n->package->id->name;
        for (auto& [k, v] : tables[pkg]) {
            if (scopes.contains(k)) {
                error = true;
                std::cerr << "Error: TypeLinkingVisitor: Ambiguous class name"  << std::endl;
            }
            scopes[k] = v;
        }
        for (auto& [k, v] : tables) {
            if (k.starts_with(pkg + "." + cdecl)) {
                error = true;
                std::cerr << "Error: TypeLinkingVisitor: When a fully qualified name resolves to a type, no strict prefix of the fully qualified name can resolve to a type in the same environment."  << std::endl;
            }
        }
    } else {
        scopes[cdecl] = tables[""][cdecl];
    }
    Visitor::visit(n);
}

void TypeLinkingVisitor::visit(std::shared_ptr<LocalVariableDeclarationStatement> n) {
    if (auto ptr = std::dynamic_pointer_cast<IdentifierType>(n->type)) {
        if (!scopes.contains(ptr->id->name)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: TYPE_LINKING, UNRESOLVED_TYPE " + ptr->id->name << std::endl;
        }
    }
    Visitor::visit(n);
}
