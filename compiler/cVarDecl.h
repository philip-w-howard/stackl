#pragma once

#include <string>

#include "cDecl.h"
#include "cArrayType.h"
#include "cExpr.h"
#include "cSymbol.h"

class cVarDecl : public cDecl
{
  public:
    cVarDecl(cTypeDecl *type, cSymbol *name) : cDecl()
    {
        if (symbolTableRoot->LocalLookup(name->Name()) != nullptr)
        {
            ThrowSemanticError(name->Name() +" already defined in local scope");
            return;
        }

        name->SetDecl(this);
        symbolTableRoot->Insert(name);

        AddChild(type);
        AddChild(name);
        AddChild(nullptr);  // array size
        AddChild(nullptr);  // init Expr
        mOffset     = 0;
        mIsGlobal   = false;
        mHasInit    = false;
    }

    cVarDecl(cVarDecl *base, cExpr *arraySize) : cDecl()
    {
        if (!arraySize->IsConst())
        {
            ThrowSemanticError("Array dimention must be a const");
            return;
        }

        int size = arraySize->ConstValue();
        AddChild(cArrayType::ArrayType(base->GetType(), size));
        AddChild(base->GetName());
        AddChild(arraySize);
        AddChild(nullptr);      // init expr
        
        base->GetName()->SetDecl(this);

        mOffset     = 0;
        mIsGlobal   = false;
    }

    void SetInit(cExpr *init) 
    { 
        if (GetType()->IsArray() || GetType()->IsStruct())
        {
            ThrowSemanticError("Cannot initialize arrays or structs");
            return;
        }
        SetChild(3, init); 
        mHasInit = true;
    }
    void SetGlobal()        { mIsGlobal = true; }

    virtual bool IsGlobal() { return mIsGlobal; }
    virtual bool IsConst()  
        { return GetInit()!=nullptr && GetInit()->IsConst(); }

    virtual bool IsVar()    { return true; }

    virtual void SetOffset(int offset)  { mOffset = offset; }
    virtual int  GetOffset()            { return mOffset; }

    virtual cTypeDecl* GetType()    { return (cTypeDecl*)GetChild(0); }
    virtual cSymbol* GetName()      { return (cSymbol*)GetChild(1); }
    virtual cExpr*   GetArraySize() { return (cExpr*)GetChild(2); }
    virtual cExpr*   GetInit()      { return (cExpr*)GetChild(3); }
    bool             HasInit()      { return mHasInit; }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
  protected:
    int       mOffset;
    bool      mIsGlobal;
    bool      mHasInit;
};

