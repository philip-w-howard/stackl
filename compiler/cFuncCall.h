#pragma once

#include <string>

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

        mFunc = dynamic_cast<cFuncDecl*>(baseType);
        
        AddChild(base);
        AddChild(params);
    }

    virtual bool IsFunc()        { return true; }
    virtual cTypeDecl *GetType() { return mFunc->ReturnType(); }

    virtual std::string toString()
    {
        std::string result;
        result =  GetBase()->toString() + "( ";
        if (GetParams() != NULL) result += GetParams()->toString();
        result += " )";

        return result;
    }

    virtual void GenerateAddress()
    {
        fatal_error("Attempted to get the address of a funciton");
    }

    virtual void GenerateCode()
    {
        if (GetParams() != NULL) GetParams()->GenerateCode();

        EmitInt(CALL_OP);
        SetLabelRef(mFunc->GetName()->Name());

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

  protected:
    cFuncDecl   *mFunc;
};

