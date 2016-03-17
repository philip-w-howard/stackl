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

#include "cSymbol.h"
#include "cSymbolTable.h"
#include "cAstNode.h"
#include "cDecl.h"
#include "cDeclsList.h"
#include "cTypeDecl.h"
#include "cStructType.h"
#include "cPointerType.h"
#include "cArrayType.h"
#include "cVarDecl.h"
#include "cFuncDecl.h"

#include "cStmt.h"
#include "cStmtsList.h"
#include "cExprStmt.h"
#include "cNopStmt.h"
#include "cReturnStmt.h"
#include "cAssignExpr.h"
#include "cIfStmt.h"
#include "cWhileStmt.h"
#include "cForStmt.h"

#include "cExpr.h"
#include "cIntExpr.h"
#include "cBinaryExpr.h"
#include "cShortCircuitExpr.h"
#include "cUnaryExpr.h"
#include "cAsmNode.h"
#include "cVarRef.h"
#include "cPlainVarRef.h"
#include "cStructRef.h"
#include "cArrayRef.h"
#include "cStructDeref.h"
#include "cPointerDeref.h"
#include "cStringLit.h"
#include "cFuncCall.h"
#include "cPostfixExpr.h"
#include "cPrefixExpr.h"
#include "cSizeofExpr.h"
#include "cAddressExpr.h"

#include "cParams.h"

#include "parser.h"

#include "parse.h"

int yylex();

extern char *yytext;
extern int yylineno;
extern int yynerrs;
extern FILE *yyin;
extern char yycurrentfile[];
