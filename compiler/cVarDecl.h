#pragma once

#include <string>

#include "cDecl.h"
#include "cArrayType.h"
#include "cExpr.h"
#include "cSymbol.h"

class cVarDecl : public cDecl
{
  public:
    cVarDecl(cTypeDecl *type, cSymbol *name) : cDecl(name)
    {
        name->SetDecl(this);
        symbolTableRoot->Insert(name);

        mType       = type;
        mExpr       = NULL;
        mOffset     = 0;
        mIsGlobal   = false;
    }

    cVarDecl(cTypeDecl *type, cSymbol *name, int arraySize) : cDecl(name)
    {
        name->SetDecl(this);
        symbolTableRoot->Insert(name);

        mType       = cArrayType::ArrayType(type, arraySize);
        mExpr       = NULL;
        mOffset     = 0;
        mIsGlobal   = false;
    }

    void SetInit(cExpr *init) { mExpr = init; }
    void SetGlobal()          { mIsGlobal = true; }

    virtual bool IsGlobal() { return mIsGlobal; }
    virtual bool IsConst()  { return mExpr!=NULL && mExpr->IsConst(); }
    virtual cExpr *GetInitValue() { return mExpr; }
    virtual bool IsVar()    { return true; }

    virtual cTypeDecl *GetType() 
    { 
        return mType;
    }

    virtual int GetOffset() { return mOffset; }

    virtual std::string toString()
    {
        std::string result;
        result = "vardecl: " + mType->GetSymbol()->toString() + " : " 
            + mName->toString() + " @ " + std::to_string(mOffset);

        return result;
    }

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

  protected:
    cTypeDecl *mType;
    cExpr     *mExpr;
    int       mOffset;
    bool      mIsGlobal;
};

