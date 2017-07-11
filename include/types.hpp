#ifndef TYPES_HPP
#define TYPES_HPP

#include <stack>
#include <functional>

typedef long double                 VType;
typedef std::stack<VType>           Stck;
typedef std::function<VType(Stck&)> FType;

#endif
