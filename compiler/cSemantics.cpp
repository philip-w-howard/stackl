#include <iostream>
#include "cSemantics.h"
#include "lex.h"

#include <stack>
#include <unordered_map>

using std::string;

//************************************
// function label scope helper data
typedef std::unordered_map<string, int> FunctionLabelScope;
static std::stack<FunctionLabelScope> sFunctionLabelStack;

static void IncreaseFunctionScope() { sFunctionLabelStack.emplace(FunctionLabelScope{}); }
static void DecreaseFunctionScope()
{
    for (auto label : sFunctionLabelStack.top())
    {
        if (label.second != -1)
            semantic_error("Reference to undefined label \"" + label.first + "\"", label.second);
    }
    sFunctionLabelStack.pop();
}

static bool InFunctionScope()                                   { return !sFunctionLabelStack.empty(); }
static bool LabelExists(std::string label)                      { return sFunctionLabelStack.top().find(label) != sFunctionLabelStack.top().end(); }
static bool IsLabelMissing(std::string label)                   { return LabelExists(label) && sFunctionLabelStack.top().find(label)->second > 0; }
static void SetMissingLabel(std::string label, int lineNumber)  { sFunctionLabelStack.top()[label] = lineNumber; }
static void SetFoundLabel(std::string label)                    { sFunctionLabelStack.top()[label] = -1; }

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
        SetMissingLabel(node->GetLabel(), node->LineNumber());
    }
}

void cSemantics::Visit(cLabeledStmt *node)
{
    if (!InFunctionScope())
    {
        semantic_error("Label \"" + node->GetLabel() + "\" declared outside of function body", node->LineNumber());
    }
    else if (LabelExists(node->GetLabel()) && !IsLabelMissing(node->GetLabel()))
    {
        semantic_error("Duplicate label \"" + node->GetLabel() + "\"", node->LineNumber());
    }
    else
    {
        SetFoundLabel(node->GetLabel());
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
