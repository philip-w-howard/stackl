#pragma once

#include <string>

#include "cExpr.h"
#include "cExpr.h"
#include "cVarRef.h"
#include "cSymbol.h"

class cAssignExpr : public cExpr
{
  public:
    cAssignExpr(cExpr* var, cExpr *expr)  : cExpr()
    {
        if (!var->IsLval()) 
        {
            ThrowSemanticError("lval must be a variable");
        } else {
            cVarRef* varref  = dynamic_cast<cVarRef*>(var);
            assert(varref != NULL);
        }

        // don't type check asm expressions
        if (!expr->IsAsm())
        {
            if (!cTypeDecl::IsCompatibleWith(var->GetType(), expr->GetType()))
            {
                ThrowSemanticError("Assignment of incompatible types");
            }
        }

        AddChild(var);
        AddChild(expr);
    }

    virtual cTypeDecl *GetType()    { return GetVar()->GetType(); }

    cVarRef* GetVar()   { return (cVarRef*)GetChild(0); }
    cExpr*   GetExpr()  { return (cExpr*)GetChild(1); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
