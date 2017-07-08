#ifndef CPPEXP_HPP
#define CPPEXP_HPP 0

#include <vector>
#include <map>
#include <string>
#include <functional>
#include <stack>
#include <utility>

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

        class FuncStore{
            std::map <std::string, FuncNode> store_;
            size_t store_min_key_len_ = 0;
        public:
            struct State {
                std::stack <std::pair<std::string, FuncNode>> close_br_;
                enum Type {
                    OPERATOR,
                    FUNCTION,
                    BRACKET,
                    ARGUMENT
                } type = OPERATOR, next_type = OPERATOR;
                void setArgument() {
                    next_type = ARGUMENT;
                }
                void setBracket() {
                    next_type = BRACKET;
                }
                void setOperator() {
                    next_type = OPERATOR;
                }
                void setFunction() {
                    next_type = FUNCTION;
                }
                std::string toString() {
                    switch (type) {
                    case OPERATOR:
                        return "OPERATION";
                    case FUNCTION:
                        return "FUNCTION";
                    case BRACKET:
                        return "BRACKET";
                    default:
                        return "ARGUMENT";
                    }
                }
                bool operator==(State::Type t) {
                    return type == t;
                }
                void next() {
                    type = next_type;
                }
            };
            void addFunction (std::function<VType(Stck&)> f, std::string name ) {
                store_[name] = FuncNode(FuncNode::FUNCTION, f, name, 255);
                if (name.length() < store_min_key_len_ || store_min_key_len_ == 0) {
                    store_min_key_len_ = name.length();
                }
            }
            void addOperator (std::function<VType(Stck&)> f, std::string name, unsigned char priority) {
                store_[name] = FuncNode(FuncNode::OPERATOR_B, f, name, priority);
                if (name.length() < store_min_key_len_ || store_min_key_len_ == 0) {
                    store_min_key_len_ = name.length();
                }
            }
            void addBrackets (std::function<VType(Stck&)> f, std::string open, std::string close) {
                store_[open] = FuncNode(FuncNode::BRACKET_OPEN, f, open, 255, close);
                if (open.length() < store_min_key_len_ || store_min_key_len_ == 0) {
                    store_min_key_len_ = open.length();
                }
                else if (close.length() < store_min_key_len_) {
                    store_min_key_len_ = close.length();
                }
            }
            bool find(std::string key, Node *n, State *s, size_t *varoffset) {
                auto it = store_.find(key);
                if (s->close_br_.empty() == false 
                        && (*s == State::ARGUMENT || *s == State::BRACKET)
                        && key == s->close_br_.top().first) {
                    *n = Node(s->close_br_.top().second);
                    s->setBracket();
                    return true;
                }
                else if (it != store_.end()) {
                    *n = Node(it->second);
                    if (n->func_.isBrO()) {
                        s->close_br_.push(std::pair<std::string, FuncNode>(it->second.closeBr, it->second.toCloseBr()));
                        s->setBracket();
                    }
                    else {
                        s->setOperator();
                    }
                    return true;
                }
                else if (key.length() > store_min_key_len_) {
                    std::string tmpk;
                    for (size_t i = key.length() - store_min_key_len_; i > 0; i--) {
                        tmpk = key.substr(i);
                        it = store_.find(tmpk);
                        if (it != store_.end()) {
                            *varoffset = i;
                            s->setArgument();
                            return true;
                        }
                    }
                    return false;
                }
                else {
                    return false;
                }
            }
        } func_store_;

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
        VType calculate();
        // Check if all vars loaded
        bool isReady ();
        bool isEmpty ();
        void clear ();
    };

}

#endif
