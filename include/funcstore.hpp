#ifndef FUNCSTORE_HPP
#define FUNCSTORE_HPP

#include <map>

#include "funcnode.hpp"
#include "node.hpp"

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
};

#endif
