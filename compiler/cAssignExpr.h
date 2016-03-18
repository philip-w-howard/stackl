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
        AddChild(var);
        AddChild(expr);
    }

    virtual cTypeDecl *GetType()    { return GetVar()->GetType(); }

    virtual void GenerateCode()
    {
        GetExpr()->GenerateCode();

        // Need to dup the result in case the assign is treated as an expr
        EmitInt(DUP_OP);
        GetVar()->GenerateAddress();
        if (GetVar()->IsArrayRef() && GetVar()->GetType()->ElementSize() == 1)
            EmitInt(POPCVARIND_OP);
        else if (GetVar()->GetType()->Size() == 1)
            EmitInt(POPCVARIND_OP);
        else
            EmitInt(POPVARIND_OP);
    }

    cVarRef* GetVar()   { return (cVarRef*)GetChild(0); }
    cExpr*   GetExpr()  { return (cExpr*)GetChild(1); }

    virtual string NodeType()             { return "assign"; }
    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
