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
        mBase = base;
        mParams = params;
    }

    virtual bool IsFunc()        { return true; }
    virtual cTypeDecl *GetType() { return mFunc->ReturnType(); }

    virtual std::string toString()
    {
        std::string result;
        result =  mBase->toString() + "( ";
        if (mParams != NULL) result += mParams->toString();
        result += " )";

        return result;
    }

    virtual void GenerateCode()
    {
        if (mParams != NULL) mParams->GenerateCode();

        EmitInt(CALL_OP);
        SetLabelRef(mFunc->GetSymbol()->Name());

        // Need to pop the args off the stack without affecting the return val
        if (mParams != NULL)
        {
            for (int ii=0; ii<mParams->Size()/WORD_SIZE; ii++)
            {
                EmitInt(SWAP_OP);
                EmitInt(POP_OP);
            }
        }
    }

  protected:
    cExpr       *mBase;
    cFuncDecl   *mFunc;
    cParams     *mParams;
};

