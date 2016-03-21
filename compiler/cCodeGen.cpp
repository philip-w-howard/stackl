#include <iostream>
#include <fstream>
#include <string>


#include "../interp/machine_def.h"

#include "cFixupTable.h"
#include "lex.h"
#include "cCodeGen.h"
#include "cGenAddr.h"

using std::string;

const string cFixupTable::GlobalLabel = "$global$";
const int cCodeGen::STACKL_WORD_SIZE = WORD_SIZE;

cCodeGen::cCodeGen(string filename) : cVisitor()
{
    m_Output.open(filename);
    if (!m_Output.is_open())
    {
        fatal_error("Unable to open output file");
    }

    m_Filename = filename;
    m_GenAddr = new cGenAddr(this);
    m_FixupTable = new cFixupTable();

    // Leave room for ISR address (if any)
    SetLabelRef("interrupt");
    SetLabelRef("systrap");

    EmitInt(JUMP_OP);
    SetLabelRef("startup__");
    EmitInt(POP_OP);            // need to throw away the return value
    EmitInt(HALT_OP);
}

cCodeGen::~cCodeGen()
{
    m_FixupTable->FixupOutput(this);

    m_Output << "X\n";
    m_Output.close();

    string makebin("makebin ");
    makebin += m_Filename;

    int result = system(makebin.c_str());

    if (result < 0) fatal_error("Error creating binary output");
}

void cCodeGen::VisitAllNodes(cAstNode *node) { node->Visit(this); }

void cCodeGen::Visit(cAddressExpr *node)
{
    cVarRef *var = node->GetVar();
    if (var == NULL)
        fatal_error("address of without underlying cVarRef");

    var->Visit(m_GenAddr);
}

void cCodeGen::Visit(cArrayRef *node)
{
    node->Visit(m_GenAddr);
    if (node->GetType()->ElementSize() == 1)
        EmitInt(PUSHCVARIND_OP);
    else
        EmitInt(PUSHVARIND_OP);
}

void cCodeGen::Visit(cArrayType *node)
{ }     // Don't gen code for array size

void cCodeGen::Visit(cAsmNode *node)
{
    if (node->GetParams() != NULL) node->GetParams()->Visit(this);
    EmitInt(node->GetOp1());
    if (node->UsesTwoArgs()) EmitInt(node->GetOp2());
}

void cCodeGen::Visit(cAssignExpr *node)
{
    node->GetExpr()->Visit(this);

    // Need to dup the result in case the assign is treated as an expr
    EmitInt(DUP_OP);
    node->GetVar()->Visit(m_GenAddr);
    if (node->GetVar()->IsArrayRef() && 
        node->GetVar()->GetType()->ElementSize() == 1)
    {
        EmitInt(POPCVARIND_OP);
    }
    else if (node->GetVar()->GetType()->Size() == 1)
        EmitInt(POPCVARIND_OP);
    else
        EmitInt(POPVARIND_OP);
}

//void cCodeGen::Visit(cAstNode *node)            { VisitAllChildren(node); }
//void cCodeGen::Visit(cBaseDeclNode *node)       { VisitAllChildren(node); }
void cCodeGen::Visit(cBinaryExpr *node)
{
    node->GetLeft()->Visit(this);
    node->GetRight()->Visit(this);
    EmitInt(node->OpAsInt());
}
//void cCodeGen::Visit(cDecl *node)               { VisitAllChildren(node); }
//void cCodeGen::Visit(cDeclsList *node)          { VisitAllChildren(node); }
//void cCodeGen::Visit(cExpr *node)               { VisitAllChildren(node); }

void cCodeGen::Visit(cExprStmt *node)
{
    node->GetExpr()->Visit(this);

    // remove the result from the stack
    EmitInt(POP_OP);
}

void cCodeGen::Visit(cForStmt *node)
{
    std::string start_loop = GenerateLabel();
    std::string end_loop = GenerateLabel();

    node->GetInit()->Visit(this);
    EmitInt(POP_OP);            // need to handle VOID
    SetLabelValue(start_loop);
    node->GetExpr()->Visit(this);
    EmitInt(JUMPE_OP);
    SetLabelRef(end_loop);
    node->GetStmt()->Visit(this);
    node->GetUpdate()->Visit(this);
    EmitInt(POP_OP);            // need to handle VOID
    EmitInt(JUMP_OP);
    SetLabelRef(start_loop);
    SetLabelValue(end_loop);
}

void cCodeGen::Visit(cFuncCall *node)
{
    if (node->GetParams() != NULL) node->GetParams()->Visit(this);

    EmitInt(CALL_OP);
    SetLabelRef(node->GetFuncName());

    // Need to pop the args off the stack without affecting the return val
    if (node->GetParams() != NULL)
    {
        for (int ii=0; ii<node->GetParams()->Size()/WORD_SIZE; ii++)
        {
            EmitInt(SWAP_OP);
            EmitInt(POP_OP);
        }
    }
}

void cCodeGen::Visit(cFuncDecl *node)
{
    if (node->IsDefinition())
    {
        EmitComment(node->GetName()->Name() + "\n");
        SetLabelValue(node->GetName()->Name());
        int adj_size = (node->DeclsSize() / WORD_SIZE * WORD_SIZE) + WORD_SIZE;
        if (node->DeclsSize() != 0)
        {
            EmitInt(ADJSP_OP);
            EmitInt(adj_size);
        }

        node->GetStmts()->Visit(this);

        // Force return statement
        cReturnStmt *ret = new cReturnStmt(new cIntExpr(0));
        ret->Visit(this);
    }
}

void cCodeGen::Visit(cIfStmt *node)
{
    std::string if_label = GenerateLabel();
    node->GetCond()->Visit(this);
    EmitInt(JUMPE_OP);
    SetLabelRef(if_label);
    if (node->GetIfStmt() != NULL) node->GetIfStmt()->Visit(this);

    if (node->GetElseStmt() != NULL)
    {
        std::string else_label = GenerateLabel();
        EmitInt(JUMP_OP);
        SetLabelRef(else_label);
        SetLabelValue(if_label);
        node->GetElseStmt()->Visit(this);
        SetLabelValue(else_label);
    }
    else
    {
        SetLabelValue(if_label);
    }
}

void cCodeGen::Visit(cIntExpr *node)
{
    EmitInt(PUSH_OP);
    EmitInt(node->ConstValue());
}

//void cCodeGen::Visit(cNopStmt *node)

void cCodeGen::Visit(cParams *node)
{
    // Visit the children in the reverse order
    cAstNode::iterator it = node->LastChild(); 
    do
    {
        it--;
        (*it)->Visit(this);
    } while (it != node->FirstChild());
}

void cCodeGen::Visit(cPlainVarRef *node)
{
    cVarDecl *var = node->GetDecl();

    if (var->GetType()->IsArray())
    {
        node->Visit(m_GenAddr);
    } else {
        if (var->IsGlobal())
        {
            node->Visit(m_GenAddr);
            
            if (var->GetType()->Size() == 1)    // ElementSize?
                EmitInt(PUSHCVARIND_OP);
            else
                EmitInt(PUSHVARIND_OP);
        } else {
            if (var->GetType()->Size() == 1)
                EmitInt(PUSHCVAR_OP);
            else
                EmitInt(PUSHVAR_OP);

            EmitInt(var->GetOffset());
        }
    }
}

void cCodeGen::Visit(cPointerDeref *node)
{
    node->GetBase()->Visit(this);
    if (node->GetType()->Size() == 1)     // EmementSize?
    {
        EmitInt(PUSHCVARIND_OP);
    } else {
        EmitInt(PUSHVARIND_OP);
    }
}

//void cCodeGen::Visit(cPointerType *node)

void cCodeGen::Visit(cPostfixExpr *node)
{
    cVarRef *var = node->GetExpr();

    cBinaryExpr *performOp = 
        new cBinaryExpr(var, node->GetOp(), new cIntExpr(1));

    performOp->Visit(this);
    EmitInt(DUP_OP);
    var->Visit(m_GenAddr);
    if (var->GetType()->Size() == 1)
        EmitInt(POPCVARIND_OP);
    else
        EmitInt(POPVARIND_OP);
}

void cCodeGen::Visit(cPrefixExpr *node)
{
    cVarRef *var = node->GetExpr();

    node->GetExpr()->Visit(this);
    cBinaryExpr *performOp = new 
        cBinaryExpr(node->GetExpr(), node->GetOp(), new cIntExpr(1));

    performOp->Visit(this);
    var->Visit(m_GenAddr);
    if (var->GetType()->Size() == 1)
        EmitInt(POPCVARIND_OP);
    else
        EmitInt(POPVARIND_OP);
}

void cCodeGen::Visit(cReturnStmt *node)
{
    if (node->GetExpr() != NULL) 
    {
        node->GetExpr()->Visit(this);
        EmitInt(RETURNV_OP);
    } else {
        EmitInt(RETURN_OP);
    }
}

void cCodeGen::Visit(cShortCircuitExpr *node)
{
    if(node->ShortOnTrue())
    {
        // get a label to jump to
        std::string skipExpression = GenerateLabel();
        std::string checkExpression = GenerateLabel();

        // at a minimum, we want to output the left expression
        node->GetLeft()->Visit(this);
        EmitInt(DUP_OP);
        EmitInt(JUMPE_OP);
        SetLabelRef(checkExpression);
        EmitInt(JUMP_OP);
        SetLabelRef(skipExpression);
        SetLabelValue(checkExpression);
        node->GetRight()->Visit(this);
        EmitInt(node->OpAsInt());
        SetLabelValue(skipExpression);
    }
    else
    {
        // get a label to jump to
        std::string jmp = GenerateLabel();

        // at a minimum, we want to output the left expression
        node->GetLeft()->Visit(this);

        // if the result of the left expression left a 0
        // on the stack, skip the right expression
        EmitInt(DUP_OP);
        EmitInt(JUMPE_OP);
        SetLabelRef(jmp);

        // generate the code for the right expression
        node->GetRight()->Visit(this);

        // generate the code for the operator
        EmitInt(node->OpAsInt());

        // jump to the end if the left expression was false
        SetLabelValue(jmp);
    }
}

void cCodeGen::Visit(cSizeofExpr *node)
{
    EmitInt(PUSH_OP);
    EmitInt(node->ConstValue());
}

//void cCodeGen::Visit(cStmt *node)               { VisitAllChildren(node); }
//void cCodeGen::Visit(cStmtsList *node)

void cCodeGen::Visit(cStringLit *node)
{
    EmitInt(PUSH_OP);
    EmitString(node->GetString());
}

void cCodeGen::Visit(cStructDeref *node)
{
    node->Visit(m_GenAddr);

    if (node->GetField()->GetDecl()->GetType()->IsArray())
    {
        // do nothing: we want the addr
    }
    else
    {
        if (node->GetField()->GetDecl()->GetType()->Size() == 1)
            EmitInt(PUSHCVARIND_OP);
        else
            EmitInt(PUSHVARIND_OP);
    }
}

void cCodeGen::Visit(cStructRef *node)
{
    node->Visit(m_GenAddr);

    if (node->GetField()->GetDecl()->GetType()->IsArray())
    {
        // do nothing: we want the addr
    }
    else
    {
        if (node->GetField()->GetDecl()->GetType()->Size() == 1)
            EmitInt(PUSHCVARIND_OP);
        else
            EmitInt(PUSHVARIND_OP);
    }
}

//void cCodeGen::Visit(cStructType *node)
//void cCodeGen::Visit(cSymbol *node)             { VisitAllChildren(node); }
//void cCodeGen::Visit(cTypeDecl *node)           { VisitAllChildren(node); }

void cCodeGen::Visit(cUnaryExpr *node)
{
    node->GetExpr()->Visit(this);

    switch (node->GetOp())
    {
        case '-':
            EmitInt(NEG_OP);
            break;
        case '~':
            EmitInt(COMP_OP);
            break;
        case '!':
            EmitInt(NOT_OP);
            break;
        default:
            fatal_error("Unrecognized unary operator");
            break;
    }
}

void cCodeGen::Visit(cVarDecl *node)
{
    // don't visit children
}

//void cCodeGen::Visit(cVarRef *node)             { VisitAllChildren(node); }

void cCodeGen::Visit(cWhileStmt *node)
{
    std::string start_loop = GenerateLabel();
    std::string end_loop = GenerateLabel();

    SetLabelValue(start_loop);
    node->GetCond()->Visit(this);
    EmitInt(JUMPE_OP);
    SetLabelRef(end_loop);
    node->GetStmt()->Visit(this);
    EmitInt(JUMP_OP);
    SetLabelRef(start_loop);
    SetLabelValue(end_loop);
}

//*****************************************
// write an integer constant to the output
void cCodeGen::EmitInt(int val)
{
    m_Output << "D " << m_Location << " " << val << "\n";
    m_Location += WORD_SIZE;
}
//*****************************************
// Generate a unique label for GOTO statements
string cCodeGen::GenerateLabel()
{
    m_Next_Label++;
    string label("LABEL_");
    label += std::to_string(m_Next_Label);
    return label;
}
//*****************************************
void cCodeGen::EmitFixup(int loc, int dest)
{
    m_Output << "F " << loc << " " << dest << "\n";
}
//*****************************************
void cCodeGen::EmitString(string str)
{
    string label = GenerateLabel();
    SetLabelRef(label);
    m_FixupTable->FixupAddString(str, label);
    // m_Output << "S " << str << "\n";
    // m_Location += WORD_SIZE;
}
//*****************************************
void cCodeGen::EmitActualString(string str)
{
    int size;
    m_Output << "S " << m_Location << " " << str << "\n";
    size = (str.size() + WORD_SIZE)/WORD_SIZE;
    size *= WORD_SIZE;
    m_Location += size;
}
//*****************************************
void cCodeGen::EmitGlobalDef(string str, int size)
{
    m_FixupTable->FixupAddGlobal(str, size);
}
//*****************************************
void cCodeGen::EmitGlobalRef(string str)
{
    SetLabelRef(cFixupTable::GlobalLabel);
}
//*****************************************
void cCodeGen::EmitActualGlobal(string str, int size)
{
    m_Output << "G " << m_Location << " " << str << " " << size << "\n";
    size = (size + WORD_SIZE - 1)/WORD_SIZE;
    size *= WORD_SIZE;
    m_Location += size;
}
//*****************************************
void cCodeGen::EmitComment(string str)
{
    m_Output << "C " << m_Location << " " << str << "\n";
}
//*****************************************
void cCodeGen::SetLabelRef(string label)
{
    m_FixupTable->AddLabelRef(label, m_Location);
    EmitInt(-99);
}
//*****************************************
void cCodeGen::SetLabelValue(string label)
{
    m_FixupTable->AddLabelValue(label, m_Location);
}
