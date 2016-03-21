#pragma once

#include <string>

#include "cVarDecl.h"
#include "cStructType.h"
#include "cTypeDecl.h"
#include "cVarRef.h"
#include "cSymbol.h"

class cStructDeref : public cVarRef
{
  public:
    cStructDeref(cExpr *base, cSymbol *field) : cVarRef() 
    {
        AddChild(base);
        cTypeDecl *baseType = base->GetType();

        if (!baseType->IsPointer())
        {
            ThrowSemanticError("The left of -> must be a pointer");
            return;
        }

        cTypeDecl *baseBaseType = baseType->ParentType();
        if (!baseBaseType->IsStruct())
        {
            ThrowSemanticError("The left of -> must be a struct pointer");
            return;
        }

        cStructType *s_decl = dynamic_cast<cStructType *>(baseBaseType);
        if (!s_decl->Contains(field))
        {
            ThrowSemanticError(field->Name() + " is not a field");
            return;
        }

        field = s_decl->GetField(field);
        AddChild(field);
    }

    virtual cTypeDecl *GetType() 
    { 
        return dynamic_cast<cTypeDecl *>(GetField()->GetDecl()->GetType()); 
    }

    cExpr* GetBase()    { return (cExpr*)GetChild(0); }
    cSymbol* GetField() { return (cSymbol*)GetChild(1); }
    cVarDecl* GetFieldDecl()     { return (cVarDecl*)(GetField()->GetDecl()); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};

