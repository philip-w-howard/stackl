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

#include "cAstNode.h"
#include "cBlockNode.h"
#include "cIntExprNode.h"
#include "cUnaryOpNode.h"
#include "cPrintNode.h"
#include "cAsmNode.h"
#include "cSizeOfNode.h"
#include "cRTINode.h"
#include "cPrintcNode.h"
#include "cScanNode.h"
#include "cReturnNode.h"
#include "cDeclsNode.h"
#include "cStmtsNode.h"
#include "cVarDeclNode.h"
#include "cConstDeclNode.h"
#include "cStructDeclNode.h"
#include "cArrayDeclNode.h"
#include "cVarAddrNode.h"
#include "cVarRefNode.h"
#include "cVarPartNode.h"
#include "cIfNode.h"
#include "cForNode.h"
#include "cWhileNode.h"
#include "cAssignNode.h"
#include "cAssignAsmNode.h"
#include "cFuncDeclNode.h"
#include "cFuncCallNode.h"
#include "cParamsSpecNode.h"
#include "cFuncStmtNode.h"
#include "cParamNode.h"
#include "cArraySpecNode.h"
#include "cArrayValNode.h"
#include "cStringLitNode.h"
#include "cBinaryExprNode.h"
#include "cShortCircuitNode.h"
#include "cBaseDeclNode.h"
#include "cVarDerefNode.h"

#include "parser.h"

#include "parse.h"

int yylex();

extern char *yytext;
extern int yylineno;
extern int yynerrs;
extern FILE *yyin;
extern char yycurrentfile[];
