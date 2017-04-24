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

        cFuncDecl *func = base->GetType()->GetFunc();
        assert(func != nullptr);
        cDeclsList *args = func->GetParams();

        if ( (args == nullptr && params != nullptr) ||
             (args != nullptr && params == nullptr))
        {
            ThrowSemanticError("Wrong number of arguments");
            return;
        }

        if (args != nullptr && params != nullptr)
        {
            if (args->NumChildren() != params->NumChildren())
            {
                ThrowSemanticError("Wrong number of arguments");
                return;
            }

            // FIX THIS: need to check types is some meaningful way
            // Allow ptr <--> int and char <--> int, but disallow 
            // every other difference
            for (int ii=0; ii<args->NumChildren(); ii++)
            {
                if (!cTypeDecl::IsCompatibleWith(
                            args->GetDecl(ii)->GetType(), 
                            params->GetParam(ii)->GetType()))
                {
                    ThrowSemanticError("Argument " + std::to_string(ii+1) +
                            " is of incompatible type");
                    return;
                }
            }
        }
    }

    virtual cExpr* GetBase()     { return (cExpr*)GetChild(0); }
    cParams* GetParams(){ return (cParams*)GetChild(1); }
    virtual bool IsFunc()        { return true; }
    cFuncDecl* GetFuncDecl()     { return (cFuncDecl*)(GetBase()->GetType()); }
    virtual cTypeDecl *GetType() { return GetFuncDecl()->ReturnType(); }
    std::string GetFuncName()    { return GetFuncDecl()->GetName()->Name(); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
};

