#include "typeLinkingVisitor.hpp"

TypeLinkingVisitor::TypeLinkingVisitor(std::unordered_map<std::string, 
                                       std::unordered_map<std::string, std::shared_ptr<SymbolTable>>>& t) : tables(t), error(false) {
                                       // add all qualified names to the current scope (e.g. A.B.C.D)
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
        // if the package name is not in the tables, reports an error
        if (!tables.contains(pkg)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: ImportStatements: " << pkg << "." << cdecl << std::endl;
        }
        // if the single type is not in the package, reports an error
        else if (cdecl != "*" && !tables.at(pkg).contains(cdecl)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: ImportStatements: " << pkg << "." << cdecl << std::endl;
        }

        //add import to current scope
        //on-demand import, reports an error when name is ambiguous and it is used
        if (cdecl == "*") {
            for (auto& [k, v] : tables[pkg]) {
                if (scopes.contains(k)) {
                    auto oldPkg = scopes.at(k)->getAst()->package;
                    std::string oldPkgName = oldPkg ? oldPkg->id->name : ""; 
                    // ambiguous class name 
                    if (pkg != oldPkgName) {
                        ambiguousNames.insert(k);
                    }
                } else {
                    scopes[k] = v;
                }
            }
        }
        //single-type import
        else {
            if (scopes.contains(cdecl)) {   
                auto oldPkg = scopes[cdecl]->getAst()->package;
                std::string oldPkgName = oldPkg ? oldPkg->id->name : "";    
                // current scope has two different classes with the same name    
                if (pkg != oldPkgName) {
                    error = true;
                    std::cerr << "Error: TypeLinkingVisitor: Ambiguous class name"  << std::endl;
                }
            } else {
                scopes[cdecl] = tables[pkg][cdecl];
            }
        }

        stmt->accept(this);
    }
}

void TypeLinkingVisitor::visit(std::shared_ptr<FieldAccessExp> n) {
    Visitor::visit(n);
}

void TypeLinkingVisitor::visit(std::shared_ptr<ClassDecl> n) {
    currentClassName = n->id->name;

    for (auto e : n->extended) {
        if (!scopes.contains(e->id->name) || ambiguousNames.contains(e->id->name)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: Invalid base class: " << e->id->name << std::endl;
        }
        e->accept(this);
    }
    for (auto i : n->implemented) {
        if (!scopes.contains(i->id->name) || ambiguousNames.contains(i->id->name)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: Invalid interface: " << i->id->name << std::endl;
        }
        i->accept(this);
    }
    for (int i = 0; i < 3; i++) {
        for (auto d : n->declarations[i]) d->accept(this);
    }
}

void TypeLinkingVisitor::visit(std::shared_ptr<InterfaceDecl> n) {
    currentClassName = n->id->name;
    n->id->accept(this);
    for (auto e : n->extended) {
        if (!scopes.contains(e->id->name) || ambiguousNames.contains(e->id->name)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: Invalid interface: " << e->id->name << std::endl;
        }
        e->accept(this);
    }
    for (auto& m : n->methods) {
        m->accept(this);
    }
}

void TypeLinkingVisitor::visit(std::shared_ptr<Program> n) {
    if (n->package) {
        currentPackageName = n->package->id->name;
    } else {
        currentPackageName = "";
    }

    //add all scopes to the symbol table
    std::string cdecl {n->classOrInterfaceDecl->id->name};
    std::string pkg = n->package ? n->package->id->name : "";

    //package name is not the same as the class/interface name
    if (pkg == cdecl) {
        error = true;
        std::cerr << "Error: TypeLinkingVisitor: When a fully qualified name resolves to a type, no strict prefix of the fully qualified name can resolve to a type in the same environment."  << std::endl;
    }
    //add all classes in the package to the current scope (including the default package)
    for (auto& [k, v] : tables[pkg]) {
        //if package contains duplicate class/interface, reports an error
        if (scopes.contains(k)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: Ambiguous class name"  << std::endl;
        }
        scopes[k] = v;
    }
    //if part of the package name is also a type, reports an error
    for (auto& [k, v] : tables) {
        if (k.starts_with(pkg + "." + cdecl)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: When a fully qualified name resolves to a type, no strict prefix of the fully qualified name can resolve to a type in the same environment."  << std::endl;
        }
    }
    Visitor::visit(n);
}

void TypeLinkingVisitor::visit(std::shared_ptr<LocalVariableDeclarationStatement> n) {
    if (auto ptr = std::dynamic_pointer_cast<IdentifierType>(n->type)) {
        if (!scopes.contains(ptr->id->name) || ambiguousNames.contains(ptr->id->name)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: TYPE_LINKING, UNRESOLVED_TYPE " + ptr->id->name << std::endl;
        }
    }
    Visitor::visit(n);
}
