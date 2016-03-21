#pragma once

#include <string>

#include "cTypeDecl.h"
#include "cStmtsList.h"
#include "cReturnStmt.h"
#include "cIntExpr.h"
#include "cSymbol.h"
#include "cDeclsList.h"
#include "codegen.h"

class cFuncDecl : public cTypeDecl
{
  public:
    cFuncDecl(cTypeDecl *type, cSymbol *name) : cTypeDecl()
    {
        // FIX THIS: Semantic checks
        name->SetDecl(this);
        symbolTableRoot->Insert(name);

        AddChild(name);
        AddChild(type);
        AddChild(nullptr); // params
        AddChild(nullptr); // stmts
        mHasStatements  = false;
        mDeclsSize      = 0;
        mSize = WORD_SIZE;
    }

    void AddParams(cAstNode *params)
    {
        SetChild(2, params);
    }

    void AddStatements(cStmtsList *stmts)
    {
        SetChild(3, stmts);
        mHasStatements = true;
    }

    virtual bool IsFunc()           { return true; }
    virtual bool IsType()           { return false; }
    virtual void SetSize(int size)  { mDeclsSize = size; }
    virtual int  GetSize()          { return mDeclsSize; }

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

        GetStmts()->GenerateCode();

        // Force return statement
        cReturnStmt *ret = new cReturnStmt(new cIntExpr(0));
        ret->GenerateCode();
    }

    cSymbol* GetName()          { return (cSymbol*)GetChild(0); }
    cTypeDecl* ReturnType()     { return (cTypeDecl*)GetChild(1); }
    cDeclsList* GetParams()     { return (cDeclsList*)GetChild(2); }
    cStmtsList* GetStmts()      { return (cStmtsList*)GetChild(3); }
    bool IsDefinition()         { return mHasStatements; }
    int DeclsSize()             { return mDeclsSize; }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
    bool mHasStatements;
    int mDeclsSize;
};

