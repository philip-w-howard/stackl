#pragma once
#include <string>
using std::string;

#include "../machine.h"

class operation
{
public:
    operation( char op ): _op( op ) {}
    int64_t calc( Machine_State* cpu, string left, string right );
    bool operator==( const char rhs ) const { return _op == rhs; }
    bool operator==( const operation& rhs ) const { return _op == rhs._op; }
private:
    char _op;
};