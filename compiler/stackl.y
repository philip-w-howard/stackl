/* lang.y */

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
    cBlockNode*         block;
    cDeclsNode*         decls;
    cDeclNode*          decl;
    cVarDeclNode*       varDecl;
    cStructDeclNode*    structDecl;
    cFuncDeclNode*      funcDecl;
    cParamsSpecNode*    paramsSpec;
    cArraySpecNode*     arraySpec;
    cFuncCallNode*      funcCall;
    cStmtsNode*         stmts;
    cStmtNode*          stmt;
    cVarRefNode*        varRef;
    cVarPartNode*       varPart;
    cArrayValNode*      arrayVal;
    cParamNode*         param;
    cExprNode*          expr;
    cStringLitNode*     string_lit;
    }

%{
    int yyerror(const char *msg);
    int semantic_error(std::string msg);

    cDeclsNode *yyast_root = NULL;
%}

%start  program

%token <symbol>    IDENTIFIER
%token <symbol>    TYPE_ID
%token <int_val>   INT_VAL
%token <string>    STRING_LIT
%token <symbol>    CHAR

%token  PRINT PRINTC
%token  GETS GETL GETI
%token  FOR WHILE IF ELSE 
%token  STRUCT
%token  RETURN
%token  JUNK_TOKEN
%token  EQ NE LT GT GE LE
%token  AND OR
%token  INC DEC
%token  PLUS_EQ MINUS_EQ TIMES_EQ DIVIDE_EQ
%token  TRAP RTI
%token  DEFINE CONST

%type <decls> program
%type <block> block
%type <sym_table> open
%type <sym_table> close
%type <decls> decls
%type <decl> decl
%type <varDecl> var_decl
%type <structDecl> struct_decl
%type <decls> global_decls
%type <decl> global_decl
%type <funcDecl> func_decl
%type <funcDecl> func_header
%type <funcDecl> func_prefix
%type <funcCall> func_call
%type <paramsSpec> paramsspec
%type <varDecl> paramspec
%type <arraySpec> arrayspec
%type <stmts> stmts
%type <stmt> stmt
%type <stmt> assign
%type <varRef> lval
%type <arrayVal> arrayval
%type <param> params
%type <expr> param
%type <expr> ccomp
%type <expr> comp
%type <expr> expr
%type <expr> term
%type <expr> value
%type <expr> fact
%type <varRef> varref
%type <varPart> varpart
%type <string_lit> string_lit
%token  <string> UNSUPPORTED

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

block:      open stmts close    { $$ = new cBlockNode(NULL, $2); }
open:   '{'                     { $$ = symbolTableRoot->IncreaseScope(); }

close:  '}'                     { $$ = symbolTableRoot->DecreaseScope(); }

decls:      decls decl          { $$ = $1;
                                  $$->AddNode($2);
                                }
        |   decl                { $$ = new cDeclsNode($1); }
decl:       var_decl ';'        { $$ = $1; }
        |   struct_decl ';'     { $$ = $1; }
        |   UNSUPPORTED         { semantic_error(std::string($1) + " is not supported");
                                  YYERROR;
                                }
        |   error ';'           { $$ = NULL; }

var_decl:   TYPE_ID IDENTIFIER 
                                { $$ = new cVarDeclNode($1, $2); }
        |   CHAR '*' IDENTIFIER { $$ = new cVarDeclNode(
                                        symbolTableRoot->Lookup("charp"),
                                        $3); }
        |   CHAR IDENTIFIER arrayspec
                                { 
                                    cSymbol *sym = new cSymbol("carray");
                                    cArrayDeclNode *arr = new cArrayDeclNode(
                                            $1, sym, $3);
                                    sym->SetType(arr);
                                    $$ = new cVarDeclNode(sym, $2);
                                }

struct_decl:  STRUCT open decls close IDENTIFIER    
                                { $$ = new cStructDeclNode($2, $3, $5); }
global_decls: global_decls global_decl  
                                {
                                    $$ = $1;
                                    $$->AddNode($2);
                                }
        | global_decl           { $$ = new cDeclsNode($1); }
global_decl: func_decl          { $$ = $1; }
        | struct_decl           { $$ = $1; }
        | CONST TYPE_ID IDENTIFIER '=' INT_VAL ';'
                                { $$ = new cConstDeclNode($3, $5); }
        | DEFINE IDENTIFIER INT_VAL
                                { $$ = new cConstDeclNode($2, $3); }
        | UNSUPPORTED           { 
                                    semantic_error(std::string(yytext) + 
                                            " is not supported");
                                    YYERROR;
                                }
func_decl:  func_header ';'
                          {
                            symbolTableRoot->DecreaseScope();
                            $$ = $1;
                          }
        |   func_header '{' stmts '}'
                          {
                            symbolTableRoot->DecreaseScope();
                            $$ = $1;
                            $1->SetBody($3);
                          }

func_header:  func_prefix paramsspec ')' 
                          {
                            $$ = $1;
                            $1->SetParams($2);
                          }
        |     func_prefix ')'
                          { $$ = $1;
                            $1->SetParams(NULL);
                          }
func_prefix: TYPE_ID IDENTIFIER '('
                          { $$ = new cFuncDeclNode($1, $2);
                            symbolTableRoot->IncreaseScope(); 
                          }
func_prefix: CHAR '*' IDENTIFIER '('
                          { $$ = new cFuncDeclNode(
                                        symbolTableRoot->Lookup("charp"),
                                        $3);
                            symbolTableRoot->IncreaseScope(); 
                          }
paramsspec:     
            paramsspec',' paramspec 
                                { $$ = $1;
                                  $1->AddNode($3); 
                                }
        |   paramspec           { $$ = new cParamsSpecNode($1); }

paramspec:  var_decl            { $$ = $1;}

 /*
arrayspec:  arrayspec '[' INT_VAL ']'
                            {
                                $$ = $1;
                                $1->AddNode($3);
                            }
 */
arrayspec:  '[' INT_VAL ']' { $$ = new cArraySpecNode($2); }

stmts:      stmts stmt          { $$ = $1; 
                                  $1->AddNode($2); 
                                }
        |   stmt                { $$ = new cStmtsNode($1); }

stmt:       decl                { $$ = $1; }
        |   IF '(' ccomp ')' stmt ELSE stmt
                                { $$ = new cIfNode($3, $5, $7); }
        |   IF '(' ccomp ')' stmt
                                { $$ = new cIfNode($3, $5, NULL); }
        |   FOR '(' assign ';' ccomp ';' assign ')' stmt
                                { $$ = new cForNode($3, $5, $7, $9); }
        |   WHILE '(' ccomp ')' stmt
                                { $$ = new cWhileNode($3, $5); }
        |   assign ';'          { $$ = $1; }
        |   func_call ';'       { $$ = new cFuncStmtNode($1); }
        |   block               { $$ = $1; }
        |   RETURN expr ';'     { $$ = new cReturnNode($2); }
        |   TRAP '(' ')' ';'    { $$ = new cTrapNode(); }
        |   RTI '(' ')' ';'     { $$ = new cRTINode(); }

assign:   lval '=' expr         { $$ = new cAssignNode($1, $3); }
        | lval '=' string_lit   { $$ = new cAssignNode($1, $3); }
        | lval '=' TRAP '(' ')' { $$ = new cAssignTrapNode($1); }
        | lval PLUS_EQ expr     { $$ = new cAssignNode($1, 
                                        new cBinaryExprNode($1, '+', $3));
                                }
        | lval MINUS_EQ expr    { $$ = new cAssignNode($1, 
                                        new cBinaryExprNode($1, '-', $3));
                                }
        | lval TIMES_EQ expr    { $$ = new cAssignNode($1, 
                                        new cBinaryExprNode($1, '*', $3));
                                }
        | lval DIVIDE_EQ expr   { $$ = new cAssignNode($1, 
                                        new cBinaryExprNode($1, '/', $3));
                                }
        | lval INC              { $$ = new cAssignNode($1,
                                        new cBinaryExprNode($1, '+',
                                            new cIntExprNode(1)));
                                }
        | INC lval              { $$ = new cAssignNode($2,
                                        new cBinaryExprNode($2, '+',
                                            new cIntExprNode(1)));
                                }
        | lval DEC              { $$ = new cAssignNode($1,
                                        new cBinaryExprNode($1, '-',
                                            new cIntExprNode(1)));
                                }
        | DEC lval              { $$ = new cAssignNode($2,
                                        new cBinaryExprNode($2, '-',
                                            new cIntExprNode(1)));
                                }
func_call:  IDENTIFIER '(' params ')' 
                                { $$ = new cFuncCallNode($1, $3); }

        |   IDENTIFIER '(' ')' 
                                { $$ = new cFuncCallNode($1, NULL); }
varref:   varref '.' varpart    { $$ = $1;
                                  $$->AddNode($3);
                                  if ($$->SemanticError()) YYERROR;
                                }
        | varpart               { $$ = new cVarRefNode($1); 
                                  if ($$->SemanticError()) YYERROR;
                                }

varpart:  IDENTIFIER arrayval   { $$ = new cVarPartNode($1, $2); }

lval:     varref                { $$ = $1; }
arrayval: arrayval '[' expr ']' 
                              { if ($1 == NULL)
                                  $$ = new cArrayValNode($3);
                                else 
                                {
                                  $$ = $1;
                                  $1->AddNode($3);
                                }
                              }
        |   /* empty */         { $$ = NULL; }

params:     params',' param
                                { $$ = $1;
                                  $1->AddNode($3);
                                }
        |   param               { $$ = new cParamNode($1); }

param:      expr                { $$ = $1; }
        |   string_lit          { $$ = $1; }
        |   '&' varref          { $$ = new cVarAddrNode($2); }

ccomp:      ccomp OR comp       { $$ = new cShortCircuitNode($1, OR, $3, true); }
        |   ccomp AND comp      { $$ = new cShortCircuitNode($1, AND, $3, false); }
        |   comp                { $$ = $1; }
comp:       comp EQ expr        { $$ = new cBinaryExprNode($1, EQ, $3); }
        |   comp NE expr        { $$ = new cBinaryExprNode($1, NE, $3); }
        |   comp GT expr        { $$ = new cBinaryExprNode($1, GT, $3); }
        |   comp LT expr        { $$ = new cBinaryExprNode($1, LT, $3); }
        |   comp LE expr        { $$ = new cBinaryExprNode($1, LE, $3); }
        |   comp GE expr        { $$ = new cBinaryExprNode($1, GE, $3); }
        |   expr                { $$ = $1; }
expr:       expr '+' term       { $$ = new cBinaryExprNode($1, '+', $3); }
        |   expr '-' term       { $$ = new cBinaryExprNode($1, '-', $3); }
        |   term                { $$ = $1; }

term:       term '*' value      { $$ = new cBinaryExprNode($1, '*', $3); }
        |   term '/' value      { $$ = new cBinaryExprNode($1, '/', $3); }
        |   term '%' value      { $$ = new cBinaryExprNode($1, '%', $3); }
        |   value               { $$ = $1; }

value:  fact                    { $$ = $1; }
        | '-' fact              { $$ = new cUnaryMinusNode($2); }

fact:        '(' ccomp ')'      { $$ = $2; }
        |   INT_VAL             { $$ = new cIntExprNode($1); }
        |   varref              { $$ = $1; }
        |   func_call           { $$ = $1; }

string_lit: STRING_LIT          { $$ = new cStringLitNode($1); }
%%

int yyerror(const char *msg)
{
    std::cerr << "ERROR: " << msg << " at symbol "
        << yytext << " on line " << yylineno << "\n";

    return 0;
}
int semantic_error(std::string msg)
{
    std::cerr << "ERROR: " << msg << " on line " << yylineno << "\n";
    
    yynerrs++;

    return 0;
}
