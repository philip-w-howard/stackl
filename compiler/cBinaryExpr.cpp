#include <iostream>

#include "cBinaryExpr.h"
#include "lex.h"
#include "parser.h"

int cBinaryExpr::ConstValue()
{
    int left, right;

    if (!IsConst()) return 0;

    left = GetLeft()->ConstValue();
    right = GetRight()->ConstValue();

    switch (mOp)
    {
        case '+': return left + right;
        case '-': return left - right;
        case '*': return left * right;
        case '/': return left / right;
        case '%': return left % right;
        case '>': return left > right;
        case '<': return left < right;
        case '&': return left & right;
        case '|': return left | right;
        case AND: return left && right;
        case OR : return left || right;
        case '^': return left ^ right;
        case EQ : return left == right;
        case NE : return left != right;
        case LE : return left <= right;
        case GE : return left >= right;
        case LEFT : return left << right;
        case RIGHT : return left >> right;
        default: 
            fatal_error("Internal compiler: unrecognized binary operator");
            break;
    }

    return 0;
}
std::string cBinaryExpr::OpToString()
{
    switch (mOp)
    {
        case '+': return "+";
        case '-': return "-";
        case '*': return "*";
        case '/': return "/";
        case '%': return "%";
        case '>': return "gt";
        case '<': return "lt";
        case '&': return "bitand";
        case '|': return "|";
        case AND: return "and";
        case OR : return "||";
        case '^': return "^";
        case EQ : return "==";
        case NE : return "!=";
        case LE : return "le";
        case GE : return "ge";
        case LEFT : return "shift_l";
        case RIGHT : return "shift_r";
        default: 
            fatal_error("Internal compiler: unrecognized binary operator");
            break;
    }

    return 0;
}

std::string cBinaryExpr::OpAsString()
{
    switch (mOp)
    {
        case '+': 
            return ("PLUS");
            break;
        case '-': 
            return ("MINUS");
            break;
        case '*': 
            return ("TIMES");
            break;
        case '/': 
            return ("DIVIDE");
            break;
        case '%': 
            return ("MOD");
            break;
        case '>': 
            return ("GT");
            break;
        case '<': 
            return ("LT");
            break;
        case '&': 
            return ("BAND");
            break;
        case '|': 
            return ("BOR");
            break;
        case AND: 
            return ("AND");
            break;
        case OR : 
            return ("OR");
            break;
        case '^': 
            return ("BXOR");
            break;
        case EQ : 
            return ("EQ");
            break;
        case NE : 
            return ("NE");
            break;
        case LE : 
            return ("LE");
            break;
        case GE : 
            return ("GE");
            break;
        case LEFT : 
            return ("SHIFTL");
            break;
        case RIGHT : 
            return ("SHIFTR");
            break;
        default: 
            fatal_error("Internal compiler: unrecognized binary operator");
            break;
    }

    return 0;
}

bool cBinaryExpr::IsLogical()
{
    if (IsComparative()) return true;

    switch (mOp)
    {
        case AND: 
        case OR : 
            return true;
    }

    return false;
}

bool cBinaryExpr::IsComparative()
{
    switch (mOp)
    {
        case '>': 
        case '<': 
        case EQ : 
        case NE : 
        case LE : 
        case GE : 
            return true;
    }

    return false;
}

bool cBinaryExpr::IsArithmetic()
{
    switch (mOp)
    {
        case '+': 
        case '-':
        case '*':
        case '/':
        case '%':
            return true;
    }

    return false;
}

/*
int cBinaryExpr::OpAsInt()
{
    switch (mOp)
    {
        case '+': 
            return (PLUS_OP);
            break;
        case '-': 
            return (MINUS_OP);
            break;
        case '*': 
            return (TIMES_OP);
            break;
        case '/': 
            return (DIVIDE_OP);
            break;
        case '%': 
            return (MOD_OP);
            break;
        case '>': 
            return (GT_OP);
            break;
        case '<': 
            return (LT_OP);
            break;
        case '&': 
            return (BAND_OP);
            break;
        case '|': 
            return (BOR_OP);
            break;
        case AND: 
            return (AND_OP);
            break;
        case OR : 
            return (OR_OP);
            break;
        case '^': 
            return (BXOR_OP);
            break;
        case EQ : 
            return (EQ_OP);
            break;
        case NE : 
            return (NE_OP);
            break;
        case LE : 
            return (LE_OP);
            break;
        case GE : 
            return (GE_OP);
            break;
        case LEFT : 
            return (SHIFTL_OP);
            break;
        case RIGHT : 
            return (SHIFTR_OP);
            break;
        default: 
            fatal_error("Internal compiler: unrecognized binary operator");
            break;
    }

    return 0;
}
*/

extern int Do_Type_Checks;

bool cBinaryExpr::TypesAreCompatible()
{
    if (!Do_Type_Checks) return true;

    cTypeDecl *left = GetLeft()->GetType();;
    cTypeDecl *right = GetRight()->GetType();;
    cTypeDecl *character = dynamic_cast<cTypeDecl *>
            (symbolTableRoot->Lookup("char")->GetDecl());
    cTypeDecl *integer = dynamic_cast<cTypeDecl *>
            (symbolTableRoot->Lookup("int")->GetDecl());
    bool leftint = ( left == integer || left == character );
    bool rightint = ( right == integer || right == character );

    if (character==NULL) fatal_error("Couldn't get type of char");
    if (integer==NULL) fatal_error("Couldn't get type of int");

    // can't perform operations on void
    if (left->IsVoid() || right->IsVoid()) return false;

    // operations on same types are OK
    if (left == right) return true;

    // arithmetic between pointers and integers is OK
    if (IsArithmetic())
    {
        if (left->IsPointer() && rightint) return true;
        if (right->IsPointer() && leftint) return true;
    }

    // promote chars to ints
    if (leftint && rightint) return true;

    // allow pointer comparisons/arithmetic of any type pointers
    if (left->IsPointer() && right->IsPointer() && 
            (IsArithmetic() || IsComparative())) return true;

    return false;
}

cTypeDecl *cBinaryExpr::GetType()
{
    cTypeDecl *left = GetLeft()->GetType();;
    cTypeDecl *right = GetRight()->GetType();;
    cTypeDecl *character = dynamic_cast<cTypeDecl *>
            (symbolTableRoot->Lookup("char")->GetDecl());
    cTypeDecl *integer = dynamic_cast<cTypeDecl *>
            (symbolTableRoot->Lookup("int")->GetDecl());

    if (character==NULL) fatal_error("Couldn't get type of char");
    if (integer==NULL) fatal_error("Couldn't get type of int");

    // The result of every logical in int
    if (IsLogical()) return integer;

    if (left->IsPointer() && right->IsPointer() && IsArithmetic()) return integer;
    if (left == right) return left;

    if (left->IsPointer() && (right==integer || right==character)) return left;
    if (right->IsPointer() && (left==integer || left==character)) return right;

    if (left->Size() == 1 && right->Size() == 1) return left;

    if (left->Size() == 1 && right == integer) return right;
    if (right->Size() == 1 && left == integer) return left;

    return NULL;
}
