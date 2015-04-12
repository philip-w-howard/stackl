#pragma once

#include <string>

#include "cStmtNode.h"

class cSymbol;

class cDeclNode : public cStmtNode
{
  public:
    cDeclNode() : cStmtNode()
    {}

    std::string TypeId();
    std::string Name();

    virtual cDeclNode *GetBaseType() { return this; }

    virtual bool IsType()       { return false; }
    virtual bool IsFunc()       { return false; }
    virtual bool IsChar()       { return false; }
    virtual bool IsInt()        { return false; }
    virtual bool IsString()     { return false; }
    virtual bool IsStruct()     { return false; }
    virtual bool IsArray()      { return false; }
    virtual bool IsPointer()    { return false; }
    //virtual bool IsAbsolute()   { return false; }
    virtual bool IsConst()      { return false; }
    virtual int Size()          { return mSize; }
    virtual int GetOffset()     { return mOffset; }

    virtual int ComputeOffsets(int base) { return base; }

    virtual bool CompatibleWith(cDeclNode *other)
    {
        cDeclNode *left = GetBaseType();
        cDeclNode *right = other->GetBaseType();

        if (left == right) return true;
        if (left->IsPointer() && right->IsArray()) return true;
        if (left->IsInt() && right->IsInt() && left->Size() >= right->Size()) 
            return true;

        return false;
    }
  protected:
    cSymbol *mId;
    int mOffset;
    int mSize;
};

