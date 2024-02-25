#include "scope.hpp"

Scope::Scope(std::shared_ptr<SymbolTable> c, std::shared_ptr<PackageTrie> p) : current(c), pkgTrie(p){
}

std::shared_ptr<SymbolTable> Scope::getNameInScope(const std::string& name, bool simple) const {
    if (simple) return getUnqualifiedNameInScope(name);
    return getQualifiedNameInScope(name);
}

std::shared_ptr<SymbolTable> Scope::getQualifiedNameInScope(const std::string& name) const {
    auto pos = std::find(name.begin(), name.end(), '.');
    std::string simpleName {name.begin(), pos};
    if (getUnqualifiedNameInScope(simpleName)) return nullptr;
    return pkgTrie->getSymbolTable(name);
}

std::shared_ptr<SymbolTable> Scope::getUnqualifiedNameInScope(const std::string& name) const {
    //try the enclosing class or interface 
    if (name == current->getClassOrInterfaceDecl()->id->name) return current;
    //try any single-type-import (A.B.C.D)
    if (singleImported.contains(name)) return singleImported.at(name);
    //try the same package 
    if (packageMembers.contains(name)) return packageMembers.at(name);
    //try any import-on-demand package (A.B.C.*) including java.lang.*
    if (onDemandImported.contains(name) && !ambiguousNames.contains(name)) return onDemandImported.at(name);
    return nullptr;
}

bool Scope::isNameValidInScope(const std::string& name) const {
    if (current->getVar(name)|| current->getField(name)) return true;
    return false;
}

AmbiguousName Scope::reclassifySimpleAmbiguousName(const std::string& name, bool initialized) {
    if (current->getVar(name)) return reclassifyAmbiguousNameByLocal(name);
    if (current->getField(name)) {
        if (initialized && !current->isFieldDeclared(name)) return AmbiguousName(AmbiguousNamesType::ERROR, nullptr);
        return reclassifyAmbiguousNameByField(name, current, false);
    }
    if (name == current->getClassOrInterfaceDecl()->id->name) return AmbiguousName(AmbiguousNamesType::TYPE, current);
    //try any single-type-import (A.B.C.D)
    if (singleImported.contains(name)) return AmbiguousName(AmbiguousNamesType::TYPE, singleImported.at(name));
    //try the same package 
    if (packageMembers.contains(name)) return AmbiguousName(AmbiguousNamesType::TYPE, packageMembers.at(name));
    //try any import-on-demand package (A.B.C.*) including java.lang.*
    if (onDemandImported.contains(name)) {
        if (ambiguousNames.contains(name)) return AmbiguousName(AmbiguousNamesType::ERROR, nullptr);
        return AmbiguousName(AmbiguousNamesType::TYPE, onDemandImported.at(name));
    }
    if (pkgTrie->root->children.contains(name)) return AmbiguousName(pkgTrie->root->children.at(name));
    return AmbiguousName(AmbiguousNamesType::ERROR, nullptr);
}

AmbiguousName Scope::reclassifyQualifiedAmbiguousName(const std::string& name, bool initialized) {
    std::istringstream qualifiedName(name);
    std::string segment;
    AmbiguousName ambiguousName;
    //a.length;
    // to do consider basic cases
    while (std::getline(qualifiedName, segment, '.')) {
        if (ambiguousName.type == AmbiguousNamesType::ERROR) break;
        if (ambiguousName.type == AmbiguousNamesType::UNINITIALIZED) {
            ambiguousName = reclassifySimpleAmbiguousName(segment, initialized);
        }
        else {
            switch (ambiguousName.type) {
                case AmbiguousNamesType::EXPRESSION:
                    if ((ambiguousName.symbolTable != nullptr)) {
                        // only call non-static field/methods
                        if (ambiguousName.symbolTable->getField(segment) != nullptr) {
                            // ambiguousName.symbolTable->getField(segment)
                            ambiguousName = reclassifyAmbiguousNameByField(segment, ambiguousName.symbolTable, false);
                        }
                        else if(!ambiguousName.symbolTable->getMethod(segment).empty()) {
                            // to do, select the closest method
                            ambiguousName = reclassifyAmbiguousNameByMethod(segment, ambiguousName.symbolTable, false);
                        } else {
                            ambiguousName = AmbiguousName(AmbiguousNamesType::ERROR, nullptr);
                        }
                    }
                    else if (ambiguousName.getDataType() == DataType::ARRAY) {
                        if (segment == "length") {
                            ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, nullptr);
                        }
                        else {
                            ambiguousName = AmbiguousName(AmbiguousNamesType::ERROR, nullptr);
                        }
                    }
                    else {
                        ambiguousName = AmbiguousName(AmbiguousNamesType::ERROR, nullptr);
                    }
                    break;
                case AmbiguousNamesType::TYPE:
                    // only call static fields/methods
                    if (ambiguousName.symbolTable != nullptr) {
                        if (ambiguousName.symbolTable->getField(segment) != nullptr) {
                            ambiguousName = reclassifyAmbiguousNameByField(segment, ambiguousName.symbolTable, true);
                        }
                        else if (!ambiguousName.symbolTable->getMethod(segment).empty()) {
                            // to do, select the closest method
                            ambiguousName = reclassifyAmbiguousNameByMethod(segment, ambiguousName.symbolTable, true);
                        } else {
                            ambiguousName = AmbiguousName(AmbiguousNamesType::ERROR, nullptr);
                        }
                    }
                    else {
                        ambiguousName = AmbiguousName(AmbiguousNamesType::ERROR, nullptr);
                    }
                    break;
                case AmbiguousNamesType::PACKAGE:
                    if (ambiguousName.trie->children[segment] != nullptr) {
                        if (ambiguousName.trie->children[segment]->symbolTable != nullptr) {
                            ambiguousName = AmbiguousName(AmbiguousNamesType::TYPE, ambiguousName.trie->children[segment]->symbolTable);
                        } else {
                            ambiguousName.trie = ambiguousName.trie->children[segment];
                        }
                    } else {
                        ambiguousName = AmbiguousName(AmbiguousNamesType::ERROR, nullptr);
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return ambiguousName;
}

AmbiguousName Scope::reclassifyAmbiguousName(const std::string& name, bool simple, bool initialized) {
    if (simple) return reclassifySimpleAmbiguousName(name, initialized);
    return reclassifyQualifiedAmbiguousName(name, initialized);
}

AmbiguousName Scope::reclassifyAmbiguousNameByLocal(const std::string& name) {
    AmbiguousName ambiguousNames(AmbiguousNamesType::EXPRESSION, nullptr);
    auto var = current->getVar(name);
    if (auto fp = std::dynamic_pointer_cast<FormalParameter>(var)) {
        if (auto fpType = std::dynamic_pointer_cast<IdentifierType>(fp->type)) {
            ambiguousNames.typeNode = fpType;
            ambiguousNames.symbolTable = getUnqualifiedNameInScope(fpType->id->name);
            if (ambiguousNames.symbolTable == nullptr) ambiguousNames.type = AmbiguousNamesType::ERROR;
        }
        else if (auto fpType = std::dynamic_pointer_cast<ArrayType>(fp->type)) {
            ambiguousNames.typeNode = fpType;
        }
        // todo :: array logic here
    }
    else if (auto local = std::dynamic_pointer_cast<LocalVariableDeclarationStatement>(var)) {
        if (auto localType = std::dynamic_pointer_cast<IdentifierType>(local->type)) {
            ambiguousNames.typeNode = localType;
            ambiguousNames.symbolTable = getUnqualifiedNameInScope(localType->id->name);
            if (ambiguousNames.symbolTable == nullptr) ambiguousNames.type = AmbiguousNamesType::ERROR;
        }
        else if (auto localType = std::dynamic_pointer_cast<ArrayType>(local->type)) {
            ambiguousNames.typeNode = localType;
        }
        // todo :: array logic here
    }
    return ambiguousNames;
}

AmbiguousName Scope::reclassifyAmbiguousNameByField(const std::string& name, std::shared_ptr<SymbolTable> st, bool staticField) {
    AmbiguousName ambiguousNames(AmbiguousNamesType::EXPRESSION, nullptr);
    auto field = st->getField(name);
    if (auto fieldType = std::dynamic_pointer_cast<IdentifierType>(field->type)) {
        ambiguousNames.typeNode = fieldType;
        ambiguousNames.symbolTable = st->getScope()->getUnqualifiedNameInScope(fieldType->id->name);
        if (ambiguousNames.symbolTable == nullptr) ambiguousNames.type = AmbiguousNamesType::ERROR;
    }
    else if (auto fieldType = std::dynamic_pointer_cast<ArrayType>(field->type)) {
            ambiguousNames.typeNode = fieldType;
    }
    if (staticField && !field->isStatic) ambiguousNames.type = AmbiguousNamesType::ERROR;
    // todo :: array logic here
    return ambiguousNames;
}

AmbiguousName Scope::reclassifyAmbiguousNameByMethod(const std::string& name, std::shared_ptr<SymbolTable> st, bool staticMethod) {
    AmbiguousName ambiguousNames(AmbiguousNamesType::EXPRESSION, nullptr);
    // todo: select the closest method, currently just selects the first one
    auto method = st->getMethod(name)[0];
    if (auto methodType = std::dynamic_pointer_cast<IdentifierType>(method->type)) {
        ambiguousNames.typeNode = methodType;
        ambiguousNames.symbolTable = st->getScope()->getUnqualifiedNameInScope(methodType->id->name);
        if (ambiguousNames.symbolTable == nullptr) ambiguousNames.type = AmbiguousNamesType::ERROR;
    }
    else if (auto methodType = std::dynamic_pointer_cast<ArrayType>(method->type)) {
        ambiguousNames.typeNode = methodType;
    }
    if (staticMethod && !method->isStatic) ambiguousNames.type = AmbiguousNamesType::ERROR;
    // todo :: array logic here
    return ambiguousNames;
}
