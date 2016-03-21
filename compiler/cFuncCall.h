#pragma once

#include <string>

#include "cTypeDecl.h"
#include "cFuncDecl.h"
#include "cVarRef.h"
#include "cParams.h"
#include "cSymbol.h"

class cFuncCall : public cVarRef
{
  public:
    cFuncCall(cExpr *base, cParams *params) : cVarRef() 
    {
        cTypeDecl *baseType = base->GetType();

        if (!baseType->IsFunc())
        {
            ThrowSemanticError("Attempted to call a non-function");
            return;
        }
        
        AddChild(base);
        AddChild(params);
    }

    virtual void GenerateAddress()
    {
        fatal_error("Attempted to get the address of a funciton");
    }

    virtual void GenerateCode()
    {
        if (GetParams() != NULL) GetParams()->GenerateCode();

        EmitInt(CALL_OP);
        SetLabelRef(GetFuncName());

        // Need to pop the args off the stack without affecting the return val
        if (GetParams() != NULL)
        {
            for (int ii=0; ii<GetParams()->Size()/WORD_SIZE; ii++)
            {
                EmitInt(SWAP_OP);
                EmitInt(POP_OP);
            }
        }
    }

    cExpr* GetBase()    { return (cExpr*)GetChild(0); }
    cParams* GetParams(){ return (cParams*)GetChild(1); }
    virtual bool IsFunc()        { return true; }
    cFuncDecl* GetFuncDecl()     { return (cFuncDecl*)(GetBase()->GetType()); }
    virtual cTypeDecl *GetType() { return GetFuncDecl()->ReturnType(); }
    std::string GetFuncName()    { return GetFuncDecl()->GetName()->Name(); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
};

