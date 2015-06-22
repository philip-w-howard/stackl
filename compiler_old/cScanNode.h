#pragma once
//*******************************************************
// Purpose: Class for a scan statement
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cStmtNode.h"
#include "cVarRefNode.h"

class cScanNode : public cStmtNode
{
  public:
    cScanNode(cVarRefNode *lval, int type) : cStmtNode()
    {
        mLval = lval;
        mInputType = type;
    }

    virtual std::string toString()
    {
        std::string result("SCAN: ");
        result += mLval->toString();
        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        mLval->ComputeOffsets(base);
        return base;
    }

    virtual void GenerateCode()
    {
        EmitInt(ILLEGAL_OP);
        /*
        if (mInputType == 'I')
        {
            EmitInt(PUSH_OP);
            mLval->EmitOffset();
            EmitInt(PUSHFP_OP);
            EmitInt(PLUS_OP);
            EmitInt(GETI_OP);
        }
        else
        {
            //if ( ! mLval->IsArrayRef()) EmitInt(PUSHVAR_OP);
            //if (mLval->GetType()->IsPointer()) EmitInt(PUSHVAR_OP);
            mLval->EmitOffset();

            if (mInputType == 'S')
                EmitInt(GETS_OP);
            else if (mInputType == 'L')
                EmitInt(GETL_OP);
            else
                EmitInt(ILLEGAL_OP);
        }
        */
    }

  protected:
    cVarRefNode *mLval;     // variable to receive the scanned value
    int mInputType;         // 'S', 'L', 'I' for String, Line, Int
};

