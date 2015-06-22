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
        case '&': return ("&");
        case '^': return ("&");
        case AND: return ("&&");
        case OR: return ("||");
        case EQ: return ("==");
        case NE: return ("!=");
        case GE: return (">=");
        case LE: return ("<=");
        case '<': return ("<"); 
        case '>': return (">"); 
        case LEFT: return ("<<"); 
        case RIGHT: return (">>"); 
    }
    return (" UnrecognizedOperator "); 
}

int cBinaryExprNode::IntOp()
{
    switch (mOperator)
    {
        case '+': return PLUS_OP;
        case '-': return MINUS_OP;
        case '*': return TIMES_OP;
        case '/': return DIVIDE_OP;
        case '%': return MOD_OP;
        case '&': return BAND_OP;
        case '|': return BOR_OP;
        case '^': return BXOR_OP;
        case AND: return AND_OP;
        case OR:  return OR_OP;
        case EQ:  return EQ_OP;
        case NE:  return NE_OP;
        case GE:  return GE_OP;
        case LE:  return LE_OP;
        case '<':  return LT_OP;
        case '>':  return GT_OP;
        case LEFT: return SHIFTL_OP;
        case RIGHT: return SHIFTR_OP;
    }
    return ILLEGAL_OP;
}


