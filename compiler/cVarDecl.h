#pragma once

#include <string>

#include "cDecl.h"
#include "cArrayType.h"
#include "cExpr.h"
#include "cSymbol.h"
#include "codegen.h"

class cVarDecl : public cDecl
{
  public:
    cVarDecl(cTypeDecl *type, cSymbol *name) : cDecl()
    {
        // FIX THIS semantic check for "already defined"
        name->SetDecl(this);
        symbolTableRoot->Insert(name);

        AddChild(type);
        AddChild(name);
        AddChild(nullptr);  // array size
        AddChild(nullptr);  // init Expr
        mOffset     = 0;
        mIsGlobal   = false;
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
    }
    void SetGlobal()        { mIsGlobal = true; }

    virtual bool IsGlobal() { return mIsGlobal; }
    virtual bool IsConst()  
        { return GetInit()!=nullptr && GetInit()->IsConst(); }

    virtual bool IsVar()    { return true; }

    virtual void SetOffset(int offset)  { mOffset = offset; }
    virtual int  GetOffset()            { return mOffset; }

    virtual int ComputeOffsets(int base)
    {
        int size = GetType()->Size();

        if (size == 1) 
            mOffset = base;
        else if (base % WORD_SIZE != 0)
            mOffset = base/WORD_SIZE*WORD_SIZE + WORD_SIZE;
        else
            mOffset = base;

        if (base < 0)
        {
            mOffset -= size;
            base = mOffset;
        }
        else
            base = mOffset + size;

        return base;
    }
    virtual void GenerateCode()
    {}

    virtual cTypeDecl* GetType()    { return (cTypeDecl*)GetChild(0); }
    virtual cSymbol* GetName()      { return (cSymbol*)GetChild(1); }
    virtual cExpr*   GetArraySize() { return (cExpr*)GetChild(2); }
    virtual cExpr*   GetInit()      { return (cExpr*)GetChild(3); }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
  protected:
    int       mOffset;
    bool      mIsGlobal;
};

