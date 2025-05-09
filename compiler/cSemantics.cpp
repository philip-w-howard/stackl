#include <iostream>
#include "cSemantics.h"
#include "lex.h"

#include <stack>
#include <unordered_map>

using std::string;
using FunctionLabelScope = std::unordered_map<string, int>;

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

    IncreaseFunctionScope();
    VisitAllChildren(node);
    DecreaseFunctionScope();

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

void cSemantics::Visit(cGotoStmt *node)
{
    if (!InFunctionScope())
    {
        semantic_error("Goto encountered outside of function body", node->LineNumber());
    }
    else if (!LabelExists(node->GetLabel()))
    {
        SetLabelLine(node->GetLabel(), node->LineNumber());
    }
}

void cSemantics::Visit(cLabeledStmt *node)
{
    if (!InFunctionScope())
    {
        semantic_error("Label \"" + node->GetLabel() + "\" declared outside of function body", node->LineNumber());
    }
    else if (LabelExists(node->GetLabel()) && GetLabelLine(node->GetLabel()) == -1)
    {
        semantic_error("Duplicate label \"" + node->GetLabel() + "\"", node->LineNumber());
    }
    else
    {
        SetLabelLine(node->GetLabel(), -1);
    }
    VisitAllChildren(node);
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

//***********************************
void cSemantics::IncreaseFunctionScope() { m_funcLabelStack.emplace(FunctionLabelScope{}); }
void cSemantics::DecreaseFunctionScope()
{
    for (auto label : m_funcLabelStack.top())
    {
        if (label.second != -1)
            semantic_error("Reference to undefined label \"" + label.first + "\"", label.second);
    }
    m_funcLabelStack.pop();
}

bool cSemantics::InFunctionScope()                          { return !m_funcLabelStack.empty(); }
bool cSemantics::LabelExists(string label)                  { return m_funcLabelStack.top().find(label) != m_funcLabelStack.top().end(); }
void cSemantics::SetLabelLine(string label, int lineNumber) { m_funcLabelStack.top()[label] = lineNumber; }
int cSemantics::GetLabelLine(string label)                  { return InFunctionScope() ? m_funcLabelStack.top().find(label)->second : -2; }