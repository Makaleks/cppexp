#include "cppexp.hpp"
#include <iostream>
using namespace cppexp;
using namespace std;

#define CHECK(a, msg) if (false == (a)) { cout << "ERROR on \"" << (msg) << "\" (Line " << __LINE__ << ": " << #a << ")\n"; GLOBAL_TEST_STATUS = -1;}

int main() {
    Exp e;
    e.loadExp("1+2^3-4");
    std::cout << "\nAnswer: " << e.calculate() << "\n\n";
    e.loadExp("1+2^3*sin(4+5)-6");
    std::cout << "\nAnswer: " << e.calculate() << "\n\n";
    e.loadExp("|1-3|*||1+1|-4|");
    std::cout << "\nAnswer: " << e.calculate() << "\n\n";

    return 0;
}
