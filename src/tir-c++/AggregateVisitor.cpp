#include "IRAst.hpp"
#include "AggregateVisitor.hpp"

using namespace TIR;

bool AggregateVisitor::visit(std::shared_ptr<Node> parent, std::shared_ptr<Node> n) {
    if (!n) return false;

    bool overrideValue = override(parent, n);
    if (overrideValue != false) return overrideValue;

    std::shared_ptr<AggregateVisitor> v_ = enter(parent, n);
    if (!v_)
        throw std::runtime_error("AggregateVisitor::enter() returned null!");

    bool r = n->aggregateChildren(v_);
    if (!r)
        throw std::runtime_error("Node::aggregateChildren() returned null!");

    r = leave(parent, n, r, v_);
    if (!r)
        throw std::runtime_error("AggregateVisitor::leave() returned null!");

    return r;
}

bool AggregateVisitor::visit(std::shared_ptr<Node> node) {
    return visit(nullptr, node);
}

bool AggregateVisitor::override(std::shared_ptr<Node> parent, std::shared_ptr<Node> n) {
    return false; 
}

std::shared_ptr<AggregateVisitor> AggregateVisitor::enter(std::shared_ptr<Node> parent, std::shared_ptr<Node> n) {
    return shared_from_this();
}

bool AggregateVisitor::leave(std::shared_ptr<Node> parent, std::shared_ptr<Node> n, bool r, std::shared_ptr<AggregateVisitor> v_) {
    return r;
}

//     if (auto node = std::dynamic_pointer_cast<BinOp>(n)) {
//         T result = unit();
//         result = bind(result, visit(node->left));
//         result = bind(result, visit(node->right));
//         return result;
//     }
//     else if (auto node = std::dynamic_pointer_cast<Call>(n)) {
//         T result = unit();
//         result = bind(result, visit(node->target));
//         for (std::shared_ptr<Expr> arg : node->args)
//             result = bind(result, visit(arg));
//         return result;
//     }
//     else if (auto node = std::dynamic_pointer_cast<CJump>(n)) {
//         T result = unit();
//         result = bind(result, visit(node->cond));
//         return result;
//     }
//     else if (auto node = std::dynamic_pointer_cast<CompUnit>(n)) {
//         T result = unit();
//         for (auto [key, func] : node->functions)
//             result = bind(result, visit(func));
//         return result;
//     }
//     else if (auto node = std::dynamic_pointer_cast<ESEQ>(n)) {
//         T result = unit();
//         result = bind(result, v->visit(node->stmt));
//         result = bind(result, v->visit(node->expr));
//         return result;
//     }
//     else if (auto node = std::dynamic_pointer_cast<EXP>(n)) {
//         T result = unit();
//         result = bind(result, visit(node->expr));
//         return result;
//     }
//     else if (auto node = std::dynamic_pointer_cast<FuncDecl>(n)) {
//         T result = unit();
//         result = bind(result, visit(node->body));
//         return result;
//     }
//     else if (auto node = std::dynamic_pointer_cast<Jump>(n)) {
//         T result = unit();
//         result = bind(result, visit(node->target));
//         return result;
//     }
//     else if (auto node = std::dynamic_pointer_cast<Mem>(n)) {
//         T result = unit();
//         result = bind(result, visit(node->expr));
//         return result;
//     }
//     else if (auto node = std::dynamic_pointer_cast<Move>(n)) {
//         T result = unit();
//         result = bind(result, visit(node->target));
//         result = bind(result, visit(node->src));
//         return result;
//     }
//     else if (auto node = std::dynamic_pointer_cast<Return>(n)) {
//         T result = unit();
//         result = bind(result, visit(node->ret));
//         return result;
//     }
//     else if (auto node = std::dynamic_pointer_cast<Seq>(n)) {
//         T result = unit();
//         for (auto stmt : node->stmts) {
//             result = bind(result, visit(stmt));
//         }
//         return result;
//     }
//     return unit();
// }
