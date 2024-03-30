#include <queue>
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

AmbiguousName Scope::reclassifySimpleAmbiguousName(const std::string& name) {
    if (current->getVar(name)) return reclassifyAmbiguousNameByLocal(name);
    if (current->getField(name)) {
        bool error{false};
        if (!current->getScopeType(ScopeType::ASSIGNMENT) && current->getScopeType(ScopeType::FIELDINITIALIZER) && !current->isFieldDeclared(name)) error = true;
        else if (current->getScopeType(ScopeType::STATIC) && !current->getField(name)->isStatic) error = true;
        return error ? AmbiguousName(AmbiguousNamesType::ERROR, nullptr) : reclassifyAmbiguousNameByField(name, current, false);
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

AmbiguousName Scope::reclassifyQualifiedAmbiguousName(const std::string& name) {
    std::istringstream qualifiedName(name);
    std::string segment;
    AmbiguousName ambiguousName;
    while (std::getline(qualifiedName, segment, '.')) {
        if (ambiguousName.type == AmbiguousNamesType::ERROR) break;
        if (ambiguousName.type == AmbiguousNamesType::UNINITIALIZED) {
            ambiguousName = reclassifySimpleAmbiguousName(segment);
        }
        else {
            switch (ambiguousName.type) {
                case AmbiguousNamesType::EXPRESSION:
                    if ((ambiguousName.symbolTable != nullptr)) {
                        // only call non-static field/methods
                        if (ambiguousName.symbolTable->getField(segment) != nullptr) {
                            // ambiguousName.symbolTable->getField(segment)
                            ambiguousName = reclassifyAmbiguousNameByField(segment, ambiguousName.symbolTable, false);
                        } else {
                            ambiguousName = AmbiguousName(AmbiguousNamesType::ERROR, nullptr);
                        }
                    }
                    else if (ambiguousName.getDataType() == DataType::ARRAY) {
                        if (segment == "length" && !current->getScopeType(ScopeType::ASSIGNMENT)) {
                            ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, nullptr);
                            ambiguousName.typeNode = std::make_shared<IntType>();
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

AmbiguousName Scope::reclassifyAmbiguousName(const std::string& name, bool simple) {
    if (simple) return reclassifySimpleAmbiguousName(name);
    return reclassifyQualifiedAmbiguousName(name);
}

AmbiguousName Scope::reclassifyAmbiguousNameByLocal(const std::string& name) {
    auto var = current->getVar(name);
    AmbiguousName ambiguousName;
    if (auto fp = std::dynamic_pointer_cast<FormalParameter>(var)) ambiguousName = createAmbiguousName(fp->type, current);
    else if (auto local = std::dynamic_pointer_cast<LocalVariableDeclarationStatement>(var)) ambiguousName = createAmbiguousName(local->type, current);
    return ambiguousName;
}

AmbiguousName Scope::reclassifyAmbiguousNameByField(const std::string& name, std::shared_ptr<SymbolTable> st, bool staticField) {
    auto field = st->getField(name);
    AmbiguousName ambiguousName = createAmbiguousName(field->type, st);
    if (staticField != field->isStatic || (field->isProtected && st->getPackage() != current->getPackage() 
    && !superBFS(current, st, true)))
        ambiguousName.type = AmbiguousNamesType::ERROR;
    return ambiguousName;
}

AmbiguousName Scope::createAmbiguousName(std::shared_ptr<Type> typeNode, std::shared_ptr<SymbolTable> st) {
    AmbiguousName ambiguousName(AmbiguousNamesType::EXPRESSION, nullptr);
    ambiguousName.typeNode = typeNode;
    if (auto type = std::dynamic_pointer_cast<ArrayType>(typeNode)) typeNode = type->dataType;
    if (auto type = std::dynamic_pointer_cast<IdentifierType>(typeNode)) {
        ambiguousName.symbolTable = st->getScope()->getNameInScope(type->id->name, type->simple);
        if (ambiguousName.symbolTable == nullptr) ambiguousName.type = AmbiguousNamesType::ERROR;
    }
    return ambiguousName;
}

bool Scope::superBFS(std::shared_ptr<SymbolTable>& start, std::shared_ptr<SymbolTable>& end, bool strictSubclass) {
    std::queue<std::shared_ptr<SymbolTable>> queue;
    if (strictSubclass && start.get() == end.get()) return false;
    //otherwise, either strictSubclass == false, or strictSubclass == true and start.get() != end.get()
    queue.push(start);
    while (queue.empty() == false) {
        auto top = queue.front();
        queue.pop();
        if (top.get() == end.get()) return true;
        for (auto super : top->getScope()->supers)
            queue.push(super);
    }
    return false;
}
