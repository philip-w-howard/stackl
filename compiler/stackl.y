/* stackl.y Grammar for the STACKL C compiler */

%{
#include <iostream>
#include <string>
#include "lex.h"
%}

%locations
%error-verbose

%union{
    int                 int_val;
    char*               string;
    cSymbol*            symbol;
    cScope*             sym_table;
    cAstNode*           ast_node;
    cDecl*              decl;
    cDeclsList*         decl_list;
    cStructType*        struct_decl;
    cVarDecl*           var_decl;
    cFuncDecl*          func_decl;
    cTypeDecl*          type_decl;
    cStmt*              stmt;
    cParams*            params;
    cStmtsList*         stmtslist;
    cExpr*              expr;
    cVarRef*            var_ref;
    cStringLit*         str;
    }

%{
    int yyerror(const char *msg);
    int semantic_error(std::string msg, int line);

    cDeclsList *yyast_root = NULL;
    int g_Feature = 0;
%}

%start  program

%token <symbol>    IDENTIFIER
%token <symbol>    TYPE_ID
%token <int_val>   INT_VAL
%token <string>    STRING_LIT
%token <symbol>    CHAR

%token STATIC VOLATILE EXTERN
%token  VOID
%token  DO FOR WHILE IF ELSE SWITCH CASE GOTO
%token  BREAK CONTINUE
%token  TYPEDEF STRUCT
%token  RETURN
%token  JUNK_TOKEN
%token  EQ NE GE LE
%token  AND OR
%token  SIZEOF
%token  INC DEC
%token  PLUS_EQ MINUS_EQ TIMES_EQ DIVIDE_EQ MOD_EQ AND_EQ OR_EQ XOR_EQ 
%token  LEFT_EQ RIGHT_EQ
%token  PTR LEFT RIGHT
%token  ASM ASM2
%token  DEFINE CONST
%token  PRAGMA ONCE INTERRUPT SYSTRAP STARTUP FEATURE STACK_SIZE LIBRARY

%type <decl_list>       program

%type <decl_list>       global_decls
%type <decl>            global_decl
%type <decl_list>       decls
%type <decl>            decl
%type <struct_decl>     struct_decl
%type <struct_decl>     struct_header
%type <var_decl>        func_pointer
%type <var_decl>        var_decl
%type <var_decl>        paramspec
/* %type <int_val>         arrayspec */
%type <func_decl>       func_decl
%type <func_decl>       func_header
%type <func_decl>       func_prefix

%type <sym_table>       open
%type <sym_table>       close
%type <type_decl>       type
%type <type_decl>       typedef

%type <decl_list>       paramspec_list
%type <params>          params

%type <stmtslist>       stmts
%type <stmt>            stmt
%type <stmtslist>       block

%type <str>             string_lit
%type <expr>            primary_expression
%type <expr>            postfix_expression
%type <expr>            unary_expression
%type <expr>            cast_expression
%type <expr>            multiplicative_expression
%type <expr>            additive_expression
%type <expr>            shift_expression
%type <expr>            relational_expression
%type <expr>            equality_expression
%type <expr>            and_expression
%type <expr>            exclusive_or_expression
%type <expr>            inclusive_or_expression
%type <expr>            logical_and_expression
%type <expr>            logical_or_expression
%type <expr>            conditional_expression
%type <expr>            constant_expression
%type <expr>            assignment_expression
%type <expr>            expr
%type <expr>            asm_stmt

%type <expr>            lval
%%

program: global_decls           { $$ = $1; 
                                  yyast_root = $$;
                                  if (yynerrs == 0)
                                  {
                                      YYACCEPT;
                                  }
                                  else
                                  {
                                      YYABORT;
                                  }
                                }
    | /* empty */               { yyast_root = NULL; YYACCEPT; }

block:      open stmts close
            { $$ = $2; }
        | '{' '}'
            { 
                $$ = new cStmtsList(new cNopStmt()); 
                if ($$->HasSemanticError()) YYERROR;
            }
open:   '{'
            { $$ = symbolTableRoot->IncreaseScope(); }

close:  '}'
            { $$ = symbolTableRoot->DecreaseScope(); }

decls:      decls decl
            { 
                $$ = $1; $$->AddNode($2); 
                if ($$->HasSemanticError()) YYERROR;
            }
        |   decl
            { 
                $$ = new cDeclsList($1); 
                if ($$->HasSemanticError()) YYERROR;
            }
decl:       var_decl ';'
            { $$ = $1; }
        |   var_decl '=' expr ';'
            { $1->SetInit($3);
              $$ = $1; 
            }
        |   struct_decl ';'
            { $$ = $1; }
        |   typedef ';'
            { $$ = $1; }
        |   error ';'
            { $$ = NULL; }

var_decl:   type IDENTIFIER 
            { 
                $$ = new cVarDecl($1, $2); 
                if ($$->HasSemanticError()) YYERROR;
            }
        |   var_decl '[' constant_expression ']'
            { 
                $$ = new cVarDecl($1, $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
        |   func_pointer
            { $$ = $1; }
        | STRUCT type IDENTIFIER
            {
                $$ = new cVarDecl($2, $3, true);
                if ($$->HasSemanticError()) YYERROR; 
            }

type:   type '*'
            { 
                $$ = cPointerType::PointerType($1); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | TYPE_ID
            { 
                $$ = dynamic_cast<cTypeDecl*>($1->GetDecl()); 
                if ($$->HasSemanticError()) YYERROR;
            }

// Added by Joe
struct_decl:  struct_header open decls close 
                {
                    $$ = $1;
                    $$->AddDecls($2, $3);
                }
// Added by Joe
struct_header:  STRUCT IDENTIFIER 
            {
                $$ = new cStructType($2);
            }
            | STRUCT 
            {
                $$ = new cStructType(nullptr);;
            }
global_decls: global_decls global_decl  
            { 
                $$ = $1; $$->AddNode($2); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | global_decl
            { 
                $$ = new cDeclsList($1); 
                if ($$->HasSemanticError()) YYERROR;
            }

typedef:  TYPEDEF type IDENTIFIER 
            { $$ = new cTypedef($2, $3); }
        | TYPEDEF struct_decl IDENTIFIER 
            { $$ = new cTypedef($2, $3); }
global_decl: func_decl
            { $$ = $1; }
        | STATIC func_decl
            {   $2->SetStatic();
                $$ = $2; 
            }
        | struct_decl ';'
            { $$ = $1; }
        | typedef ';'
            { $$ = $1; }
        | CONST type IDENTIFIER '=' constant_expression ';'
            { 
                cVarDecl *var = new cVarDecl($2, $3);
                var->SetInit($5);
                var->SetGlobal();
                var->SetStatic();
                var->SetConst();
                $$ = var;
                if ($$->HasSemanticError()) YYERROR;
            }
        | DEFINE IDENTIFIER INT_VAL
            { 
                cTypeDecl *type = 
                    symbolTableRoot->Lookup("int")->GetDecl()->GetType();
                cVarDecl *var = new cVarDecl(type, $2);
                var->SetInit(new cIntExpr($3));
                var->SetGlobal();
                var->SetStatic();
                var->SetConst();
                $$ = var;
                if ($$->HasSemanticError()) YYERROR;
            }
        | DEFINE IDENTIFIER '-' INT_VAL
            { 
                cTypeDecl *type = 
                    symbolTableRoot->Lookup("int")->GetDecl()->GetType();
                cVarDecl *var = new cVarDecl(type, $2);
                var->SetInit(new cIntExpr(-$4));
                var->SetGlobal();
                var->SetStatic();
                var->SetConst();
                $$ = var;
                if ($$->HasSemanticError()) YYERROR;
            }
        | EXTERN var_decl ';'
            { 
                $2->SetGlobal();
                $2->SetExtern();
                $$ = $2; 
            }
        | STATIC var_decl ';'
            { 
                $2->SetGlobal();
                $2->SetStatic();
                $$ = $2; 
            }
        | var_decl ';'
            { 
                $1->SetGlobal();
                $$ = $1; 
            }
        |   PRAGMA ONCE { $$ = new cPragma("once", "");
                          if (process_once()) 
                          {
                              yyerror("'#pragma once' in main source file");
                              YYABORT; 
                          }
                        }
        |   PRAGMA INTERRUPT IDENTIFIER 
                        { 
                            $$ = new cPragma("interrupt", $3->Name());
                            symbolTableRoot->InsertRoot("$$interrupt", $3); 
                        }
        |   PRAGMA SYSTRAP IDENTIFIER 
                        { 
                            $$ = new cPragma("systrap", $3->Name());
                            symbolTableRoot->InsertRoot("$$systrap", $3); 
                        }
        |   PRAGMA STARTUP IDENTIFIER 
                        {
                            $$ = new cPragma("startup", $3->Name());
                            symbolTableRoot->InsertRoot("$$startup", $3); 
                        }
        |   PRAGMA FEATURE IDENTIFIER 
                        {
                            $$ = new cPragma("feature", $3->Name());
                            string feature = "$$feature" + 
                                std::to_string(g_Feature++);
                            symbolTableRoot->InsertRoot(feature, $3); 
                        }
        |   PRAGMA LIBRARY STRING_LIT 
                        {
                            $$ = new cPragma("library", $3);
                        }
        |   PRAGMA STACK_SIZE INT_VAL 
                        {
                            $$ = new cPragma("stack_size", std::to_string($3));
                            cSymbol *size = new cSymbol(std::to_string($3));
                            symbolTableRoot->InsertRoot( "$$stack_size", size); 
                        }
        | error ';'
            { $$ = NULL; }
func_decl:  func_header ';'
            { 
                $$ = $1; 
                symbolTableRoot->DecreaseScope();
            }
        |   func_header '{' stmts '}'
            { 
                $1->AddStatements($3);
                $$ = $1; 
                if ($$->HasSemanticError()) YYERROR;
                symbolTableRoot->DecreaseScope();
            }
        |   func_header '{' '}'
            { 
                $1->AddStatements( new cStmtsList(new cNopStmt()) );
                $$ = $1; 
                if ($$->HasSemanticError()) YYERROR;
                symbolTableRoot->DecreaseScope();
            }

func_header:  func_prefix paramspec_list ')' 
            { 
                $1->AddParams($2);
                $$ = $1; 
                if ($$->HasSemanticError()) YYERROR;
            }
        |     func_prefix ')'
            { $$ = $1; }
func_prefix: type IDENTIFIER '('
            { 
                $$ = new cFuncDecl($1, $2);
                symbolTableRoot->IncreaseScope(); 
                if ($$->HasSemanticError()) YYERROR;
            }
func_pointer: type '(' '*' IDENTIFIER ')' '(' paramspec_list ')'
            { semantic_error("Function pointers not implemented", yylineno); YYERROR; }
            | type '(' '*' IDENTIFIER ')' '(' ')'
            { semantic_error("Function pointers not implemented", yylineno); YYERROR; }

paramspec_list:     
            paramspec_list',' paramspec 
            { 
                $$ = $1; $$->AddNode($3); 
                if ($$->HasSemanticError()) YYERROR;
            }
        |   paramspec
            { 
                $$ = new cDeclsList($1); 
                if ($$->HasSemanticError()) YYERROR;
            }
        /*
           need ELLIPSES
        */

paramspec:  var_decl
            { $$ = $1; }

stmts:      stmts stmt
            { 
                $$ = $1; $$->AddNode($2); 
                if ($$->HasSemanticError()) YYERROR;
            }
        |   stmt
            { 
                $$ = new cStmtsList($1); 
                if ($$->HasSemanticError()) YYERROR;
            }

stmt:       decl
            { $$ = $1; }
        |   ';'
            { 
                $$ = new cNopStmt(); 
                if ($$->HasSemanticError()) YYERROR;
            }
        |   IF '(' expr ')' stmt ELSE stmt
            { 
                $$ = new cIfStmt($3, $5, $7); 
                if ($$->HasSemanticError()) YYERROR;
            }
        |   IF '(' expr ')' stmt
            { 
                $$ = new cIfStmt($3, $5, NULL); 
                if ($$->HasSemanticError()) YYERROR;
            }
        |   FOR '(' expr ';' expr ';' expr ')' stmt
            { 
                $$ = new cForStmt($3, $5, $7, $9); 
                if ($$->HasSemanticError()) YYERROR;
            }
        |   WHILE '(' expr ')' stmt
            { 
                $$ = new cWhileStmt($3, $5); 
                if ($$->HasSemanticError()) YYERROR;
            }
        |   DO stmt WHILE '(' expr ')' ';' 
            { 
                $$ = new cDoWhileStmt($5, $2); 
                if ($$->HasSemanticError()) YYERROR;
            }

        |   SWITCH '(' expr ')' stmt
            { semantic_error("Not implemented " + std::to_string(__LINE__), yylineno); }
        |   expr ';'
            { 
                $$ = new cExprStmt($1); 
                if ($$->HasSemanticError()) YYERROR;
            }
        |   block
            { $$ = $1; }
        |   RETURN expr ';'
            { 
                $$ = new cReturnStmt($2); 
                if ($$->HasSemanticError()) YYERROR;
            }
        |   RETURN ';'
            { 
                $$ = new cReturnStmt(NULL); 
                if ($$->HasSemanticError()) YYERROR;
            }
        |   BREAK ';'
            { semantic_error("Not implemented " + std::to_string(__LINE__), yylineno); }
        |   CONTINUE ';'
            { semantic_error("Not implemented " + std::to_string(__LINE__), yylineno); }
        |   GOTO IDENTIFIER ';'
            { semantic_error("Not implemented " + std::to_string(__LINE__), yylineno); }
        |   asm_stmt ';'
            { $$ = $1; }

asm_stmt : ASM '(' string_lit ')' 
            { 
                $$ = new cAsmNode($3, NULL); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | ASM '(' string_lit ',' params ')'
            { 
                $$ = new cAsmNode($3, $5); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | ASM2 '(' string_lit ',' constant_expression ')' 
            { 
                $$ = new cAsmNode($3, $5->ConstValue(), NULL); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | ASM2 '(' string_lit  ',' constant_expression ',' params ')'
            { 
                $$ = new cAsmNode($3, $5->ConstValue(), $7); 
                if ($$->HasSemanticError()) YYERROR;
            }
lval:     unary_expression
            { $$ = $1; }
params:     params',' assignment_expression
            { 
                $$ = $1; $$->AddNode($3); 
                if ($$->HasSemanticError()) YYERROR;
            }
        |   assignment_expression
            { 
                $$ = new cParams($1); 
                if ($$->HasSemanticError()) YYERROR;
            }

string_lit: STRING_LIT
            { 
                $$ = new cStringLit($1); 
                if ($$->HasSemanticError()) YYERROR;
            }

primary_expression
	: IDENTIFIER
            { 
                $$ = new cPlainVarRef($1); 
                if ($$->HasSemanticError()) YYERROR;
            }
	| INT_VAL
            { 
                $$ = new cIntExpr($1); 
                if ($$->HasSemanticError()) YYERROR;
            }
	| string_lit
            { $$ = $1; }
	| '(' expr ')'
            { $$ = $2; }

	;

postfix_expression
	: primary_expression
            { $$ = $1; }
	| postfix_expression '[' expr ']'
            { 
                $$ = new cArrayRef($1, $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	| postfix_expression '(' params ')'
            { 
                $$ = new cFuncCall($1, $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | postfix_expression '(' ')' 
            { 
                $$ = new cFuncCall($1, NULL); 
                if ($$->HasSemanticError()) YYERROR;
            }
	| postfix_expression '.' IDENTIFIER
            { 
                $$ = new cStructRef($1, $3);
                if ($$->HasSemanticError()) YYERROR;
            }
	| postfix_expression PTR IDENTIFIER
            { 
                $$ = new cStructDeref($1, $3);
                if ($$->HasSemanticError()) YYERROR;
            }
	| postfix_expression INC
            { 
                $$ = new cPostfixExpr($1, '+'); 
                if ($$->HasSemanticError()) YYERROR;
            }
	| postfix_expression DEC
            { 
                $$ = new cPostfixExpr($1, '-'); 
                if ($$->HasSemanticError()) YYERROR;
            }
	;

unary_expression
	: postfix_expression
            { $$ = $1; }
	| INC unary_expression
            { 
                $$ = new cPrefixExpr('+', $2); 
                if ($$->HasSemanticError()) YYERROR;
            }
	| DEC unary_expression
            { 
                $$ = new cPrefixExpr('-', $2); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | '+' cast_expression
            { $$ = $2; }
        | '-' cast_expression
            { 
                $$ = new cUnaryExpr('-', $2); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | '~' cast_expression
            { 
                $$ = new cUnaryExpr('~', $2); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | '!' cast_expression
            { 
                $$ = new cUnaryExpr('!', $2); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | '*' cast_expression
            { 
                $$ = new cPointerDeref($2); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | '&' cast_expression
            { 
                $$ = new cAddressExpr($2);
                if ($$->HasSemanticError()) YYERROR;
            }
	| SIZEOF unary_expression 
            { 
                $$ = new cSizeofExpr($2);
                if ($$->HasSemanticError()) YYERROR;
            }
	| SIZEOF '(' type ')'
            { 
                $$ = new cSizeofExpr($3);
                if ($$->HasSemanticError()) YYERROR;
            }
	;

cast_expression
	: unary_expression
            { $$ = $1; }
	| '(' type ')' cast_expression      
            { $$ = new cCastExpr($2, $4); }
	;

multiplicative_expression
	: cast_expression
            { $$ = $1; }
	| multiplicative_expression '*' cast_expression
            { 
                $$ = new cBinaryExpr($1, '*', $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	| multiplicative_expression '/' cast_expression
            { 
                $$ = new cBinaryExpr($1, '/', $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	| multiplicative_expression '%' cast_expression
            { 
                $$ = new cBinaryExpr($1, '%', $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	;

additive_expression
	: multiplicative_expression
            { $$ = $1; }
	| additive_expression '+' multiplicative_expression
            { 
                $$ = new cBinaryExpr($1, '+', $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	| additive_expression '-' multiplicative_expression
            { 
                $$ = new cBinaryExpr($1, '-', $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	;

shift_expression
	: additive_expression
            { $$ = $1; }
	| shift_expression LEFT additive_expression
            { 
                $$ = new cBinaryExpr($1, LEFT, $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	| shift_expression RIGHT additive_expression
            { 
                $$ = new cBinaryExpr($1, RIGHT, $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	;

relational_expression
	: shift_expression
            { $$ = $1; }
	| relational_expression '<' shift_expression
            { 
                $$ = new cBinaryExpr($1, '<', $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	| relational_expression '>' shift_expression
            { 
                $$ = new cBinaryExpr($1, '>', $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	| relational_expression LE shift_expression
            { 
                $$ = new cBinaryExpr($1, LE, $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	| relational_expression GE shift_expression
            { 
                $$ = new cBinaryExpr($1, GE, $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	;

equality_expression
	: relational_expression
            { $$ = $1; }
	| equality_expression EQ relational_expression
            { 
                $$ = new cBinaryExpr($1, EQ, $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	| equality_expression NE relational_expression
            { 
                $$ = new cBinaryExpr($1, NE, $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	;

and_expression
	: equality_expression
            { $$ = $1; }
	| and_expression '&' equality_expression
            { 
                $$ = new cBinaryExpr($1, '&', $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	;

exclusive_or_expression
	: and_expression
            { $$ = $1; }
	| exclusive_or_expression '^' and_expression
            { 
                $$ = new cBinaryExpr($1, '^', $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	;

inclusive_or_expression
	: exclusive_or_expression
            { $$ = $1; }
	| inclusive_or_expression '|' exclusive_or_expression
            { 
                $$ = new cBinaryExpr($1, '|', $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
	;

logical_and_expression
	: inclusive_or_expression
            { $$ = $1; }
	| logical_and_expression AND inclusive_or_expression
            { 
                $$ = new cShortCircuitExpr($1, AND, $3, false); 
                if ($$->HasSemanticError()) YYERROR;
            }
	;

logical_or_expression
	: logical_and_expression
            { $$ = $1; }
	| logical_or_expression OR logical_and_expression
            { 
                $$ = new cShortCircuitExpr($1, OR, $3, true); 
                if ($$->HasSemanticError()) YYERROR;
            }

	;

conditional_expression
        : logical_or_expression     { $$ = $1; }
        | logical_or_expression '?' expr ':' conditional_expression
            {
                $$ = new cTernaryExpr($1, $3, $5);
                if ($$->HasSemanticError()) YYERROR;
            }

constant_expression
        : conditional_expression
            { 
                $$ = $1;
                if (!$$->IsConst())
                {
                    semantic_error("Expression is not constant", yylineno);
                    YYERROR;
                }
            }

assignment_expression
        : conditional_expression
            { $$ = $1; }
        | lval '=' expr
            { 
                $$ = new cAssignExpr($1, $3); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | lval '=' asm_stmt
            { $$ = new cAssignExpr($1, $3); }
        | lval PLUS_EQ expr
            { 
                $$ = new cAssignExpr($1, new cBinaryExpr($1, '+', $3)); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | lval MINUS_EQ expr
            { 
                $$ = new cAssignExpr($1, new cBinaryExpr($1, '-', $3)); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | lval TIMES_EQ expr
            { 
                $$ = new cAssignExpr($1, new cBinaryExpr($1, '*', $3)); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | lval DIVIDE_EQ expr
            { 
                $$ = new cAssignExpr($1, new cBinaryExpr($1, '/', $3)); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | lval MOD_EQ expr
            { 
                $$ = new cAssignExpr($1, new cBinaryExpr($1, '%', $3)); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | lval OR_EQ expr
            { 
                $$ = new cAssignExpr($1, new cBinaryExpr($1, '|', $3)); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | lval AND_EQ expr
            { 
                $$ = new cAssignExpr($1, new cBinaryExpr($1, '&', $3)); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | lval XOR_EQ expr
            { 
                $$ = new cAssignExpr($1, new cBinaryExpr($1, '^', $3)); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | lval LEFT_EQ expr
            { 
                $$ = new cAssignExpr($1, new cBinaryExpr($1, LEFT, $3)); 
                if ($$->HasSemanticError()) YYERROR;
            }
        | lval RIGHT_EQ expr
            { 
                $$ = new cAssignExpr($1, new cBinaryExpr($1, RIGHT, $3)); 
                if ($$->HasSemanticError()) YYERROR;
            }
expr: assignment_expression
            { $$ = $1; }
        /*
        | expr ',' assignment_expression
            { 
                semantic_error("Not implemented " + std::to_string(__LINE__), yylineno); 
                YYERROR;
            }
	;
        */

%%

int yyerror(const char *msg)
{
    std::cerr << yycurrentfile << ":" << yylineno << ": error:"
        << msg << " at symbol " << yytext << "\n";

    return 0;
}
int semantic_error(std::string msg, int line)
{
    std::cerr << yycurrentfile << ":" << line << ": error: "
        << msg << "\n";
    
    yynerrs++;

    return 0;
}
void fatal_error(std::string msg)
{
    std::cerr << "Internal compiler error " << msg << std::endl;
    //std::cerr << symbolTableRoot->toString() << std::endl;
    exit(1);
}
