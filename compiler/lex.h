#pragma once
//*******************************************************
// Purpose: include all AST nodes, parser globals and scanner globals
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include "cAddressExpr.h"
#include "cArrayRef.h"
#include "cArrayType.h"
#include "cAsmNode.h"
#include "cAssignExpr.h"
#include "cAstNode.h"
#include "cBaseDeclNode.h"
#include "cBinaryExpr.h"
#include "cCastExpr.h"
#include "cDecl.h"
#include "cDeclsList.h"
#include "cDoWhileStmt.h"
#include "cExpr.h"
#include "cExprStmt.h"
#include "cForStmt.h"
#include "cFuncCall.h"
#include "cFuncDecl.h"
#include "cIfStmt.h"
#include "cIntExpr.h"
#include "cNopStmt.h"
#include "cParams.h"
#include "cPlainVarRef.h"
#include "cPointerDeref.h"
#include "cPointerType.h"
#include "cPostfixExpr.h"
#include "cPragma.h"
#include "cPrefixExpr.h"
#include "cReturnStmt.h"
#include "cShortCircuitExpr.h"
#include "cSizeofExpr.h"
#include "cStmt.h"
#include "cStmtsList.h"
#include "cStringLit.h"
#include "cStructDeref.h"
#include "cStructRef.h"
#include "cStructType.h"
#include "cSymbol.h"
#include "cTypeDecl.h"
#include "cTypedef.h"
#include "cUnaryExpr.h"
#include "cVarDecl.h"
#include "cVarRef.h"
#include "cWhileStmt.h"
#include "cTernaryExpr.h"

#include "cSymbolTable.h"
#include "parser.h"
#include "parse.h"

int yylex();
int process_once();

extern char *yytext;
extern int yylineno;
extern int yynerrs;
extern FILE *yyin;
extern char yycurrentfile[];
