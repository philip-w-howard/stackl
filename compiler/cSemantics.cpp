#include <iostream>
#include "cSemantics.h"
#include "lex.h"

using std::string;

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

void cSemantics::Visit(cFuncDecl *node)
{
    m_funcReturnType = node->ReturnType();

    VisitAllChildren(node);

    // Force a return statement
    if (!node->ReturnType()->IsVoid())
    {
        node->AddChild(new cReturnStmt(new cIntExpr(0)));
    }
    else
    {
        node->AddChild(new cReturnStmt(NULL));
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
