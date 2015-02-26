//*******************************************************
// Purpose: Class for binary expressions
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cBinaryExprNode.h"
#include "lex.h"

std::string cBinaryExprNode::StrOp()
{
    switch (mOperator)
    {
        case '+': return ("+");
        case '-': return ("-");
        case '*': return ("*");
        case '/': return ("/");
        case '%': return ("%");
        case AND: return ("&&");
        case OR: return ("||");
        case EQ: return ("==");
        case NE: return ("!=");
        case GE: return (">=");
        case LE: return ("<=");
        case LT: return ("<"); 
        case GT: return (">"); 
    }
    return (" UnrecognizedOperator "); 
}


