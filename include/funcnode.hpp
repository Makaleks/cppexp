#ifndef FUNCNODE_HPP
#define FUNCNODE_HPP 0

#include "types.hpp"

struct FuncNode {
    enum FType {
        NONE,
        FUNCTION,
        OPERATOR_U,
        OPERATOR_B,
        BRACKET_OPEN,
        // special for sin(), etc
        BRACKET_OPEN_FUNCTION,
        BRACKET_CLOSE
    } type = NONE;
    std::string toString() {
        switch(type) {
        case FUNCTION:
            return "FUNCTION";
        case OPERATOR_U:
            return "OPERATOR_U";
        case OPERATOR_B:
            return "OPERATOR_B";
        case BRACKET_OPEN:
            return "BRACKET_OPEN";
        case BRACKET_OPEN_FUNCTION:
            return "BRACKET_OPEN_FUNCTION";
        case BRACKET_CLOSE:
            return "BRACKET_CLOSE";
        default:
            return "NONE";
        }
    }
    FuncNode ()
        : type(NONE) {}
    FuncNode (FType t, std::function<VType(Stck&)> f, std::string nname, unsigned char prior, std::string b = "")
        : type(t), closeBr(b), func(f), name(nname), priority(prior) {}
    std::string closeBr;
    std::function<VType(Stck&)> func;
    std::string name;
    unsigned char priority = 0;
    bool isFunc() { return type == FUNCTION; }
    bool isOpU() { return type == OPERATOR_U; }
    bool isOpB() { return type == OPERATOR_B; }
    bool isBrO() { return type == BRACKET_OPEN; }
    bool isBrOF() { return type == BRACKET_OPEN_FUNCTION; }
    bool isBrC() { return type == BRACKET_CLOSE; }
    FuncNode toCloseBr() {
        return FuncNode(BRACKET_CLOSE, func, closeBr, priority, closeBr);
    }
};

#endif
