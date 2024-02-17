#include <iostream>
#include <sstream>
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
        bool validPkg {false};
        for (auto& [k, v] : tables) {
            std::istringstream iss(k);
            std::string token;
            std::string cur;
            while (std::getline(iss, token, '.')) {
                cur += token;
                if (cur == pkg) {
                    validPkg = true;
                    break;
                }
                cur += ".";
            }
            if (validPkg) break;
        }
        if (!validPkg) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: ImportStatements: " << pkg << "." << cdecl << std::endl;
        }
        // if the single type is not in the package, reports an error
        else if (cdecl != "*" && !tables.at(pkg).contains(cdecl)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: ImportStatements: " << pkg << "." << cdecl << std::endl;
        }

        if (cdecl != "*") {
            if (cdecl == currentClassName && pkg != currentPackageName) {
                error = true;
                std::cerr << "Error: TypeLinkingVisitor: No single-type-import declaration clashes with the class or interface declared in the same file" << std::endl;
            }
            if(singleImported.contains(cdecl)) {
                error = true;
                std::cerr << "Error: TypeLinkingVisitor: Ambiguous class name"  << std::endl;
            }
            singleImported.insert(cdecl);
            scopes[cdecl] = tables[pkg][cdecl];
        }
        //add import to current scope
        //on-demand import, reports an error when name is ambiguous and it is used
        else {
            for (auto& [className, classInfo] : tables[pkg]) {
                if (!scopes.contains(className)) {
                    scopes[className] = classInfo;
                }     
                onDemandImported[className].insert(pkg);           
            }
        }

        stmt->accept(this);
    }
}

void TypeLinkingVisitor::visit(std::shared_ptr<FieldAccessExp> n) {
    Visitor::visit(n);
}

void TypeLinkingVisitor::visit(std::shared_ptr<ClassDecl> n) {

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
    //add all scopes to the symbol table
    std::string cdecl {n->classOrInterfaceDecl->id->name};
    std::string pkg = n->package ? n->package->id->name : "";

    currentPackageName = pkg;
    currentClassName = cdecl;

    //package name is not the same as the class/interface name
    // if (pkg == cdecl) {
    //     error = true;
    //     std::cerr << "Error: TypeLinkingVisitor: When a fully qualified name resolves to a type, no strict prefix of the fully qualified name can resolve to a type in the same environment."  << std::endl;
    // }
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
        // if (!scopes.contains(ptr->id->name) || ambiguousNames.contains(ptr->id->name)) {
        //     error = true;
        //     std::cerr << "Error: TypeLinkingVisitor: TYPE_LINKING, UNRESOLVED_TYPE " + ptr->id->name << std::endl;
        // }
        std::string className{ptr->id->name};
        if(!singleImported.contains(className) && onDemandImported[className].size() > 1) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: Ambiguous class name due to on-demand imports： " + ptr->id->name << std::endl;
        }
        size_t pos = className.find('.');
        if (pos != std::string::npos) {
            // Extract the substring from the beginning up to the position of the period
            std::string s = className.substr(0, pos);
            if (scopes.contains(s)) {
                error = true;
                std::cerr << "Error: TypeLinkingVisitor: When a fully qualified name resolves to a type, no strict prefix of the fully qualified name can resolve to a type in the same environment." << std::endl;
            }
        }
    
    }
    Visitor::visit(n);
}

void TypeLinkingVisitor::visit(std::shared_ptr<ClassInstanceCreationExp> n) {
    std::string className{n->classType->id->name};
    if(!singleImported.contains(className) && onDemandImported[className].size() > 1) {
        error = true;
        std::cerr << "Error: TypeLinkingVisitor: Ambiguous class name due to on-demand imports： " + className << std::endl;
    }
    size_t pos = className.find('.');
    if (pos != std::string::npos) {
        // Extract the substring from the beginning up to the position of the period
        std::string s = className.substr(0, pos);
        if (scopes.contains(s)) {
            error = true;
            std::cerr << "Error: TypeLinkingVisitor: When a fully qualified name resolves to a type, no strict prefix of the fully qualified name can resolve to a type in the same environment." << std::endl;
        }
    }
    if(!singleImported.contains(className) && onDemandImported[className].size() > 1) {
        error = true;
        std::cerr << "Error: TypeLinkingVisitor: Ambiguous class name due to on-demand imports： " + className << std::endl;
    }
    Visitor::visit(n);
}
