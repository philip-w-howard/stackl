#pragma once

#include <string>

#include "cAstNode.h"

class cSymbol;

class cDeclNode : public cAstNode
{
  public:
    cDeclNode() : cAstNode()
    {}

    std::string TypeId();
    std::string Name();

    virtual cDeclNode *GetBaseType() { return this; }

    virtual bool IsType()   { return false; }
    virtual bool IsFunc()   { return false; }
    virtual bool IsInt()    { return false; }
    virtual bool IsStruct() { return false; }
    virtual bool IsArray()  { return false; }
    virtual int Size() { return mSize; }
    virtual int GetOffset() { return mOffset; }

    virtual int ComputeOffsets(int base) { return base; }

    virtual bool CompatibleWith(cDeclNode *other)
    {
        cDeclNode *left = GetBaseType();
        cDeclNode *right = other->GetBaseType();

        if (left == right) return true;
        if (left->IsInt() && right->IsInt() && left->Size() >= right->Size()) 
            return true;

        return false;
    }
  protected:
    static const int WORD_SIZE = 4;
    cSymbol *mId;
    int mOffset;
    int mSize;
};

