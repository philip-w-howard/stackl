#pragma once
//*******************************************************
// Purpose: Class to represent a function call
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cAstNode.h"
#include "cExprNode.h"
#include "cParamsSpecNode.h"
#include "cParamNode.h"
#include "cFuncDeclNode.h"
#include "codegen.h"

class cFuncCallNode : public cExprNode
{
  public:
    // id is the name of the function
    // params are the parameters in the call
    cFuncCallNode(cSymbol *id, cParamNode *params)  : cExprNode()
    {
        cSymbol *func = symbolTableRoot->Lookup(id->Name());

        if (func != NULL)
        {
            id = func;

            mId = id;
            mParams = params;
        }
    }

    // return the type of the returned value
    virtual cDeclNode *GetType()
    {
        return mId->GetType()->GetBaseType();
    }

    virtual std::string toString()
    {
        std::string result("(FUNC CALL: " + mId->toString());
        if (mParams != NULL) result += mParams->toString();
        result += ")";
        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        if (mParams != NULL) mParams->ComputeOffsets(base);
        return base;
    }

    virtual void GenerateCode()
    {
        assert(mParams == NULL);
        EmitInt(CALL_OP);
        SetJumpSource(mId->Name());

        /*
        EmitPushFP();
        // need to add params
        if (mParams != NULL) 
        {
            mParams->GenerateCode();
            EmitString("Frame_Pointer = Stack_Pointer - ");
            EmitInt(mParams->Size());
            EmitString(";\n");
        }
        else
        {
            EmitString("Frame_Pointer = Stack_Pointer;\n");
        }

        EmitString(mId->Name() + "_");
        EmitInt(mId->Id());
        EmitString("();\n");
        // need to pop params
        if (mParams != NULL) 
        {
            EmitString("Stack_Pointer -= ");
            EmitInt(mParams->Size());
            EmitString(";\n");
        }
        EmitPopFP();
        */
    }
  protected:
    cSymbol *mId;           // name of the function
    cParamNode *mParams;    // params passed to the function
};

