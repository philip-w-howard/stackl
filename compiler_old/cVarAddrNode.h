#pragma once

#include <string>
#include <list>
using std::list;

#include "cAstNode.h"
#include "cExprNode.h"
#include "cAstList.h"
#include "cStructDeclNode.h"
#include "cArrayValNode.h"
#include "cVarDeclNode.h"
#include "cSymbolTable.h"
#include "cVarPartNode.h"

#include "parse.h"
#include "codegen.h"

class cVarAddrNode : public cExprNode
{
  public:
    cVarAddrNode(cVarRefNode *var) 
    {
        mOffset = 0;
        mVar = var;
    }

    cDeclNode *GetType()
    {
        return mVar->GetType();
    }

    virtual int ComputeOffsets(int base)
    {
        mVar->ComputeOffsets(base);
        mOffset = mVar->GetOffset();

        return base;
    }

    virtual std::string toString()
    {
        std::string result = "&";
        result += mVar->toString();

        return result;
    }

    virtual void GenerateCode()
    {
        if (mVar->IsGlobal())
        {
            EmitInt(PUSH_OP);
            EmitGlobalRef(mVar->Name());
            EmitInt(PUSH_OP);
            EmitInt(mOffset);
            EmitInt(PLUS_OP);
        } else {
            EmitInt(PUSH_OP);
            EmitInt(mOffset);
            EmitInt(PUSHFP_OP);
            EmitInt(PLUS_OP);
        }
    }

  protected:
    cVarRefNode *mVar;
    int         mOffset;
};

