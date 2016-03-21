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

    cExpr* GetBase()    { return (cExpr*)GetChild(0); }
    cParams* GetParams(){ return (cParams*)GetChild(1); }
    virtual bool IsFunc()        { return true; }
    cFuncDecl* GetFuncDecl()     { return (cFuncDecl*)(GetBase()->GetType()); }
    virtual cTypeDecl *GetType() { return GetFuncDecl()->ReturnType(); }
    std::string GetFuncName()    { return GetFuncDecl()->GetName()->Name(); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
};

