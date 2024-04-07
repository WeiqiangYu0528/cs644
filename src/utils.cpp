#include "utils.h"

void PrintCompUnit(std::shared_ptr<TIR::CompUnit> root, const std::string& filename) {
    if (!root) return;

    std::queue<std::shared_ptr<TIR::Node>> queue;
    std::unordered_set<std::shared_ptr<TIR::Node>> visited;
    std::stringstream graph;
    graph << "digraph G {\n";
    graph << "    rankdir=LR;\n";  
    queue.push(root);

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();

        if (visited.contains(current)) continue;
        visited.insert(current);

        std::stringstream addressStream;
        addressStream << "0x" << std::hex << reinterpret_cast<uintptr_t>(current.get());
        std::string label = addressStream.str() + "\n " + current->getLabel();

        if (auto node = std::dynamic_pointer_cast<TIR::CompUnit>(current)) {
            graph << "    \"" << label << "\" [label=\"" << label << "\"];\n";
            for (const auto& func : node->getFunctions()) {
                queue.push(func.second);
                std::stringstream funcLabelStream;
                funcLabelStream << "0x" << std::hex << reinterpret_cast<uintptr_t>(func.second.get()) << "\n " + func.second->getLabel();
                graph << "    \"" << label << "\" -> \"" << funcLabelStream.str() << "\";\n";
            }
            for (const auto& field : node->getFields()) {

                std::stringstream labelStream;
                queue.push(field);
                labelStream << "0x" << std::hex << reinterpret_cast<uintptr_t>(field.get()) << "\n " + field->getLabel();
                graph << "    \"" << label << "\" -> \"" << labelStream.str() << "\";\n";
            }
                
        } else if (auto funcDeclNode = std::dynamic_pointer_cast<TIR::FuncDecl>(current)) {
            graph << "    \"" << label << "\" [label=\"" << label << "\"];\n";
            auto bodyNode = funcDeclNode->getBody();
            if (bodyNode) {
                queue.push(bodyNode);
                std::stringstream bodyLabelStream;
                bodyLabelStream << "0x" << std::hex << reinterpret_cast<uintptr_t>(bodyNode.get()) << "\n " + bodyNode->getLabel();
                graph << "    \"" << label << "\" -> \"" << bodyLabelStream.str() << "\";\n";
            }
        } else if (auto moveNode = std::dynamic_pointer_cast<TIR::Move>(current)) {
            graph << "    \"" << label << "\" [label=\"" << label << "\"];\n";
            auto target = moveNode->getTarget();
            auto source = moveNode->getSource();
            queue.push(target);
            queue.push(source);
            std::stringstream labelStream;
            labelStream << "0x" << std::hex << reinterpret_cast<uintptr_t>(target.get()) << "\n " + target->getLabel();
            graph << "    \"" << label << "\" -> \"" << labelStream.str() << "\";\n";            
            labelStream.str("");
            labelStream << "0x" << std::hex << reinterpret_cast<uintptr_t>(source.get()) << "\n " + source->getLabel();
            graph << "    \"" << label << "\" -> \"" << labelStream.str() << "\";\n";    
        } else if (auto expNode = std::dynamic_pointer_cast<TIR::Exp>(current)) {
            graph << "    \"" << label << "\" [label=\"" << label << "\"];\n";
            auto exp = expNode->getExpr();
            queue.push(exp);
            std::stringstream labelStream;
            labelStream << "0x" << std::hex << reinterpret_cast<uintptr_t>(exp.get()) << "\n " + exp->getLabel();
            graph << "    \"" << label << "\" -> \"" << labelStream.str() << "\";\n";                        
        } else if (auto seqNode = std::dynamic_pointer_cast<TIR::Seq>(current)) {
            graph << "    \"" << label << "\" [label=\"" << label << "\"];\n";
            for (const auto& stmt : seqNode->getStmts()) {
                if (stmt) {
                    queue.push(stmt);
                    std::stringstream stmtLabelStream;
                    stmtLabelStream << "0x" << std::hex << reinterpret_cast<uintptr_t>(stmt.get()) << "\n " + stmt->getLabel();
                    graph << "    \"" << label << "\" -> \"" << stmtLabelStream.str() << "\";\n";
                }
            }
        
        }  else if (auto cJumpNode = std::dynamic_pointer_cast<TIR::CJump>(current)) {
            graph << "    \"" << label << "\" [label=\"" << label << "\"];\n";
            auto cond = cJumpNode->getCond();
            queue.push(cond);     
            std::stringstream labelStream;
            labelStream << "0x" << std::hex << reinterpret_cast<uintptr_t>(cond.get()) << "\n " + cond->getLabel();
            graph << "    \"" << label << "\" -> \"" << labelStream.str() << "\";\n";            
            graph << "    \"" << label << "\" -> \"T label: " << cJumpNode->getTrueLabel() << "\";\n";
            if (cJumpNode->hasFalseLabel())
                graph << "    \"" << label << "\" -> \"F label: " << cJumpNode->getFalseLabel() << "\";\n";
        } else if (auto returnNode = std::dynamic_pointer_cast<TIR::Return>(current)) {
            graph << "    \"" << label << "\" [label=\"" << label << "\"];\n";
            auto ret = returnNode->getRet();
            queue.push(ret);     
            std::stringstream labelStream;
            labelStream << "0x" << std::hex << reinterpret_cast<uintptr_t>(ret.get()) << "\n " + ret->getLabel();
            graph << "    \"" << label << "\" -> \"" << labelStream.str() << "\";\n";            
        }
        else if (auto jumpNode = std::dynamic_pointer_cast<TIR::Jump>(current)) {
            graph << "    \"" << label << "\" [label=\"" << label << "\"];\n";
            auto jmp = jumpNode->getTarget();
            queue.push(jmp);     
            std::stringstream labelStream;
            labelStream << "0x" << std::hex << reinterpret_cast<uintptr_t>(jmp.get()) << "\n " + jmp->getLabel();
            graph << "    \"" << label << "\" -> \"" << labelStream.str() << "\";\n";            
        }
        else if (auto callNode = std::dynamic_pointer_cast<TIR::Call_s>(current)) {
            graph << "    \"" << label << "\" [label=\"" << label << "\"];\n";
            auto target = callNode->getTarget();
            queue.push(target);     
            std::stringstream labelStream;
            labelStream << "0x" << std::hex << reinterpret_cast<uintptr_t>(target.get()) << "\n " + target->getLabel();
            graph << "    \"" << label << "\" -> \"" << labelStream.str() << "\";\n";            
        }

    }

    graph << "}\n";
    std::string dotFile = filename + ".dot";
    std::string pdfFile = filename + ".pdf";
    std::ofstream out(dotFile);
    out << graph.str();
    out.close();
    std::string command = "dot -Tpdf " + dotFile + " -o " + pdfFile;
    system(command.c_str());
    std::remove(dotFile.c_str());
}