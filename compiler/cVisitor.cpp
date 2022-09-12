#include "lex.h"
#include "cVisitor.h"

void cVisitor::VisitAllChildren(cAstNode *node)
{
    for (int ii=0; ii<node->NumChildren(); ii++)
    {
        if (node->GetChild(ii) != nullptr) node->GetChild(ii)->Visit(this);
    }
}

void cVisitor::Visit(cAddressExpr *node)        { VisitAllChildren(node); }
void cVisitor::Visit(cArrayRef *node)           { VisitAllChildren(node); }
void cVisitor::Visit(cArrayType *node)          { VisitAllChildren(node); }
void cVisitor::Visit(cAsmNode *node)            { VisitAllChildren(node); }
void cVisitor::Visit(cAssignExpr *node)         { VisitAllChildren(node); }
void cVisitor::Visit(cAstNode *node)            { VisitAllChildren(node); }
void cVisitor::Visit(cBaseDeclNode *node)       { VisitAllChildren(node); }
void cVisitor::Visit(cBinaryExpr *node)         { VisitAllChildren(node); }
void cVisitor::Visit(cBreakStmt *node)          { VisitAllChildren(node); }
void cVisitor::Visit(cCastExpr *node)           { VisitAllChildren(node); }
void cVisitor::Visit(cContinueStmt *node)       { VisitAllChildren(node); }
void cVisitor::Visit(cDecl *node)               { VisitAllChildren(node); }
void cVisitor::Visit(cDeclsList *node)          { VisitAllChildren(node); }
void cVisitor::Visit(cDoWhileStmt *node)        { VisitAllChildren(node); }
void cVisitor::Visit(cExpr *node)               { VisitAllChildren(node); }
void cVisitor::Visit(cExprStmt *node)           { VisitAllChildren(node); }
void cVisitor::Visit(cForStmt *node)            { VisitAllChildren(node); }
void cVisitor::Visit(cFuncCall *node)           { VisitAllChildren(node); }
void cVisitor::Visit(cFuncDecl *node)           { VisitAllChildren(node); }
void cVisitor::Visit(cIfStmt *node)             { VisitAllChildren(node); }
void cVisitor::Visit(cIntExpr *node)            { VisitAllChildren(node); }
void cVisitor::Visit(cNopStmt *node)            { VisitAllChildren(node); }
void cVisitor::Visit(cParams *node)             { VisitAllChildren(node); }
void cVisitor::Visit(cPlainVarRef *node)        { VisitAllChildren(node); }
void cVisitor::Visit(cPointerDeref *node)       { VisitAllChildren(node); }
void cVisitor::Visit(cPointerType *node)        { VisitAllChildren(node); }
void cVisitor::Visit(cPostfixExpr *node)        { VisitAllChildren(node); }
void cVisitor::Visit(cPragma *node)             { VisitAllChildren(node); }
void cVisitor::Visit(cPrefixExpr *node)         { VisitAllChildren(node); }
void cVisitor::Visit(cReturnStmt *node)         { VisitAllChildren(node); }
void cVisitor::Visit(cShortCircuitExpr *node)   { VisitAllChildren(node); }
void cVisitor::Visit(cSizeofExpr *node)         { VisitAllChildren(node); }
void cVisitor::Visit(cStmt *node)               { VisitAllChildren(node); }
void cVisitor::Visit(cStmtsList *node)          { VisitAllChildren(node); }
void cVisitor::Visit(cStringLit *node)          { VisitAllChildren(node); }
void cVisitor::Visit(cStructDeref *node)        { VisitAllChildren(node); }
void cVisitor::Visit(cStructRef *node)          { VisitAllChildren(node); }
void cVisitor::Visit(cStructType *node)         { VisitAllChildren(node); }
void cVisitor::Visit(cSymbol *node)             { VisitAllChildren(node); }
void cVisitor::Visit(cTypeDecl *node)           { VisitAllChildren(node); }
void cVisitor::Visit(cTypedef *node)            { VisitAllChildren(node); }
void cVisitor::Visit(cUnaryExpr *node)          { VisitAllChildren(node); }
void cVisitor::Visit(cVarDecl *node)            { VisitAllChildren(node); }
void cVisitor::Visit(cVarRef *node)             { VisitAllChildren(node); }
void cVisitor::Visit(cWhileStmt *node)          { VisitAllChildren(node); }
