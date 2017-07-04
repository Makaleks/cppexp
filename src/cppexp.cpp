#include "cppexp.hpp"
#include <locale>

using namespace cppexp;

bool Exp::isReady () {
    if (rpn_.empty()) return true;
    auto end = varlst_.end();
    for (auto &v : rpn_) {
        if (v.isVar() && varlst_.find(v.name_) == end) {
            return false;
        }
    }
    return true;
}

bool Exp::isEmpty () {
    return rpn_.empty();
}

void Exp::clear () {
    rpn_.clear();
}

void Exp::loadVar (std::string name, VType val) {
    varlst_[name] = val;
}

void Exp::loadVars (std::vector <std::pair <std::string, VType>> vec) {
    for (auto &v : vec) {
        loadVar (v.first, v.second);
    }
}

enum NType {
    CONST, VAR, FUNC
};

#include <iostream>

bool Exp::loadExp (std::string expr) {
    std::cout << "Input:\n" << expr << '\n';
    auto getNext = [&](std::string::iterator it, Node *n) -> std::string::iterator {
        auto s_it = it;
        if (std::isdigit(*it)) {
            do {
                it++;
            } while (std::isdigit(*it) || *it == '.');
            long double num = std::stold(std::string(s_it, it));
            n->clear();
            *n = Node(num);
            //printf("= %10Lg: const\n", num);
            return it;
        }
        else {
            auto tmp_it = func_store_.end();
            while (true) {
                it++;
                if (it == expr.end()) {
                    n->clear();
                    *n = Node(std::string(s_it, it));
                    //printf("= %10s: var\n", std::string(s_it, it).c_str());
                    return it;
                }
                //if (*s_it == 's') puts(std::string(s_it, it).c_str());
                tmp_it = func_store_.find(std::string(s_it, it));
                if (tmp_it != func_store_.end()) {
                    n->clear();
                   *n = Node(tmp_it->second);
                    //printf("= %10s: func\n", std::string(s_it, it).c_str());
                    return it; 
                }
                else if ((it + 1 != expr.end() && std::isdigit(*it)) && func_store_.find(std::string(1, *(it + 1))) != func_store_.end()) {
                    n->clear();
                    *n = Node(std::string(s_it, it));
                    //printf("= %10s: var\n", std::string(s_it, it).c_str());
                    return it;
                }
            }
        }
    };
    auto it = expr.begin();
    Node tmp(0);
    std::stack <Node> op_stck;
    std::vector <Node> rpn;
    std::map <std::string, VType> varlst;
    auto pf = [&]() {
        for (size_t i = 0; i < rpn.size(); i++) {
            auto &n = rpn[i];
            if (n.isConst()) std::cout << n.val_ << "(c)";
            else if (n.isVar()) std::cout << n.name_ << "(v)";
            else std::cout << n.func_.name << "(f)";
            std::cout << ' ';
        }
        std::cout << '\n';
    };
    int i = 0;
    auto ps = [&]() {
        printf("%2d    stck: ", i); 
        for (auto nit = rpn.begin(); nit != rpn.end(); nit++) 
            std::cout << nit->getString() <<", "; 
        printf("\n   op_stck: "); 
        auto cp = op_stck;
        std::vector<Node> v;
        while (false == cp.empty()) {
            v.push_back(cp.top());
            cp.pop();
        }
        for (size_t j = v.size(); j > 0; j--) std::cout << v[j-1].getString() <<", "; 
        puts("\n"); i++;
    };
    while(it != expr.end()) {
        it = getNext(it, &tmp);
        if (tmp.isConst()) {
            rpn.push_back(tmp);
        }
        else if (tmp.isVar()) {
            rpn.push_back(tmp);
            varlst[tmp.name_] = 0;
        }
        else if (tmp.isFunc()) {
            //printf("ok: %d\n", tmp.func_.type == FuncNode::OPERATOR_B);
            auto &f = tmp.func_;
            //std::cout << f.name << ' ' << (f.type == FuncNode::FUNCTION) << '\n';
            if (op_stck.empty()) {
                op_stck.push(tmp);
            }
            else {
                if (f.isOpU() || f.isFunc() || f.isBrO()) {
                    //rpn.push_back(top);
                    //op_stck.pop();
                    //if (f.isFunc()) std::cout << "Pushing " << f.name << '\n';
                    op_stck.push(tmp);
                }
                else if (f.isOpB()) {
                    while (false == op_stck.empty() && (
                            op_stck.top().func_.isFunc()
                            || (op_stck.top().func_.isOpB()
                            && op_stck.top().func_.priority >= f.priority))) {
                        rpn.push_back(op_stck.top());
                        op_stck.pop();
                    }
                    op_stck.push(tmp);
                }
                else if (f.isBrC()) {
                    while (op_stck.top().func_.openBr != f.openBr) {
                        rpn.push_back(op_stck.top());
                        op_stck.pop();
                    }
                    op_stck.pop();
                    if (op_stck.empty() == false 
                            && op_stck.top().func_.isFunc()) {
                        rpn.push_back(op_stck.top());
                        op_stck.pop();
                    }
                }
            }
        }
        else return false;
        ps();
    }
    //std::cout << op_stck.size() << '\n';
    while (op_stck.empty() == false) {
        rpn.push_back(op_stck.top());
        op_stck.pop();
    }
    pf();
    rpn_ = rpn;
    varlst_ = varlst;
    return true;
}

#include <cmath>
VType nplus (std::stack<VType> &stck) {
    VType var = stck.top();
    stck.pop();
    var+= stck.top();
    stck.pop();
    return var;
}
VType nminus (std::stack<VType> &stck) {
    VType var = -stck.top();
    stck.pop();
    var+= stck.top();
    stck.pop();
    return var;
}
VType nmul (std::stack<VType> &stck) {
    VType var = stck.top();
    stck.pop();
    var*= stck.top();
    stck.pop();
    return var;
}
VType ndiv (std::stack<VType> &stck) {
    VType var2 = stck.top();
    stck.pop();
    VType var1 = stck.top();
    stck.pop();
    return var1 / var2;
}
VType npow (std::stack<VType> &stck) {
    VType var2 = stck.top();
    stck.pop();
    VType var1 = stck.top();
    stck.pop();
    return powl(var1, var2);
}
VType nsin (std::stack<VType> &stck) {
    VType var = stck.top();
    stck.pop();
    return sin(var);
}
VType br (std::stack<VType> &stck) {
    VType var = stck.top();
    stck.pop();
    return var;
}

Exp::Exp () {
    func_store_["+"] = FuncNode (FuncNode::OPERATOR_B, nplus,  "+", 100);
    func_store_["-"] = FuncNode (FuncNode::OPERATOR_B, nminus, "-", 100);
    func_store_["*"] = FuncNode (FuncNode::OPERATOR_B, nmul,   "*", 200);
    func_store_["/"] = FuncNode (FuncNode::OPERATOR_B, ndiv,   "/", 200);
    func_store_["^"] = FuncNode (FuncNode::OPERATOR_B, npow,   "^", 250);
    func_store_["("] = FuncNode (FuncNode::BRACKET_OPEN, br,   "(", 255, "(");
    func_store_[")"] = FuncNode (FuncNode::BRACKET_CLOSE, br,   ")", 255, "(");
    func_store_["sin"] = FuncNode (FuncNode::FUNCTION, nsin,   "sin", 255);
}
