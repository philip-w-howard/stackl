#pragma once

#include <string>

#include "cExpr.h"
#include "cSymbol.h"

class cBinaryExpr : public cExpr
{
  public:
    cBinaryExpr(cExpr *left, int op, cExpr *right) : cExpr() 
    {
        AddChild(left);
        AddChild(right);
        mOp = op;
        if (!TypesAreCompatible())
        {
            ThrowSemanticError("Incompatible types in binary operation");
        }
    }

    virtual bool IsConst() { return GetLeft()->IsConst() && GetRight()->IsConst(); }
    virtual int  ConstValue();

    virtual cTypeDecl *GetType();

    cExpr* GetLeft()    { return (cExpr*)GetChild(0); }
    cExpr* GetRight()   { return (cExpr*)GetChild(1); }

    std::string OpToString();
    //int OpAsInt();
    std::string OpAsString();

    bool IsArithmetic();
    bool IsLogical();
    bool IsComparative();
    bool TypesAreCompatible();

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
  protected:
    int   mOp;

};

