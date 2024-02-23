#include <iostream>
#include <sstream>
#include "typeLinkingVisitor.hpp"

TypeLinkingVisitor::TypeLinkingVisitor(std::shared_ptr<Scope> s) :scope(s), error(false) {
}

bool TypeLinkingVisitor::isError() const {
    return error;
}

void TypeLinkingVisitor::visit(std::shared_ptr<ImportStatements> n) {
    for (auto stmt: n->stmts) {
        std::string pkg = stmt->package->name;
        std::string cdecl = stmt->id->name;
        std::string qualifiedName {pkg + "." + cdecl};
        // if the package name is not in the tables, reports an error
        bool validPkg {true};

        if (cdecl == "*") {
            if (!scope->pkgTrie->isPackage(pkg)) validPkg = false;
            else {
                for (auto& [cdecl, table] : scope->pkgTrie->getMemberTypes(pkg)) {
                    if (scope->onDemandImported.contains(cdecl) && scope->onDemandImported[cdecl]->getPackage() != pkg) {
                        scope->ambiguousNames.insert(cdecl);
                    } else {
                        scope->onDemandImported[cdecl] = table;
                    }      
                }
            }
        } else {
            std::shared_ptr<SymbolTable> table = scope->pkgTrie->getSymbolTable(qualifiedName);
            if (table == nullptr) validPkg = false;
            if (cdecl == currentClassName && pkg != currentPackageName) {
                error = true;
                std::cerr << "Error: TypeLinkingVisitor: No single-type-import declaration clashes with the class or interface declared in the same file" << std::endl;
            }
            if(scope->singleImported.contains(cdecl) && scope->singleImported[cdecl]->getPackage() != pkg) {
                error = true;
                std::cerr << "Error: TypeLinkingVisitor: Ambiguous class name"  << std::endl;
            }
            if (table) scope->singleImported[cdecl] = table;
        }

        if (!validPkg) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: ImportStatements: " << pkg << "." << cdecl << std::endl;
        }
        stmt->accept(this);
    }
}

void TypeLinkingVisitor::visit(std::shared_ptr<FieldAccessExp> n) {
    Visitor::visit(n);
}

void TypeLinkingVisitor::visit(std::shared_ptr<ClassDecl> n) {

    for (auto e : n->extended) {
        if (!scope->getNameInScope(e->id->name, e->simple)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: UNRESOLVED_TYPE " + e->id->name << std::endl;
        }
        e->accept(this);
    }
    for (auto i : n->implemented) {
        if (!scope->getNameInScope(i->id->name, i->simple)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: UNRESOLVED_TYPE " + i->id->name << std::endl;
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
        if (!scope->getNameInScope(e->id->name, e->simple)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: UNRESOLVED_TYPE " + e->id->name << std::endl;
        }
        e->accept(this);
    }
    for (auto& m : n->methods) {
        m->accept(this);
    }
}

void TypeLinkingVisitor::visit(std::shared_ptr<Program> n) {
    currentClassName = n->classOrInterfaceDecl->id->name;
    currentPackageName = n->package->id ? n->package->id->name : "";
    for (auto& [cdecl, table]: scope->pkgTrie->getMemberTypes(currentPackageName)) {
        scope->packageMembers[cdecl] = table;
    }
    Visitor::visit(n);
}

// void TypeLinkingVisitor::visit(std::shared_ptr<IdentifierType> n) {
//     if (!isNameInScope(n->id->name, n->simple)) {
//         error = true;
//         std::cerr << "Error: TypeLinkingVisitor: UNRESOLVED_TYPE " + n->id->name << std::endl;
//     }
// }

void TypeLinkingVisitor::visit(std::shared_ptr<LocalVariableDeclarationStatement> n) {
    if (auto ptr = std::dynamic_pointer_cast<IdentifierType>(n->type)) {
        if (!scope->getNameInScope(ptr->id->name, ptr->simple)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: UNRESOLVED_TYPE " + ptr->id->name << std::endl;
        }
    }
    Visitor::visit(n);
}

void TypeLinkingVisitor::visit(std::shared_ptr<ClassInstanceCreationExp> n) {
    if (!scope->getNameInScope(n->classType->id->name, n->classType->simple)) {
        error = true;
        std::cerr << "Error: TypeLinkingVisitor: UNRESOLVED_TYPE " + n->classType->id->name << std::endl;
    }
    Visitor::visit(n);
}
