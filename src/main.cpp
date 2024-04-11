#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "PackageTrie.hpp"
#include "parser.h"
#include "contextVisitor.hpp"
#include "symbolTable.hpp"
#include "typeCheckingVisitor.hpp"
#include "typeLinkingVisitor.hpp"
#include "hierarchyVisitor.hpp"
#include "transformVisitor.hpp"
#include "IRAst.hpp"
#include "Simulator.hpp"
#include "canonicalVisitor.hpp"
#include "utils.h"
#include "IRCfgVisitor.hpp"
#include <Tiling.hpp>
#include "cfgVisitor.hpp"

std::unordered_map<DataType, DataType> arrayDataTypes = {
    {DataType::VOID, DataType::VOIDARRAY}, {DataType::INT, DataType::INTARRAY}, {DataType::BOOLEAN, DataType::BOOLEANARRAY}, {DataType::CHAR, DataType::CHARARRAY}, {DataType::BYTE, DataType::BYTEARRAY}, {DataType::SHORT, DataType::SHORTARRAY}, {DataType::LONG, DataType::LONGARRAY}, {DataType::FLOAT, DataType::FLOATARRAY}, {DataType::DOUBLE, DataType::DOUBLEARRAY}, {DataType::OBJECT, DataType::OBJECTARRAY}};

std::string extractFilename(const std::string &filePath)
{
    size_t start = filePath.rfind('/') + 1; // Find the last '/' character
    size_t end = filePath.rfind('.');       // Find the last '.' character
    if (start == std::string::npos || end == std::string::npos || end <= start)
    {
        return ""; // Return an empty string if the format is not as expected
    }
    return filePath.substr(start, end - start); // Extract the substring representing the filename
}

bool DFS(std::string current, std::unordered_set<std::string> &visited, std::unordered_set<std::string> &pathVisited,
         std::unordered_map<std::string, std::vector<std::string>> &edges)
{
    visited.insert(current);
    pathVisited.insert(current);
    for (std::string neighbour : edges[current])
    {
        if (pathVisited.contains(neighbour))
            return true;
        else if (visited.contains(neighbour))
            continue;
        else if (DFS(neighbour, visited, pathVisited, edges))
            return true;
    }
    pathVisited.erase(current);
    return false;
}

void generateAssemblyFile(const std::string &filename, const std::vector<std::string> &assemblyCodes)
{
    std::ofstream outFile("output/" + filename);
    if (!outFile.is_open())
    {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    for (const auto &line : assemblyCodes)
    {
        outFile << line << "\n";
    }

    outFile.close();
}

template <typename T>
bool detectCycle(std::vector<std::shared_ptr<T>> decls)
{
    // prepare the graph
    std::vector<std::string> vertices;
    std::unordered_map<std::string, std::vector<std::string>> edges;
    for (auto decl : decls)
    {
        vertices.push_back(decl->id->name);                 // add all classes/interfaces to vector of vertices
        edges[decl->id->name] = std::vector<std::string>{}; // initialize edges vector for each class/interface
        for (auto ext : decl->extended)
        {                                                   // go through neighbours
            edges[decl->id->name].push_back(ext->id->name); // add them to the list of edges
        }
    }
    // perform DFS
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> pathVisited;
    for (std::string vertex : vertices)
    {
        if (!edges.contains(vertex))
            continue;
        if (visited.contains(vertex))
            continue;
        else
        {
            bool result = DFS(vertex, visited, pathVisited, edges);
            if (result)
                return result;
        }
    }
    return false;
}

std::pair<DataType, std::string> getTypeValuePair(const std::shared_ptr<Type> &type)
{
    DataType typeValue = type->type;
    std::string objectName = "";
    if (typeValue == DataType::OBJECT)
    {
        std::shared_ptr<IdentifierType> idType = std::dynamic_pointer_cast<IdentifierType>(type);
        assert(idType != nullptr);
        objectName = idType->id->name;
    }
    else if (typeValue == DataType::ARRAY)
    {
        std::shared_ptr<ArrayType> arrayType = std::dynamic_pointer_cast<ArrayType>(type);
        assert(arrayType != nullptr);
        typeValue = arrayDataTypes[arrayType->dataType->type];
        if (typeValue == DataType::OBJECTARRAY)
        {
            std::shared_ptr<IdentifierType> idType = std::dynamic_pointer_cast<IdentifierType>(arrayType->dataType);
            assert(idType != nullptr);
            objectName = idType->id->name;
        }
    }
    return std::make_pair(typeValue, objectName);
}

struct PairStringVectorHash
{
    size_t operator()(const std::pair<std::string, std::vector<std::pair<DataType, std::string>>> &pair) const
    {
        std::hash<std::string> stringHasher;
        size_t seed = stringHasher(pair.first);
        for (const auto &innerPair : pair.second)
        {
            seed ^= std::hash<DataType>{}(innerPair.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= stringHasher(innerPair.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

void populateIMTables(std::shared_ptr<Program> n, bool &error)
{

    assert(n->scope->current->imtablesPopulated == false);
    n->scope->current->imtablesPopulated = true;
    auto &iscmtable = n->scope->current->getISCMTable();
    auto &isimtable = n->scope->current->getISIMTable();

    // signature is pair<methodName, list of pair<paramType, objectName>> where objectName == "" for paramType != OBJECT, OBJECTARRAY
    // mtableSignaturesToReturnTypes maps signatures to return types, which is a pair<paramType, objectName> as above
    std::unordered_map<std::pair<std::string, std::vector<std::pair<DataType, std::string>>>, std::pair<DataType, std::string>, PairStringVectorHash>
        mtableSignaturesToReturnTypes;

    // for entries in our mtable,
    for (auto &p : n->scope->current->getMTable())
    {
        std::string methodName = p.first;
        // for each element in entry.second
        for (auto &method : p.second)
        {
            // make pair<mname, paramtypes> and map it to the method's return type
            std::vector<std::pair<DataType, std::string>> paramTypes{};
            for (auto &fp : method->formalParameters)
            {
                paramTypes.push_back(getTypeValuePair(fp->type));
            }
            std::pair<std::string, std::vector<std::pair<DataType, std::string>>> signature = std::make_pair(methodName, paramTypes);
            mtableSignaturesToReturnTypes[signature] = getTypeValuePair(method->type);
        }
    }

    auto processTable = [&](const auto &table, auto &writeTable, const bool defaultAddToTable, bool &error)
    {
        for (auto p : table)
        {
            std::string methodName = p.first;
            for (auto method : p.second)
            {
                std::vector<std::pair<DataType, std::string>> paramTypes{};
                for (std::shared_ptr<FormalParameter> fp : method->formalParameters)
                {
                    paramTypes.push_back(getTypeValuePair(fp->type));
                }
                auto signature = std::make_pair(methodName, paramTypes);
                auto returnType = getTypeValuePair(method->type);
                if (mtableSignaturesToReturnTypes.contains(signature))
                {
                    if (mtableSignaturesToReturnTypes[signature] != returnType)
                    {
                        std::cerr << "Error: Method is inherited with identical signature but different return type from existing method" << std::endl;
                        error = true;
                        return;
                    }
                }
                else if (!defaultAddToTable)
                    writeTable[methodName].push_back(method);
                if (defaultAddToTable)
                    writeTable[methodName].push_back(method);
            }
        }
    };

    // if we're an InterfaceDecl
    std::shared_ptr<InterfaceDecl> idecl = std::dynamic_pointer_cast<InterfaceDecl>(n->classOrInterfaceDecl);
    if (idecl != nullptr)
    {
        // for supers
        for (std::shared_ptr<SymbolTable> superTable : n->scope->supers)
        {
            if (!superTable)
                continue; // THIS SHOULD NEVER TRIGGER
            // recursively populate super imtable
            if (superTable->imtablesPopulated == false)
                populateIMTables(superTable->getAst(), error);
            if (error)
                return;
            // add extended interface methods to isimtable unless overridden
            std::shared_ptr<InterfaceDecl> sidecl = std::dynamic_pointer_cast<InterfaceDecl>(superTable->getClassOrInterfaceDecl());
            if (sidecl != nullptr)
            {
                processTable(superTable->getMTable(), isimtable, false, error);
                processTable(superTable->getISIMTable(), isimtable, false, error);
            } // otherwise, most likely an interface is extending a class, which will be caught later in hierarchy checking
        }
    }
    else
    {
        // assert we're a ClassDecl
        std::shared_ptr<ClassDecl> cdecl = std::dynamic_pointer_cast<ClassDecl>(n->classOrInterfaceDecl);
        assert(cdecl != nullptr);
        // for supers
        for (std::shared_ptr<SymbolTable> superTable : n->scope->supers)
        {
            if (!superTable)
                continue; // THIS SHOULD NEVER TRIGGER
            // recursively populate super imtable
            if (superTable->imtablesPopulated == false)
                populateIMTables(superTable->getAst(), error);
            if (error)
                return;
            // if super is an InterfaceDecl, add extended superinterface methods to isimtable unless there's an error
            std::shared_ptr<InterfaceDecl> sidecl = std::dynamic_pointer_cast<InterfaceDecl>(superTable->getClassOrInterfaceDecl());
            if (sidecl != nullptr)
            {
                processTable(superTable->getMTable(), isimtable, true, error);
                processTable(superTable->getISIMTable(), isimtable, true, error);
            }
            else
            {
                // add extended superclass methods to iscmtable unless overridden
                std::shared_ptr<ClassDecl> scdecl = std::dynamic_pointer_cast<ClassDecl>(superTable->getClassOrInterfaceDecl());
                assert(scdecl != nullptr);
                processTable(superTable->getMTable(), iscmtable, false, error);
                processTable(superTable->getISCMTable(), iscmtable, false, error);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    std::unordered_map<std::string,
                       std::unordered_map<std::string, std::shared_ptr<SymbolTable>>>
        tables;
    std::shared_ptr<PackageTrie> pkgTrie = std::make_shared<PackageTrie>();
    std::vector<std::shared_ptr<Program>> asts;
    bool error{false};
    for (int i = 1; i < argc; ++i)
    {
        std::ifstream inputFile;
        inputFile.open(argv[i]);
        if (!inputFile.is_open())
        {
            std::cerr << "Error: Unable to open file " << argv[i] << std::endl;
            return 1;
        }
        std::string filename{extractFilename(argv[i])};
        yy::MyLexer lexer(*static_cast<std::istream *>(&inputFile), filename);
        Ast ast;
        yy::parser parser(lexer, ast);
        int ret = parser.parse();
        if (ret != 0)
        {
            error = true;
            break;
        }
        if (std::shared_ptr<Program> program = ast.getAst())
        {
            // PrintVisitor visitor;
            ContextVisitor cvisitor{};
            program->accept(&cvisitor);
            auto [pkg, cdecl] = program->getQualifiedName();
            std::string qualifiedName{pkg + "." + cdecl};
            auto symbolTable = cvisitor.getSymbolTable();
            if (cvisitor.isError() || !pkgTrie->insert(qualifiedName, symbolTable))
            {
                std::cerr << "Error: Environment Building failed" << std::endl;
                error = true;
                break;
            }
            program->scope = std::make_shared<Scope>(symbolTable, pkgTrie);
            symbolTable->setScope(program->scope);
            asts.push_back(program);
        }
    }

    // partition class and interface declarations into two vectors, and populate each scope's supers as well
    std::vector<std::shared_ptr<ClassDecl>> classDecls;
    std::vector<std::shared_ptr<InterfaceDecl>> interfaceDecls;

    for (auto program : asts)
    {
        if (!program)
            continue;
        if (program->classOrInterfaceDecl)
        {
            // Check if this program holds a ClassDecl
            std::shared_ptr<ClassDecl> classDecl = std::dynamic_pointer_cast<ClassDecl>(program->classOrInterfaceDecl);
            if (classDecl != nullptr)
                // if yes, add to classDecls but also add implemented list to supers
                classDecls.push_back(classDecl);
            else
            {
                // otherwise, ensure that program holds an InterfaceDecl, and add to interfaceDecls
                std::shared_ptr<InterfaceDecl> interfaceDecl = std::dynamic_pointer_cast<InterfaceDecl>(program->classOrInterfaceDecl);
                assert(interfaceDecl != nullptr);
                interfaceDecls.push_back(interfaceDecl);
            }
        }
    }

    bool classCycle = detectCycle<ClassDecl>(classDecls);
    bool interfaceCycle = detectCycle<InterfaceDecl>(interfaceDecls);
    if (classCycle)
    {
        std::cerr << "Error: Detected a cycle among the classes" << std::endl;
        std::cerr << "Error: Hierarchy Checking failed" << std::endl;
        error = true;
    }
    else if (interfaceCycle)
    {
        std::cerr << "Error: Detected a cycle among the interfaces" << std::endl;
        std::cerr << "Error: Hierarchy Checking failed" << std::endl;
        error = true;
    }

    if (!error)
    {
        for (auto ast : asts)
        {
            TypeLinkingVisitor tvisitor{ast->scope};
            ast->accept(&tvisitor);
            if (tvisitor.isError())
            {
                std::cerr << "Error: Type linking failed" << std::endl;
                error = true;
                break;
            }
        }
    }

    // populate supers
    for (auto program : asts)
    {
        if (!program)
            continue;
        if (program->classOrInterfaceDecl)
        {

            // Add extended list to supers
            for (std::shared_ptr<IdentifierType> idType : program->classOrInterfaceDecl->extended)
                program->scope->supers.push_back(program->scope->getNameInScope(idType->id->name, idType->simple));

            // Check if this program holds a ClassDecl
            std::shared_ptr<ClassDecl> classDecl = std::dynamic_pointer_cast<ClassDecl>(program->classOrInterfaceDecl);
            if (classDecl != nullptr)
                // if yes, add implemented list to supers
                for (std::shared_ptr<IdentifierType> idType : classDecl->implemented)
                    program->scope->supers.push_back(program->scope->getNameInScope(idType->id->name, idType->simple));
        }
    }

    if (!error)
    {
        for (auto ast : asts)
            if (ast->scope->current->imtablesPopulated == false)
                populateIMTables(ast, error);
    }

    if (!error)
    {
        for (std::shared_ptr<Program> program : asts)
        {
            HierarchyVisitor hvisitor(program->scope);
            program->accept(&hvisitor);
            if (hvisitor.isError())
            {
                std::cerr << "Error: Hierarchy Checking failed" << std::endl;
                error = true;
                break;
            }
        }
    }

    if (!error)
    {
        // Rule 9: Class/interface can't contain (declare or inherit) two methods with same signature but diff return types
        for (auto ast : asts)
        {
            // std::unordered_map<std::vector<std::pair<DataType, std::string>>, std::pair<DataType, std::string>, PairVectorHash> mapSignatureToReturnType; //stores signatures seen thus far
            std::unordered_map<std::pair<std::string, std::vector<std::pair<DataType, std::string>>>, std::pair<DataType, std::string>, PairStringVectorHash> mapSignatureToReturnType;
            auto processTable = [&](const auto &table, bool &error)
            {
                assert(!error);
                for (const auto &p : table)
                {
                    std::string methodName = p.first;
                    for (std::shared_ptr<Method> method : p.second)
                    {
                        std::vector<std::pair<DataType, std::string>> paramTypes{};
                        for (auto fp : method->formalParameters)
                        {
                            paramTypes.push_back(getTypeValuePair(fp->type));
                        }
                        auto signature = std::make_pair(methodName, paramTypes);
                        auto returnType = getTypeValuePair(method->type);
                        if (mapSignatureToReturnType.contains(signature))
                        {
                            if (mapSignatureToReturnType[signature] != returnType)
                            {
                                std::cerr << "Error: Class/Interface contains two methods with same signature but diff return types" << std::endl;
                                error = true;
                                return;
                            }
                        }
                        else
                            mapSignatureToReturnType[signature] = returnType;
                    }
                }
            };
            // if its an interface,
            std::shared_ptr<InterfaceDecl> idecl = std::dynamic_pointer_cast<InterfaceDecl>(ast->classOrInterfaceDecl);
            if (idecl != nullptr)
            {
                processTable(ast->scope->current->getMTable(), error); // check declared methods
                if (error)
                    break;
                processTable(ast->scope->current->getISIMTable(), error); // check inherited from superinterfaces
                if (error)
                    break;
            }
            else
            {
                // assert its a class
                std::shared_ptr<ClassDecl> cdecl = std::dynamic_pointer_cast<ClassDecl>(ast->classOrInterfaceDecl);
                assert(cdecl != nullptr);
                processTable(ast->scope->current->getMTable(), error); // check declared
                if (error)
                    break;
                processTable(ast->scope->current->getISCMTable(), error); // check inherited from superclass
                if (error)
                    break;
                processTable(ast->scope->current->getISIMTable(), error); // check inherited from superinterface
                if (error)
                    break;
            }
        }
    }

    if (!error)
    {
        for (std::shared_ptr<Program> program : asts)
        {
            TypeCheckingVisitor tcvisitor(program->scope);
            program->accept(&tcvisitor);
            if (tcvisitor.isError())
            {
                std::cerr << "Error: Type Checking failed" << std::endl;
                error = true;
                break;
            }
        }
    }

    if (!error)
    {
        for (std::shared_ptr<Program> program : asts)
        {
            std::cout << program->scope->current->getClassOrInterfaceDecl()->id->name << std::endl;
            CFGVisitor cfgvisitor(program->scope);
            program->accept(&cfgvisitor);
            // break;
        }
    }

    if (!error)
    {
        std::shared_ptr<TIR::NodeFactory_c> nodeFactory = std::make_shared<TIR::NodeFactory_c>();
        std::vector<std::shared_ptr<TIR::Stmt>> staticFields;
        std::unordered_map<std::string, int> staticFieldsMap;
        std::unordered_map<std::string, std::shared_ptr<TIR::FuncDecl>> staticMethodMap;
        std::shared_ptr<TIR::CompUnit> compUnit;
        for (std::shared_ptr<Program> program : asts)
        {
            TransformVisitor tvisitor(program->scope, nodeFactory);
            program->accept(&tvisitor);
            std::shared_ptr<TIR::CompUnit> cu = tvisitor.getCompUnit();
            if (compUnit == nullptr)
                compUnit = cu;
            for (auto fieldDecl : cu->getFields())
            {
                auto field = std::dynamic_pointer_cast<TIR::Temp>(fieldDecl->getTarget());
                staticFieldsMap[field->getName()] = 0;
                staticFields.push_back(fieldDecl);
            }
            for (auto &[k, v] : cu->getFunctions())
            {
                staticMethodMap[k] = v;
            }
        }
        staticFields.push_back(nodeFactory->IRReturn(nodeFactory->IRConst(0)));
        compUnit->setStaticInitFunc(nodeFactory->IRFuncDecl(TIR::Configuration::STATIC_INIT_FUNC, 0, nodeFactory->IRSeq(staticFields)));
        compUnit->setFunctions(staticMethodMap);

        try
        {
            std::shared_ptr<CanonicalVisitor> cvisitor = std::make_shared<CanonicalVisitor>();
            cvisitor->visit(compUnit);
            // {
                // TIR::Simulator sim(compUnit);
                // sim.staticFields = staticFieldsMap;
                // sim.initStaticFields();
                // long result = sim.call(compUnit->getName() + "_test_int");
                // std::cout << "After CanonicalVisitor: program evaluates to " << result << std::endl;
                // staticFieldsMap = sim.staticFields;
            // }
        }
        catch (std::exception &e)
        {
            std::cout << "Error in CanonicalVisitor" << std::endl;
        }

        try
        {
            std::shared_ptr<TIR::CfgVisitor> cfv = std::make_shared<TIR::CfgVisitor>();
            cfv->visit(compUnit);
            std::shared_ptr<TIR::CheckCanonicalIRVisitor> ccv = std::make_shared<TIR::CheckCanonicalIRVisitor>();
            std::cout << "Canonical? " << (ccv->visit(compUnit) ? "Yes" : "No") << std::endl;
            // {
            //     TIR::Simulator sim(compUnit);
            //     sim.staticFields = staticFieldsMap;
            //     sim.initStaticFields();
            //     long result = sim.call(compUnit->getName() + "_test_int");
            //     std::cout << "After CFG: program evaluates to " << result << std::endl;
            //     staticFieldsMap = sim.staticFields;
            // }
        }
        catch (std::exception &e)
        {
            std::cout << "Error in CFG" << std::endl;
        }

        {
            // generate .s file for compUnit

            std::vector<std::string> assemblyCodes;
            assemblyCodes.push_back("extern __malloc");
            assemblyCodes.push_back("extern __exception\n");
            // static data
            assemblyCodes.push_back("section .data");
            for (auto &[key, value] : staticFieldsMap)
            {
                assemblyCodes.push_back(key + ": dd " + std::to_string(value));
            }
            // for (auto instr : assemblyCodes)
            // {
            //     std::cout << instr << std::endl;
            // }
            assemblyCodes.push_back("\nsection .text");
            assemblyCodes.push_back("global _start");
            Tiling tiler;

            assemblyCodes.push_back("_start:"); // Entry point label
            assemblyCodes.push_back("call " + compUnit->getName() + "_test_int");
            assemblyCodes.push_back("mov ebx, eax");
            assemblyCodes.push_back("mov eax, 1");
            assemblyCodes.push_back("int 0x80");

            for (auto &[funcName, funcDecl] : compUnit->getFunctions())
            {
                if (funcName != "Arrays_equals_array_boolean_array_boolean_boolean" && funcName != "Arrays_equals_array_char_array_char_boolean") {
                std::cout << funcName << std::endl;
                tiler.currentStackOffset = 0;
                tiler.tempToStackOffset.clear();
                // std::vector<std::string> assemblyInstructions;

                assemblyCodes.push_back("\n" + funcDecl->getName() + ":");
                assemblyCodes.push_back("push ebp");
                assemblyCodes.push_back("mov ebp, esp");
                for (int i = 0; i < funcDecl->getNumParams(); i++)
                    tiler.tempToStackOffset[Configuration::ABSTRACT_ARG_PREFIX + std::to_string(i)] = 4 * (i + 2);

                // if (!tempToStackOffset.contains(node->getName())) {
                //     tempToStackOffset[node->getName()] = currentStackOffset;
                // }
                // std::cout << funcDecl->getName() << std::endl;

                for (auto stmt : std::dynamic_pointer_cast<Seq>(funcDecl->getBody())->getStmts())
                {
                    // if (std::dynamic_pointer_cast<Call_s>(stmt) != nullptr) {
                    //     assemblyInstructions.push_back("call " + std::dynamic_pointer_cast<Call_s>(stmt)->getTarget()->getName());
                    // }
                    tiler.tileStmt(stmt, assemblyCodes);
                }
                // assemblyCodes.insert(assemblyCodes.end(), assemblyInstructions.begin(), assemblyInstructions.end());
                // for (auto instr : assemblyInstructions)
                // {
                //     std::cout << instr << std::endl;
                // }
                }
            }

            generateAssemblyFile("output.s", assemblyCodes);
        }
    }

    if (error)
        return 42;
    else
        return 0;
}
