#include "lex.h"
#include "cAstXml.h"

cAstXml::cAstXml(std::string filename) : cVisitor()
{
    output.open(filename, std::ofstream::out);
    if (!output.is_open()) fatal_error("Unable to open " + filename);
}

cAstXml::~cAstXml()
{
    if (output.is_open()) output.close();
}

void cAstXml::VisitAllNodes(cAstNode *node) { node->Visit(this); }

void cAstXml::DefaultVisit(cAstNode *node, std::string name, std::string attr)
{
    output << "<" + name + attr;

    if (node->HasChildren())
    {
        output << ">";

        VisitAllChildren(node);
    }

    if (node->HasChildren()) 
        output <<  "</" + name + ">\n";
    else
        output <<  "/>";
}

void cAstXml::Visit(cAddressExpr *node)
{
    DefaultVisit(node, "AddressExpr");
}
void cAstXml::Visit(cArrayRef *node)
{
    DefaultVisit(node, "ArrayRef");
}
void cAstXml::Visit(cArrayType *node)
{
    DefaultVisit(node, "ArrayType");
}
void cAstXml::Visit(cAsmNode *node)
{
    string attr("");

    attr = " Op=\"" + node->GetOp1String() + "\" ";

    if (node->UsesTwoArgs())
    {
        attr += "Op2=\"" + std::to_string(node->GetOp2()) + "\" ";
    }
    DefaultVisit(node, "Asm", attr);
}
void cAstXml::Visit(cAssignExpr *node)
{
    DefaultVisit(node, "AssignExpr");
}
void cAstXml::Visit(cAstNode *node)
{
    DefaultVisit(node, "AstNode");
}
void cAstXml::Visit(cBaseDeclNode *node)
{
    string attr = " size=\"" + std::to_string(node->Size()) + "\" ";
    DefaultVisit(node, "BaseDecl", attr);
}
void cAstXml::Visit(cBinaryExpr *node)
{
    string attr = " op=\"" + node->OpToString() + "\" ";
    DefaultVisit(node, "BinaryExpr", attr);
}
void cAstXml::Visit(cDecl *node)
{
    DefaultVisit(node, "Decl");
}
void cAstXml::Visit(cDeclsList *node)
{
    DefaultVisit(node, "DeclsList");
}
void cAstXml::Visit(cExpr *node)
{
    DefaultVisit(node, "Expr");
}
void cAstXml::Visit(cExprStmt *node)
{
    DefaultVisit(node, "ExprStmt");
}
void cAstXml::Visit(cForStmt *node)
{
    DefaultVisit(node, "ForStmt");
}
void cAstXml::Visit(cFuncCall *node)
{
    DefaultVisit(node, "FuncCall");
}
void cAstXml::Visit(cFuncDecl *node)
{
    string attr = " localsize=\"" + std::to_string(node->GetSize()) + "\" ";
    DefaultVisit(node, "FuncDecl", attr);
}
void cAstXml::Visit(cIfStmt *node)
{
    DefaultVisit(node, "IfStmt");
}
void cAstXml::Visit(cIntExpr *node)
{
    string attr = " value=\"" + std::to_string(node->ConstValue()) +"\" ";
    DefaultVisit(node, "IntExpr", attr);
}
void cAstXml::Visit(cNopStmt *node)
{
    DefaultVisit(node, "Nop");
}
void cAstXml::Visit(cParams *node)
{
    DefaultVisit(node, "Params");
}
void cAstXml::Visit(cPlainVarRef *node)
{
    DefaultVisit(node, "PlainVarRef");
}
void cAstXml::Visit(cPointerDeref *node)
{
    DefaultVisit(node, "PoitnerDeref");
}
void cAstXml::Visit(cPointerType *node)
{
    DefaultVisit(node, "PointerType");
}
void cAstXml::Visit(cPostfixExpr *node)
{
    string attr = " op=\"" + node->OpToString() + "\" ";
    DefaultVisit(node, "PostfixExpr", attr);
}
void cAstXml::Visit(cPragma *node)
{
    string attr = " name=\"" + node->GetOp() + "\" ";
    
    if (node->GetArg().length() > 0)
    {
        attr += " arg=\"" + node->GetArg() + "\" ";
    }
    DefaultVisit(node, "Pragma", attr);             
}

void cAstXml::Visit(cPrefixExpr *node)
{
    string attr = " op=\"" + node->OpToString() + "\" ";
    DefaultVisit(node, "PrefixExpr", attr);
}
void cAstXml::Visit(cReturnStmt *node)
{
    DefaultVisit(node, "Return");
}
void cAstXml::Visit(cShortCircuitExpr *node)
{
    string attr = " op=\"" + node->OpToString() + "\" ";
    DefaultVisit(node, "ShortCircuitExpr", attr);
}
void cAstXml::Visit(cSizeofExpr *node)
{
    string attr = " value=\"" + std::to_string(node->ConstValue()) +"\" ";
    DefaultVisit(node, "Sizeof");
}
void cAstXml::Visit(cStmt *node)
{
    DefaultVisit(node, "Stmt");
}
void cAstXml::Visit(cStmtsList *node)
{
    DefaultVisit(node, "StmtsList");
}
void cAstXml::Visit(cStringLit *node)
{
    string attr = " value=\"" + node->GetString() + "\" ";
    DefaultVisit(node, "StringLit", attr);
}
void cAstXml::Visit(cStructDeref *node)
{
    DefaultVisit(node, "StructDeref");
}
void cAstXml::Visit(cStructRef *node)
{
    DefaultVisit(node, "StructRef");
}
void cAstXml::Visit(cStructType *node)
{
    DefaultVisit(node, "StructType");
}
void cAstXml::Visit(cSymbol *node)
{
    string attr = " name=\"" + node->Name() + "\"";
    attr += " seq=\"" + std::to_string(node->GetSeq()) + "\" ";
    DefaultVisit(node, "Symbol", attr);
}
void cAstXml::Visit(cTypeDecl *node)
{
    DefaultVisit(node, "TypeDecl");
}
void cAstXml::Visit(cUnaryExpr *node)
{
    string attr = " op=\"" + node->OpToString() + "\" ";
    DefaultVisit(node, "UnaryExpr", attr);
}
void cAstXml::Visit(cVarDecl *node)
{
    string attr = " offset=\"" + std::to_string(node->GetOffset()) + "\" ";
    if (node->IsGlobal()) attr += " global=\"true\" ";
    DefaultVisit(node, "VarDecl", attr);
}
void cAstXml::Visit(cVarRef *node){
    DefaultVisit(node, "VarRef");
}
void cAstXml::Visit(cWhileStmt *node){
    DefaultVisit(node, "While");
}

