#include <iostream>
#include "cSemantics.h"
#include "lex.h"

using std::string;

static int s_JumpContextLevel;

//***********************************
cSemantics::cSemantics() : cVisitor()
{
}

cSemantics::~cSemantics()
{
}

void cSemantics::VisitAllNodes(cAstNode *node) 
{ 
    node->Visit(this); 
}

void cSemantics::Visit(cBreakStmt *node)
{
    if (s_JumpContextLevel == 0)
    {
        semantic_error("Break statement not within a loop or switch", node->LineNumber());
    }
    VisitAllChildren(node);
}

void cSemantics::Visit(cContinueStmt *node)
{
    if (s_JumpContextLevel == 0)
    {
        semantic_error("Continue statement not within a loop", node->LineNumber());
    }
    VisitAllChildren(node);
}

void cSemantics::Visit(cDoWhileStmt *node)
{
    ++s_JumpContextLevel;
    VisitAllChildren(node);
    --s_JumpContextLevel;
}

void cSemantics::Visit(cForStmt *node)
{
    ++s_JumpContextLevel;
    VisitAllChildren(node);
    --s_JumpContextLevel;
}

void cSemantics::Visit(cFuncDecl *node)
{
    m_funcReturnType = node->ReturnType();

    VisitAllChildren(node);

    // Force a return statement
    if (node->IsDefinition())
    {
        if (!node->ReturnType()->IsVoid())
        {
            node->GetStmts()->AddChild(new cReturnStmt(new cIntExpr(0)));
        }
        else
        {
            node->GetStmts()->AddChild(new cReturnStmt(NULL));
        }
    }
}

void cSemantics::Visit(cReturnStmt *node)
{
    if (m_funcReturnType->IsVoid() && node->GetExpr() == nullptr) return;
    if (m_funcReturnType->IsVoid() && node->GetExpr() != nullptr) 
    {
        semantic_error("Return statement type is incompatible with function declaration",
            node->LineNumber());
    }
    else if (!m_funcReturnType->IsVoid() && node->GetExpr() == nullptr)
    {
        semantic_error("Return statement type is incompatible with function declaration",
            node->LineNumber());
    }
    else if (!cTypeDecl::IsCompatibleWith(m_funcReturnType, 
                                     node->GetExpr()->GetType()))
    {
        semantic_error("Return statement type is incompatible with function declaration",
            node->LineNumber());
    }
}

void cSemantics::Visit(cWhileStmt *node)
{
    ++s_JumpContextLevel;
    VisitAllChildren(node);
    --s_JumpContextLevel;
}