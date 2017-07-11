#ifndef NODE_HPP
#define NODE_HPP

#include "funcnode.hpp"

class Node {
public:
    VType val_;
    std::string name_;
    FuncNode func_;
    friend class Exp;
public:
    Node (std::string varname)
        : name_(varname) {}
    Node (VType constval)
        : val_(constval) {}
    Node (FuncNode f)
        : func_(f) {}
    Node (const Node &src)
        : val_(src.val_), name_(src.name_), func_(src.func_) {}
    bool isVar () {
        return !name_.empty();
    }
    bool isConst () {
        return name_.empty() && false == static_cast<bool>(func_.func);
    }
    bool isFunc () {
        return static_cast<bool>(func_.func);
    }
    void clear() {
        name_.clear();
        func_.func = std::function<VType(Stck&)>();
    }
    std::string toString() {
        if (isConst()) {
            std::stringstream s;
            s << val_;
            return s.str();
        }
        else if (isVar()) return name_;
        else return func_.name;
    }
};

#endif
