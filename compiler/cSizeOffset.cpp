#include "lex.h"
#include "cSizeOffset.h"

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

    node->SetSize(m_offset);
    m_offset = offset;
}

void cSizeOffset::Visit(cVarDecl *node)
{
    int size = node->GetType()->Size();

    if (size == 1)
        node->SetOffset(m_offset);
    else if (m_offset % WORD_SIZE != 0)
        node->SetOffset(m_offset/WORD_SIZE*WORD_SIZE + WORD_SIZE);
    else
        node->SetOffset(m_offset);

    if (m_offset < 0)
    {
        node->SetOffset( node->GetOffset() - size);
        m_offset = node->GetOffset();
    }
    else
        m_offset = node->GetOffset() + size;
}
