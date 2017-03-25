#include <iostream>
#include <fstream>
#include <string>

#include "../interp/machine_def.h"

#include "lex.h"
#include "cCodeGen.h"
#include "cGenAddr.h"
#include "../version.h"

using std::string;

const int cCodeGen::STACKL_WORD_SIZE = WORD_SIZE;

void cCodeGen::Write_Header()
{
    //int feature_index = 0;
    //string feature_name;
    //cSymbol *feature;

    m_Output << "#stackl " << VERSION << " stacklc" << std::endl;

    /*
    feature_name = "$$feature" + std::to_string(feature_index++);
    while ((feature = symbolTableRoot->Lookup(feature_name)) != nullptr)
    {
        m_Output << "#" << feature->Name() << std::endl;
        feature_name = "$$feature" + std::to_string(feature_index++);
    }

    // Handle vectors (if defined)
    cSymbol *interrupt = symbolTableRoot->Lookup("$$interrupt");
    if (interrupt != nullptr) EmitInst("#interrupt " + interrupt->Name());

    cSymbol *systrap = symbolTableRoot->Lookup("$$systrap");
    if (systrap != nullptr) EmitInst("#systrap " + systrap->Name());

    cSymbol *startup = symbolTableRoot->Lookup("$$startup");
    if (startup != nullptr) EmitInst("#startup " + startup->Name());

    cSymbol *stack_size = symbolTableRoot->Lookup("$$stack_size");
    if (stack_size != nullptr) EmitInst("#stack_size " + stack_size->Name());
    */

    //m_Output << "#begindata" << std::endl;
}

//***********************************
cCodeGen::cCodeGen(const char * filename) : cVisitor()
{
    m_Output.open(filename);
    if (!m_Output.is_open())
    {
        fatal_error("Unable to open output file");
    }

    m_GenAddr = new cGenAddr(this);

    Write_Header();

}

cCodeGen::~cCodeGen()
{
    m_Output.close();
}

void cCodeGen::VisitAllNodes(cAstNode *node) { node->Visit(this); }

void cCodeGen::EmitLineNumber(cAstNode *node)
{
    string inst = ".source " + node->SourceFile() + " " +  std::to_string(node->LineNumber());
    EmitInst(inst);
    EmitComment("Source: " + node->SourceFile() + " Line: " +
            std::to_string(node->LineNumber()) );
}

void cCodeGen::Visit(cAddressExpr *node)
{
    cVarRef *var = node->GetVar();
    if (var == NULL)
        fatal_error("address of without underlying cVarRef");

    var->Visit(m_GenAddr);
}

void cCodeGen::Visit(cArrayRef *node)
{
    EmitLineNumber(node);
    node->Visit(m_GenAddr);
    if ( node->GetType()->IsPointer() && !node->GetBase()->IsPointer() )
        EmitInst("PUSHVARIND");
    else if (node->GetType()->ElementSize() == 1)
        EmitInst("PUSHCVARIND");
    else
        EmitInst("PUSHVARIND");
}

void cCodeGen::Visit(cArrayType *node)
{ }     // Don't gen code for array size

void cCodeGen::Visit(cAsmNode *node)
{
    EmitLineNumber(node);
    if (node->GetParams() != NULL) node->GetParams()->Visit(this);
    if (node->UsesTwoArgs()) 
        EmitInst(node->GetOp1String(), node->GetOp2());
    else
        EmitInst(node->GetOp1String());
}

void cCodeGen::Visit(cAssignExpr *node)
{
    EmitLineNumber(node);
    node->GetExpr()->Visit(this);

    // Need to dup the result in case the assign is treated as an expr
    EmitInst("DUP");
    node->GetVar()->Visit(m_GenAddr);

    if (node->GetVar()->IsArrayRef() &&
        !node->GetVar()->GetBase()->IsPointer() &&
        node->GetVar()->GetType()->IsPointer())
    {
        EmitInst("POPVARIND");
    }
    else if (node->GetVar()->IsArrayRef() && 
        node->GetVar()->GetType()->ElementSize() == 1)
    {
        EmitInst("POPCVARIND");
    }
    else if (node->GetVar()->GetType()->Size() == 1)
        EmitInst("POPCVARIND");
    else
        EmitInst("POPVARIND");
}

//void cCodeGen::Visit(cAstNode *node)            { VisitAllChildren(node); }
//void cCodeGen::Visit(cBaseDeclNode *node)       { VisitAllChildren(node); }
void cCodeGen::Visit(cBinaryExpr *node)
{
    node->GetLeft()->Visit(this);
    //if (node->GetRight()->GetType()->ParentType()->IsPointer())
    if (node->GetRight()->IsPointer() && 
            !node->GetLeft()->IsPointer() &&
            node->IsArithmetic())
    {
        EmitInst("PUSH " +
                std::to_string(node->GetRight()->GetType()->ElementSize()));
        EmitInst("TIMES");
    }

    node->GetRight()->Visit(this);
    //if (node->GetLeft()->GetType()->ParentType()->IsPointer())
    if (node->GetLeft()->IsPointer() && 
            !node->GetRight()->IsPointer() &&
            node->IsArithmetic())
    {
        EmitInst("PUSH " +
                std::to_string(node->GetLeft()->GetType()->ElementSize()));
        EmitInst("TIMES");
    }
    EmitInst(node->OpAsString());
}
//void cCodeGen::Visit(cDecl *node)               { VisitAllChildren(node); }
//void cCodeGen::Visit(cDeclsList *node)          { VisitAllChildren(node); }
void cCodeGen::Visit(cDoWhileStmt *node)
{
    EmitLineNumber(node);
    std::string start_loop = GenerateLabel();
    std::string end_loop = GenerateLabel();

    EmitLabel(start_loop);
    node->GetStmt()->Visit(this);
    node->GetCond()->Visit(this);
    EmitInst("JUMPE", end_loop);
    EmitInst("JUMP", start_loop);
    EmitLabel(end_loop);
}
//void cCodeGen::Visit(cExpr *node)               { VisitAllChildren(node); }

void cCodeGen::Visit(cExprStmt *node)
{
    node->GetExpr()->Visit(this);

    // remove the result from the stack
    if (!node->GetExpr()->GetType()->IsVoid()) EmitInst("POP");
}

void cCodeGen::Visit(cForStmt *node)
{
    EmitLineNumber(node);
    std::string start_loop = GenerateLabel();
    std::string end_loop = GenerateLabel();

    node->GetInit()->Visit(this);
    EmitInst("POP");            // need to handle VOID
    EmitLabel(start_loop);
    node->GetExpr()->Visit(this);
    EmitInst("JUMPE", end_loop);
    node->GetStmt()->Visit(this);
    node->GetUpdate()->Visit(this);
    EmitInst("POP");            // need to handle VOID
    EmitInst("JUMP", start_loop);
    EmitLabel(end_loop);
}

void cCodeGen::Visit(cFuncCall *node)
{
    EmitLineNumber(node);
    if (node->GetParams() != NULL) node->GetParams()->Visit(this);

    EmitInst("CALL", node->GetFuncName());

    // Need to pop the args off the stack without affecting the return val
    if (node->GetParams() != NULL)
    {
        if (node->GetType()->IsVoid())
        {
            EmitInst("ADJSP -" + std::to_string(node->GetParams()->Size()));
        }
        else
        {
            EmitInst("POPARGS " + std::to_string(node->GetParams()->Size()));
        }

        /*
        for (int ii=0; ii<node->GetParams()->Size()/WORD_SIZE; ii++)
        {
            EmitInst("SWAP");
            EmitInst("POP");
        }
        */
    }
}

void cCodeGen::Visit(cFuncDecl *node)
{
    EmitLineNumber(node);
    if (node->IsDefinition())
    {
        string inst = ".function " + node->GetName()->Name();
        EmitInst(inst);
        EmitComment(node->GetName()->Name() + "\n");
        EmitLabel(node->GetName()->Name());
        int adj_size = (node->DeclsSize() / WORD_SIZE * WORD_SIZE) + WORD_SIZE;
        if (node->DeclsSize() != 0)
        {
            EmitInst("ADJSP", adj_size);
        }

        node->GetStmts()->Visit(this);

        // Force return statement
        if (!node->ReturnType()->IsVoid())
        {
            cReturnStmt *ret = new cReturnStmt(new cIntExpr(0));
            ret->Visit(this);
        }
        else
        {
            EmitInst("RETURN");
        }
    }
}

void cCodeGen::Visit(cIfStmt *node)
{
    EmitLineNumber(node);
    std::string if_label = GenerateLabel();
    node->GetCond()->Visit(this);
    EmitInst("JUMPE", if_label);
    if (node->GetIfStmt() != NULL) node->GetIfStmt()->Visit(this);

    if (node->GetElseStmt() != NULL)
    {
        std::string else_label = GenerateLabel();
        EmitInst("JUMP", else_label);
        EmitLabel(if_label);
        node->GetElseStmt()->Visit(this);
        EmitLabel(else_label);
    }
    else
    {
        EmitLabel(if_label);
    }
}

void cCodeGen::Visit(cIntExpr *node)
{
    EmitInst("PUSH", node->ConstValue());
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
    if (node->IsFunc())
    {
        EmitInst("PUSH", node->GetName()->Name());
    }
    else
    {
        cVarDecl *var = node->GetDecl();

        if (var->IsConst() && var->HasInit())
        {
            var->GetInit()->Visit(this);
        }
        else if (var->GetType()->IsArray())
        {
            node->Visit(m_GenAddr);
        } 
        else if (var->IsGlobal())
        {
            node->Visit(m_GenAddr);
            
            if (var->GetType()->Size() == 1)    // ElementSize?
                EmitInst("PUSHCVARIND");
            else
                EmitInst("PUSHVARIND");
        } 
        else if (var->GetType()->Size() == 1)
            EmitInst("PUSHCVAR", var->GetOffset());
        else
            EmitInst("PUSHVAR", var->GetOffset());
    }
}

void cCodeGen::Visit(cPointerDeref *node)
{
    node->GetBase()->Visit(this);
    if (node->GetType()->Size() == 1)     // EmementSize?
    {
        EmitInst("PUSHCVARIND");
    } else {
        EmitInst("PUSHVARIND");
    }
}

//void cCodeGen::Visit(cPointerType *node)

void cCodeGen::Visit(cPostfixExpr *node)
{
    cBinaryExpr *performOp;

    EmitLineNumber(node);
    cVarRef *var = node->GetExpr();

    // NOTE: Used to adjust for sizeof item for pointer arithmetic,
    // but cBinaryExpr now does that.
    performOp = 
        new cBinaryExpr(node->GetExpr(), node->GetOp(), new cIntExpr(1));

    var->Visit(this);
    performOp->Visit(this);
    var->Visit(m_GenAddr);
    if (var->GetType()->Size() == 1)
        EmitInst("POPCVARIND");
    else
        EmitInst("POPVARIND");
}

void cCodeGen::Visit(cPragma *node)
{
    EmitInst("#" + node->GetOp() + " " + node->GetArg());
}

void cCodeGen::Visit(cPrefixExpr *node)
{
    cBinaryExpr *performOp;

    EmitLineNumber(node);
    cVarRef *var = node->GetExpr();

    // NOTE: Used to adjust for sizeof item for pointer arithmetic,
    // but cBinaryExpr now does that.
    performOp = 
        new cBinaryExpr(node->GetExpr(), node->GetOp(), new cIntExpr(1));

    performOp->Visit(this);

    EmitInst("DUP");

    var->Visit(m_GenAddr);
    if (var->GetType()->Size() == 1)
        EmitInst("POPCVARIND");
    else
        EmitInst("POPVARIND");
}

void cCodeGen::Visit(cReturnStmt *node)
{
    EmitLineNumber(node);
    if (node->GetExpr() != NULL) 
    {
        node->GetExpr()->Visit(this);
        EmitInst("RETURNV");
    } else {
        EmitInst("RETURN");
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
        EmitInst("DUP");
        EmitInst("JUMPE", checkExpression);
        EmitInst("JUMP", skipExpression);

        EmitLabel(checkExpression);
        node->GetRight()->Visit(this);
        EmitInst(node->OpAsString());
        EmitLabel(skipExpression);
    }
    else
    {
        // get a label to jump to
        std::string jmp = GenerateLabel();

        // at a minimum, we want to output the left expression
        node->GetLeft()->Visit(this);

        // if the result of the left expression left a 0
        // on the stack, skip the right expression
        EmitInst("DUP");
        EmitInst("JUMPE", jmp);

        // generate the code for the right expression
        node->GetRight()->Visit(this);

        // generate the code for the operator
        EmitInst(node->OpAsString());

        // jump to the end if the left expression was false
        EmitLabel(jmp);
    }
}

void cCodeGen::Visit(cSizeofExpr *node)
{
    EmitInst("PUSH", node->ConstValue());
}

//void cCodeGen::Visit(cStmt *node)               { VisitAllChildren(node); }
//void cCodeGen::Visit(cStmtsList *node)

void cCodeGen::Visit(cStringLit *node)
{
    string label = GenerateLabel();
    EmitInst("PUSH", label);
    EmitStringLit(node->GetString(), label);
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
            EmitInst("PUSHCVARIND");
        else
            EmitInst("PUSHVARIND");
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
            EmitInst("PUSHCVARIND");
        else
            EmitInst("PUSHVARIND");
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
            EmitInst("NEG");
            break;
        case '~':
            EmitInst("COMP");
            break;
        case '!':
            EmitInst("NOT");
            break;
        default:
            fatal_error("Unrecognized unary operator");
            break;
    }
}

void cCodeGen::Visit(cVarDecl *node)
{
    // NOTE: we explicitly avoid visiting children

    // If global, emit label and allocate space
    EmitLineNumber(node);
    if (node->IsGlobal())
    {
        EmitInst(".dataseg");
        EmitLabel(node->GetName()->Name());
        if (node->IsConst() && node->HasInit())
        {
            EmitInst(".data", node->GetInit()->ConstValue());
        }
        else
        {
            EmitInst(".block", 
                    (node->GetType()->Size() + WORD_SIZE - 1)/WORD_SIZE);
        }
        EmitInst(".codeseg");
    }
    else if (node->HasInit())
    {
        node->GetInit()->Visit(this);

        if (node->GetType()->Size() == 1)
            EmitInst("POPCVAR " + std::to_string(node->GetOffset()));
        else
            EmitInst("POPVAR " + std::to_string(node->GetOffset()));
    }

}

//void cCodeGen::Visit(cVarRef *node)             { VisitAllChildren(node); }

void cCodeGen::Visit(cWhileStmt *node)
{
    EmitLineNumber(node);
    std::string start_loop = GenerateLabel();
    std::string end_loop = GenerateLabel();

    EmitLabel(start_loop);
    node->GetCond()->Visit(this);
    EmitInst("JUMPE", end_loop);
    node->GetStmt()->Visit(this);
    EmitInst("JUMP", start_loop);
    EmitLabel(end_loop);
}

//*****************************************
// Emit an instruction
void cCodeGen::EmitInst(string inst, string label)
{
    m_Output << inst << " @" << label << "\n";
}

//*****************************************
// Emit an instruction
void cCodeGen::EmitInst(string inst, int param)
{
    m_Output << inst << " " << param << "\n";
}

//*****************************************
// Emit an instruction
void cCodeGen::EmitInst(string inst)
{
    m_Output << inst << "\n";
}

//*****************************************
// Generate a unique label for GOTO statements
string cCodeGen::GenerateLabel()
{
    m_Next_Label++;
    string label("$LABEL_");
    label += std::to_string(m_Next_Label);
    return label;
}
//*****************************************
void cCodeGen::EmitStringLit(string str, string label)
{
    m_Output << ".dataseg\n";
    EmitLabel(label);
    m_Output << ".string \"" << str << "\"\n";
    m_Output << ".codeseg\n";
}
//*****************************************
void cCodeGen::EmitComment(string str)
{
    m_Output << "; " << " " << str << "\n";
}
//*****************************************
void cCodeGen::EmitLabel(string label)
{
    m_Output << label << ":\n";
}
