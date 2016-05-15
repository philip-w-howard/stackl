#pragma once

#include <string>

#include "cVarRef.h"
#include "cSymbol.h"

class cArrayRef : public cVarRef
{
  public:
    cArrayRef(cExpr *base, cExpr *index) : cVarRef() 
    {
        cVarRef *var = dynamic_cast<cVarRef*>(base);
        if (var == NULL) 
        {
            ThrowSemanticError("Generating address for cArrayRef "
                        "without an underlying cVarRef");
            return;
        }

        cTypeDecl *baseType = base->GetType();

        //if (!baseType->IsPointer() && !base->IsArray())
        if (!baseType->IsPointer() && !baseType->IsArray())
        {
            ThrowSemanticError("Attempted to index a non-array");
            return;
        }

        if (baseType->IsArray()) baseType = baseType->ParentType();

        AddChild(base);
        AddChild(index);
        AddChild(baseType);

        //baseType = baseType->ParentType();
    }

    cVarRef* GetBase()            { return (cVarRef*)GetChild(0); }
    //virtual cTypeDecl *GetType()  { return GetBase()->GetType(); }
    virtual cTypeDecl *GetType()  { return (cTypeDecl*)GetChild(2); }
    cExpr* GetIndex()             { return (cExpr*)GetChild(1); }
    virtual bool IsArrayRef()     { return true; }
    virtual bool IsStruct()       { return GetType()->ParentType()->IsStruct();}

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};


