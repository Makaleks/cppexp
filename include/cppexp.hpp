#ifndef CPPEXP_HPP
#define CPPEXP_HPP 0

#include <vector>
#include <map>
#include <string>
#include <stack>
#include <utility>

#include <sstream> 

#include "types.hpp"
#include "node.hpp"
#include "funcstore.hpp"

namespace cppexp {

// TODO: replace VType to template T
    class Exp {
    protected:
        FuncStore func_store_;

        void optimize ();
        std::map <std::string, VType> varlst_;
        std::vector <Node> rpn_;
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
