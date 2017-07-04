#include "cppexp.hpp"
#include <iostream>
using namespace cppexp;
using namespace std;

#define CHECK(a, msg) if (false == (a)) { cout << "ERROR on \"" << (msg) << "\" (Line " << __LINE__ << ": " << #a << ")\n"; GLOBAL_TEST_STATUS = -1;}

int main() {
    Exp e;
    e.loadExp("1+2^3-4");
    e.loadExp("1+2^3*sin(4+5)-6");

    return 0;
}
