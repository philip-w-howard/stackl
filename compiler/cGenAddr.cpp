#include <iostream>
#include <fstream>
#include <string>

#include "lex.h"
#include "cGenAddr.h"

using std::string;

cGenAddr::cGenAddr(cCodeGen* codeGen) : cVisitor() 
{
    m_CodeGen = codeGen;
}

void cGenAddr::VisitAllNodes(cAstNode *node) { node->Visit(this); }

//void cGenAddr::Visit(cAddressExpr *node) { }

void cGenAddr::Visit(cArrayRef *node)
{
    int size;
    cExpr *var = node->GetBase();

    if (node->GetBase()->IsPointer())
    {
        var->Visit(m_CodeGen);
        size = node->GetType()->ElementSize();
        //size = node->GetBase()->Size();
    }
    else
    {
        var->Visit(this);
        size = node->GetType()->Size();
    }

    node->GetIndex()->Visit(m_CodeGen);
    if (size == 1)
    {
        m_CodeGen->EmitInst("PLUS");
    } else {
        m_CodeGen->EmitInst("PUSH", size);
        m_CodeGen->EmitInst("TIMES");
        m_CodeGen->EmitInst("PLUS");
    }
}

//void cGenAddr::Visit(cArrayType *node)          { VisitAllChildren(node); }
//void cGenAddr::Visit(cAsmNode *node) { }

//void cGenAddr::Visit(cAssignExpr *node) { }

//void cGenAddr::Visit(cAstNode *node)            { VisitAllChildren(node); }
//void cGenAddr::Visit(cBaseDeclNode *node)       { VisitAllChildren(node); }
//void cGenAddr::Visit(cBinaryExpr *node)         { VisitAllChildren(node); }
//void cGenAddr::Visit(cBreakStmt *node)          { VisitAllChildren(node); }
//void cGenAddr::Visit(cContinueStmt *node)       { VisitAllChildren(node); }
//void cGenAddr::Visit(cDecl *node)               { VisitAllChildren(node); }
//void cGenAddr::Visit(cDeclsList *node)          { VisitAllChildren(node); }
//void cGenAddr::Visit(cExpr *node)               { VisitAllChildren(node); }
//void cGenAddr::Visit(cExprStmt *node)           { VisitAllChildren(node); }
//void cGenAddr::Visit(cForStmt *node)            { VisitAllChildren(node); }
void cGenAddr::Visit(cFuncCall *node)
{
    fatal_error("Attempted to get the address of a funciton");
}
//void cGenAddr::Visit(cFuncDecl *node)           { VisitAllChildren(node); }
//void cGenAddr::Visit(cGotoStmt *node)           { VisitAllChildren(node); }
//void cGenAddr::Visit(cIfStmt *node)             { VisitAllChildren(node); }
//void cGenAddr::Visit(cIntExpr *node)            { VisitAllChildren(node); }
//void cGenAddr::Visit(cLabeledStmt *node)        { VisitAllChildren(node); }
//void cGenAddr::Visit(cNopStmt *node)            { VisitAllChildren(node); }
//void cGenAddr::Visit(cParams *node)             { VisitAllChildren(node); }
void cGenAddr::Visit(cPlainVarRef *node)
{
    cVarDecl *var = node->GetDecl();
    // FIX THIS: need an actual check here
    if (var == NULL)
        fatal_error("Attempted to generate code for a cVarRef without a decl");
    if (var->IsGlobal())
    {
        m_CodeGen->EmitInst("PUSH", var->GetVarName());
    } else {
        m_CodeGen->EmitInst("PUSH", var->GetOffset());
        m_CodeGen->EmitInst("PUSHFP");
        m_CodeGen->EmitInst("PLUS");
    }
}

void cGenAddr::Visit(cPointerDeref *node)
{
    node->GetBase()->Visit(m_CodeGen);
}

//void cGenAddr::Visit(cPointerType *node)        { VisitAllChildren(node); }
//void cGenAddr::Visit(cPostfixExpr *node)        { VisitAllChildren(node); }
//void cGenAddr::Visit(cPrefixExpr *node)         { VisitAllChildren(node); }
//void cGenAddr::Visit(cReturnStmt *node)         { VisitAllChildren(node); }
//void cGenAddr::Visit(cShortCircuitExpr *node)   { VisitAllChildren(node); }
//void cGenAddr::Visit(cSizeofExpr *node)         { VisitAllChildren(node); }
//void cGenAddr::Visit(cStmt *node)               { VisitAllChildren(node); }
//void cGenAddr::Visit(cStmtsList *node)          { VisitAllChildren(node); }
//void cGenAddr::Visit(cStringLit *node)          { VisitAllChildren(node); }

void cGenAddr::Visit(cStructDeref *node)
{
    node->GetBase()->Visit(m_CodeGen);

    cVarDecl *field = node->GetFieldDecl();
    m_CodeGen->EmitInst("PUSH", field->GetOffset());
    m_CodeGen->EmitInst("PLUS");
}

void cGenAddr::Visit(cStructRef *node)
{
    cVarRef *var = node->GetBase();

    var->Visit(this);

    cVarDecl *field = node->GetFieldDecl();
    m_CodeGen->EmitInst("PUSH", field->GetOffset());
    m_CodeGen->EmitInst("PLUS");
}
//void cGenAddr::Visit(cStructType *node)         { VisitAllChildren(node); }
//void cGenAddr::Visit(cSymbol *node)             { VisitAllChildren(node); }
//void cGenAddr::Visit(cTernaryExpr *node)        { VisitAllChildren(node); }
//void cGenAddr::Visit(cTypeDecl *node)           { VisitAllChildren(node); }
//void cGenAddr::Visit(cUnaryExpr *node)          { VisitAllChildren(node); }
//void cGenAddr::Visit(cVarDecl *node)            { VisitAllChildren(node); }
//void cGenAddr::Visit(cVarRef *node)             { VisitAllChildren(node); }
//void cGenAddr::Visit(cWhileStmt *node)          { VisitAllChildren(node); }
