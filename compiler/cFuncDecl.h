#pragma once

#include <string>

#include "cTypeDecl.h"
#include "cStmtsList.h"
#include "cReturnStmt.h"
#include "cIntExpr.h"
#include "cSymbol.h"
#include "codegen.h"

class cFuncDecl : public cTypeDecl
{
  public:
    cFuncDecl(cTypeDecl *type, cSymbol *name) : cTypeDecl(name, WORD_SIZE)
    {
        name->SetDecl(this);
        symbolTableRoot->Insert(name);

        mReturnType     = type;
        mHasStatements  = false;
        mStatements     = NULL;
        mParams         = NULL;
        mDeclsSize      = 0;
    }

    void AddParams(cAstNode *params)
    {
        mParams = params;
    }

    void AddStatements(cStmtsList *stmts)
    {
        mStatements = stmts;
        mHasStatements = true;
    }

    cTypeDecl *ReturnType() { return mReturnType; }
    virtual bool IsFunc() { return true; }
    virtual bool IsType() { return false; }

    virtual std::string toString()
    {
        std::string result;
        result = "function: " + GetName()->toString();
        if (mParams != NULL)
            result += "( " + mParams->toString() + " )";
        else
            result += "( )";

        result += "\n";

        if (mHasStatements)
        {
            if (mStatements != NULL)
                result += mStatements->toString();
            else
                result += "{}";
        }
        else
        {
            result += ";";
        }

        return result;
    }

    virtual int ComputeOffsets(int base)
    {
        int locals = 0;
        if(mParams != NULL)  mParams->ComputeOffsets(-STACK_FRAME_SIZE);
        if (mStatements != NULL) locals = mStatements->ComputeOffsets(locals);
        mDeclsSize = locals;

        return base;
    }

    virtual void GenerateCode()
    {
        if (!mHasStatements) return;

        EmitComment(GetName()->Name() + "\n");
        SetLabelValue(GetName()->Name());
        int adj_size = (mDeclsSize / WORD_SIZE * WORD_SIZE) + WORD_SIZE;
        if (mDeclsSize != 0)
        {
            EmitInt(ADJSP_OP);
            EmitInt(adj_size);
        }

        mStatements->GenerateCode();

        // Force return statement
        cReturnStmt *ret = new cReturnStmt(new cIntExpr(0));
        ret->GenerateCode();
    }

  protected:
    bool mHasStatements;
    cTypeDecl *mReturnType;
    cStmtsList *mStatements;
    cAstNode *mParams;
    int mDeclsSize;
};

