#include "lex.h"
#include "cVisitor.h"
#include "astnodes.h"

void cVisitor::PreVisitAllNodes(cAstNode *node)
{
    VisitAllChildren(node);

    node->Visit(this);
}

void cVisitor::PostVisitAllNodes(cAstNode *node)
{
    node->Visit(this);

    VisitAllChildren(node);
}

void cVisitor::VisitAllChildren(cAstNode *node)
{
    cAstNode::iterator it;
    for (it=node->FirstChild(); it!=node->LastChild(); it++)
    {
        if ((*it) != nullptr) (*it)->Visit(this);
    }
}

void cVisitor::Visit(cAstNode *node)          { VisitAllChildren(node); }
void cVisitor::Visit(cAssignNode *node)       { VisitAllChildren(node); }
void cVisitor::Visit(cBinaryExprNode *node)   { VisitAllChildren(node); }
void cVisitor::Visit(cBlockNode *node)        { VisitAllChildren(node); }
void cVisitor::Visit(cDeclNode *node)         { VisitAllChildren(node); }
void cVisitor::Visit(cDeclsNode *node)        { VisitAllChildren(node); }
void cVisitor::Visit(cExprNode *node)         { VisitAllChildren(node); }
void cVisitor::Visit(cFloatExprNode *node)    { VisitAllChildren(node); }
void cVisitor::Visit(cFuncDeclNode *node)     { VisitAllChildren(node); }
void cVisitor::Visit(cFuncExprNode *node)     { VisitAllChildren(node); }
void cVisitor::Visit(cIfNode *node)           { VisitAllChildren(node); }
void cVisitor::Visit(cIntExprNode *node)      { VisitAllChildren(node); }
void cVisitor::Visit(cOpNode *node)           { VisitAllChildren(node); }
void cVisitor::Visit(cParamListNode *node)    { VisitAllChildren(node); }
void cVisitor::Visit(cParamsNode *node)       { VisitAllChildren(node); }
void cVisitor::Visit(cPrintNode *node)        { VisitAllChildren(node); }
void cVisitor::Visit(cProgramNode *node)      { VisitAllChildren(node); }
void cVisitor::Visit(cReturnNode *node)       { VisitAllChildren(node); }
void cVisitor::Visit(cStmtNode *node)         { VisitAllChildren(node); }
void cVisitor::Visit(cStmtsNode *node)        { VisitAllChildren(node); }
void cVisitor::Visit(cStructDeclNode *node)   { VisitAllChildren(node); }
void cVisitor::Visit(cSymbol *node)           { VisitAllChildren(node); }
void cVisitor::Visit(cVarDeclNode *node)      { VisitAllChildren(node); }
void cVisitor::Visit(cVarExprNode *node)      { VisitAllChildren(node); }
void cVisitor::Visit(cWhileNode *node)        { VisitAllChildren(node); }

