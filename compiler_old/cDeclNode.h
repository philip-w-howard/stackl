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

        virtual cDeclNode *GetArrayElementType() { return NULL; }
        virtual cDeclNode *GetType()=0;

        virtual bool IsType()       { return false; }
        virtual bool IsFunc()       { return false; }
        virtual bool IsChar()       { return false; }
        virtual bool IsInt()        { return false; }
        virtual bool IsString()     { return false; }
        virtual bool IsStruct()     { return false; }
        virtual bool IsArray()      { return false; }
        virtual bool IsPointer()    { return false; }
        virtual bool IsGlobal()     { return false; }
        virtual bool IsConst()      { return false; }
        virtual int  GetValue()     { return 0; }
        virtual int Size()          { return mSize; }
        virtual int GetOffset()     { return mOffset; }
        virtual int ComputeOffsets(int base) { return base; }
        virtual cDeclNode* GetPointsTo() { return NULL; }
        virtual int GetPtrLevel() { return 0; }
        virtual bool CompatibleWith(cDeclNode *other)
        {
            cDeclNode *leftType = GetType();
            cDeclNode *rightType = other->GetType();

            // acceptable assignments:
            // foo abc = foo xyz
            // foo * abc = foo[5] xyz
            // foo * abc = foo * xyz
            // foo * abc = 123456

            if (leftType == rightType)
                return true;
            if(IsArray())
                if(rightType->IsPointer() && rightType->GetPointsTo() == leftType)
                    return true;
            if(other->IsArray() && rightType->IsPointer())
                return true;
            if(leftType->IsPointer())
            {
                if(rightType->IsPointer())
                    return true;
                if(rightType->IsArray())
                    return true;
                if(rightType->IsInt())
                    return true;
                if(leftType->GetPointsTo() == rightType)
                    return true;
            }
            if (leftType->IsInt() && rightType->IsInt() && leftType->Size() >= rightType->Size())
                return true;
            if(rightType->IsPointer())
            {
                if(rightType->GetPointsTo() == leftType)
                    return true;
            }

            return false;
        }
    protected:
        cSymbol *mId;
        int mOffset;
        int mSize;
};

