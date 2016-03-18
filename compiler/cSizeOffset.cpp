#include "lex.h"
#include "cSizeOffset.h"

cSizeOffset::cSizeOffset() : cVisitor()
{
    m_offset = 0;
}

void cSizeOffset::VisitAllNodes(cAstNode *node) { node->Visit(this); }

/*
void cSizeOffset::Visit(cAddressExpr *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cArrayRef *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cArrayType *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cAsmNode *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cAssignExpr *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cAstNode *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cBaseDeclNode *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cBinaryExpr *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cDecl *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cDeclsList *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cExpr *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cExprStmt *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cForStmt *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cFuncCall *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
*/
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
/*
void cSizeOffset::Visit(cIfStmt *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cIntExpr *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cNopStmt *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cParams *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cPlainVarRef *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cPointerDeref *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cPointerType *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cPostfixExpr *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cPrefixExpr *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cReturnStmt *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cShortCircuitExpr *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cSizeofExpr *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cStmt *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cStmtsList *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cStringLit *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cStructDeref *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cStructRef *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
*/
void cSizeOffset::Visit(cStructType *node)
{
    int offset = m_offset;
    m_offset = 0;

    VisitAllChildren(node);

    node->SetSize(m_offset);
    m_offset = offset;
}
/*
void cSizeOffset::Visit(cSymbol *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cTypeDecl *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cUnaryExpr *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
*/
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
/*
void cSizeOffset::Visit(cVarRef *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
void cSizeOffset::Visit(cWhileStmt *node)
{
    m_offset = node->ComputeOffsets(m_offset);
}
*/
