#include "lex.h"
#include "cSizeOffset.h"
#include "cCodeGen.h"

cSizeOffset::cSizeOffset() : cVisitor()
{
    m_offset = 0;
}

void cSizeOffset::VisitAllNodes(cAstNode *node) { node->Visit(this); }

void cSizeOffset::Visit(cFuncDecl *node)
{
    int old_offset = m_offset;

    m_offset = -STACK_FRAME_SIZE;
    if (node->GetParams() != nullptr)  node->GetParams()->Visit(this);

    m_offset = 0;
    if (node->GetStmts() != nullptr)   node->GetStmts()->Visit(this);

    node->SetSize(m_offset);

    m_offset = old_offset;
}

void cSizeOffset::Visit(cStructType *node)
{
    int offset = m_offset;
    m_offset = 0;

    VisitAllChildren(node);

    m_offset = RoundUp(m_offset);
    node->SetSize(m_offset);
    m_offset = offset;
}

void cSizeOffset::Visit(cVarDecl *node)
{
    int size = node->GetType()->Size();
    int offset;

    if (size == 1)
        node->SetOffset(m_offset);
    else
    {
        offset = RoundUp(m_offset);
        node->SetOffset(offset);
    }

    if (m_offset < 0)
    {
        node->SetOffset( node->GetOffset() - RoundUp(size));
        m_offset = node->GetOffset();
    }
    else
        m_offset = node->GetOffset() + size;
}

int cSizeOffset::RoundUp(int val)
{
    if (val % cCodeGen::STACKL_WORD_SIZE == 0) return val;

    if (val < 0)
    {
        val = val/cCodeGen::STACKL_WORD_SIZE*cCodeGen::STACKL_WORD_SIZE 
            - cCodeGen::STACKL_WORD_SIZE;
    }
    else
    {
        val = val/cCodeGen::STACKL_WORD_SIZE*cCodeGen::STACKL_WORD_SIZE 
            + cCodeGen::STACKL_WORD_SIZE;
    }
    return val;
}
