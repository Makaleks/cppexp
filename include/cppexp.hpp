#ifndef CPPEXP_HPP
#define CPPEXP_HPP 0

#include <vector>
#include <map>
#include <string>
#include <functional>
#include <stack>

    #include <sstream> 

namespace cppexp {

// TODO: replace to template T
typedef long double VType;

    typedef std::stack<VType> Stck;

    enum FuncMeta {
        FUNCTION,
        OPERATOR,
        BRACKET
    };

    class Exp {
    protected:
        struct FuncNode {
            enum FType {
                NONE,
                FUNCTION,
                OPERATOR_U,
                OPERATOR_B,
                BRACKET_OPEN,
                BRACKET_CLOSE
            } type;
            FuncNode ()
                : type(NONE) {}
            FuncNode (FType t, std::function<VType(Stck&)> f, std::string nname, unsigned char prior, std::string b = "")
                : type(t), openBr(b), func(f), name(nname), priority(prior) {}
            std::string openBr;
            std::function<VType(Stck&)> func;
            std::string name;
            unsigned char priority = 0;
            bool isFunc() { return type == FUNCTION; }
            bool isOpU() { return type == OPERATOR_U; }
            bool isOpB() { return type == OPERATOR_B; }
            bool isBrO() { return type == BRACKET_OPEN; }
            bool isBrC() { return type == BRACKET_CLOSE; }
        };
        std::map <std::string, FuncNode> func_store_;

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
            //Node (FuncNode::FType t, std::function <VType(Stck)> func)
            //    : func_(t, func) {}
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
            std::string getString() {
                if (isConst()) {
                    std::stringstream s;
                    s << val_;
                    return s.str();
                }
                else if (isVar()) return name_;
                else return func_.name;
            }
        };
        void optimize ();
        std::map <std::string, VType> varlst_;
        std::vector <Node> rpn_;
        // TODO: fix to func. holder
    public:
        Exp ();
        Exp (std::string expr);
        bool loadExp (std::string expr);
        void loadVar (std::string name, VType val);
        void loadVars (std::vector <std::pair <std::string, VType>> vec);
        // Check if all vars loaded
        bool isReady ();
        bool isEmpty ();
        void clear ();
    };

}

#endif
