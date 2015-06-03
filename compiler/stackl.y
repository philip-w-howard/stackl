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
    cAsmNode*           asmstmt;
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
%token  TYPEDEF STRUCT
%token  RETURN
%token  JUNK_TOKEN
%token  EQ NE GE LE
%token  AND OR
%token  SIZE_OF
%token  INC DEC
%token  PLUS_EQ MINUS_EQ TIMES_EQ DIVIDE_EQ AND_EQ OR_EQ XOR_EQ 
%token  LEFT_EQ RIGHT_EQ
%token  PTR LEFT RIGHT
%token  ASM ASM2
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
%type <asmstmt> asm_stmt
%type <varRef> lval
%type <arrayVal> arrayval
%type <param> params
%type <expr> param
%type <expr> primary_expression
%type <expr> deref_expression
%type <expr> postfix_expression
%type <expr> unary_expression
%type <expr> cast_expression
%type <expr> multiplicative_expression
%type <expr> additive_expression
%type <expr> shift_expression
%type <expr> relational_expression
%type <expr> equality_expression
%type <expr> and_expression
%type <expr> exclusive_or_expression
%type <expr> inclusive_or_expression
%type <expr> logical_and_expression
%type <expr> logical_or_expression
%type <expr> expr
%type <varRef> varref
%type <varPart> varpart
%type <string_lit> string_lit
%type <symbol> type
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
        | '{' '}'               { $$ = new cBlockNode(NULL, NULL); }
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

var_decl:   type IDENTIFIER 
                                { $$ = new cVarDeclNode($1, $2); }
        |   type IDENTIFIER arrayspec
                                { 
                                    cSymbol *sym = new cSymbol("carray");
                                    cArrayDeclNode *arr = new cArrayDeclNode(
                                            $1, sym, $3);
                                    sym->SetDecl(arr);
                                    $$ = new cVarDeclNode(sym, $2);
                                }
type:   type '*' {
                        std::string new_type = $1->Name();
                        new_type += "*";
                        cSymbol* sym = new cSymbol(new_type);
                        sym->SetDecl(new cBaseDeclNode(sym, 4, true, false, false, $1->GetType()));
                        symbolTableRoot->InsertRoot(sym);
                        $$ = sym;
               }
      | TYPE_ID { $$ = $1; }  
struct_decl:  TYPEDEF STRUCT open decls close IDENTIFIER    
                                { $$ = new cStructDeclNode($3, $4, $6); }
global_decls: global_decls global_decl  
                                {
                                    $$ = $1;
                                    $$->AddNode($2);
                                }
        | global_decl           { $$ = new cDeclsNode($1); }
global_decl: func_decl          { $$ = $1; }
        | struct_decl           { $$ = $1; }
        | CONST type IDENTIFIER '=' INT_VAL ';'
                                { $$ = new cConstDeclNode($3, $5); }
        | DEFINE IDENTIFIER INT_VAL
                                { $$ = new cConstDeclNode($2, $3); }
        | DEFINE IDENTIFIER '-' INT_VAL
                                { $$ = new cConstDeclNode($2, -$4); }
        | type IDENTIFIER 
                                { $$ = new cVarDeclNode($1, $2, true); }
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

        |   func_header '{' '}'
                          {
                            symbolTableRoot->DecreaseScope();
                            $$ = $1;
                            $1->SetBody(NULL);
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
func_prefix: type IDENTIFIER '('
                          { $$ = new cFuncDeclNode($1, $2);
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
        |   ';'                 { $$ = NULL; }
        |   IF '(' expr ')' stmt ELSE stmt
                                { $$ = new cIfNode($3, $5, $7); }
        |   IF '(' expr ')' stmt
                                { $$ = new cIfNode($3, $5, NULL); }
        |   FOR '(' assign ';' expr ';' assign ')' stmt
                                { $$ = new cForNode($3, $5, $7, $9); }
        |   WHILE '(' expr ')' stmt
                                { $$ = new cWhileNode($3, $5); }
        |   assign ';'          { $$ = $1; }
        |   func_call ';'       { $$ = new cFuncStmtNode($1); }
        |   block               { $$ = $1; }
        |   RETURN expr ';'     { $$ = new cReturnNode($2); }
        |   asm_stmt ';'        { $$ = $1; }

assign:   lval '=' expr         { $$ = new cAssignNode($1, $3); }
        | lval '=' string_lit   { $$ = new cAssignNode($1, $3); }
        | lval '=' asm_stmt
                                { $$ = new cAssignAsmNode($1, $3); }
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
        | lval OR_EQ expr       { $$ = new cAssignNode($1, 
                                        new cBinaryExprNode($1, '|', $3));
                                }
        | lval AND_EQ expr      { $$ = new cAssignNode($1, 
                                        new cBinaryExprNode($1, '&', $3));
                                }
        | lval XOR_EQ expr      { $$ = new cAssignNode($1, 
                                        new cBinaryExprNode($1, '^', $3));
                                }
        | lval LEFT_EQ expr     { $$ = new cAssignNode($1, 
                                        new cBinaryExprNode($1, LEFT, $3));
                                }
        | lval RIGHT_EQ expr    { $$ = new cAssignNode($1, 
                                        new cBinaryExprNode($1, RIGHT, $3));
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
asm_stmt : ASM '(' INT_VAL ')' 
                                { $$ = new cAsmNode($3, NULL); }
        | ASM '(' INT_VAL ',' params ')'
                                { $$ = new cAsmNode($3, $5); }
        | ASM2 '(' INT_VAL ',' INT_VAL ')' 
                                { $$ = new cAsmNode($3, $5, NULL); }
        | ASM2 '(' INT_VAL  ',' INT_VAL ',' params ')'
                                { $$ = new cAsmNode($3, $5, $7); }
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
/* Need to add PTR */

varpart:  IDENTIFIER arrayval   { $$ = new cVarPartNode($1, $2); }

lval:     varref                { $$ = $1; }
        |   '*' varref          { $$ = new cVarDerefNode($2); } 
/*        | var_decl              { $$ = new cVarRefNode(new cVarPartNode($1->GetVar(), NULL)); } */
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


primary_expression
        :   INT_VAL             { $$ = new cIntExprNode($1); }
        |   varref              { $$ = $1; }

        /* grammar calls for these. For now with the defs above
	: IDENTIFIER
	| CONSTANT              { $$ = new cIntExprNode($1); }
        */

        /* handled separately for now
	| STRING_LITERAL        { $$ = new cStringLitNode($1); }
        */
	| '(' expr ')'    { $$ = $2; }

        /* Only allow specific SIZEOFs
	| SIZEOF deref_expression
	| SIZEOF '(' deref_expression ')'
	| SIZEOF '(' type_name ')'
        */
        |   SIZE_OF '(' IDENTIFIER ')' { $$ = new cSizeOfNode($3); }
        |   SIZE_OF '(' type ')' { $$ = new cSizeOfNode($3); }
        |   SIZE_OF IDENTIFIER { $$ = new cSizeOfNode($2); }
	;

deref_expression
	: primary_expression
            { $$ = $1; }
        /* Not implemented yet
	| deref_expression '[' expression ']'
	| deref_expression '.' IDENTIFIER
	| deref_expression PTR_OP IDENTIFIER
        */

        /* should be deref_expr, but we only allow varref for now
	| '&' deref_expression
	| '*' deref_expression
        */
        | '&' varref            { $$ = new cVarAddrNode($2); }
        | '*' varref            { $$ = new cVarDerefNode($2); }

postfix_expression
	: deref_expression
            { $$ = $1; }
        /* INC/DEC handled in assign for now
	| postfix_expression INC
	| postfix_expression DEC
        */
	;

unary_expression
	: postfix_expression
            { $$ = $1; }
        /* INC/DEC handled in assign for now
	| INC unary_expression
	| DEC unary_expression
        */
        | '+' cast_expression   { $$ = $2; }
        | '-' cast_expression   { $$ = new cUnaryOpNode($2, '-'); }
        | '~' cast_expression   { $$ = new cUnaryOpNode($2, '~'); }
        | '!' cast_expression   { $$ = new cUnaryOpNode($2, '!'); }
	;

cast_expression
	: unary_expression
            { $$ = $1; }
	| func_call             { $$ = $1; }
	| '(' TYPE_ID ')' cast_expression      
                { semantic_error("Type casts not implemented"); }
	;

multiplicative_expression
	: cast_expression
            { $$ = $1; }
	| multiplicative_expression '*' cast_expression
            { $$ = new cBinaryExprNode($1, '*', $3); }
	| multiplicative_expression '/' cast_expression
            { $$ = new cBinaryExprNode($1, '/', $3); }
	| multiplicative_expression '%' cast_expression
            { $$ = new cBinaryExprNode($1, '%', $3); }
	;

additive_expression
	: multiplicative_expression
            { $$ = $1; }
	| additive_expression '+' multiplicative_expression
            { $$ = new cBinaryExprNode($1, '+', $3); }
	| additive_expression '-' multiplicative_expression
            { $$ = new cBinaryExprNode($1, '-', $3); }
	;

shift_expression
	: additive_expression
            { $$ = $1; }
	| shift_expression LEFT additive_expression
            { $$ = new cBinaryExprNode($1, LEFT, $3); }
	| shift_expression RIGHT additive_expression
            { $$ = new cBinaryExprNode($1, RIGHT, $3); }
	;

relational_expression
	: shift_expression
            { $$ = $1; }
	| relational_expression '<' shift_expression
            { $$ = new cBinaryExprNode($1, '<', $3); }
	| relational_expression '>' shift_expression
            { $$ = new cBinaryExprNode($1, '>', $3); }
	| relational_expression LE shift_expression
            { $$ = new cBinaryExprNode($1, LE, $3); }
	| relational_expression GE shift_expression
            { $$ = new cBinaryExprNode($1, GE, $3); }
	;

equality_expression
	: relational_expression
            { $$ = $1; }
	| equality_expression EQ relational_expression
            { $$ = new cBinaryExprNode($1, EQ, $3); }
	| equality_expression NE relational_expression
            { $$ = new cBinaryExprNode($1, NE, $3); }
	;

and_expression
	: equality_expression
            { $$ = $1; }
	| and_expression '&' equality_expression
            { $$ = new cBinaryExprNode($1, '&', $3); }
	;

exclusive_or_expression
	: and_expression
            { $$ = $1; }
	| exclusive_or_expression '^' and_expression
            { $$ = new cBinaryExprNode($1, '^', $3); }
	;

inclusive_or_expression
	: exclusive_or_expression
            { $$ = $1; }
	| inclusive_or_expression '|' exclusive_or_expression
            { $$ = new cBinaryExprNode($1, '|', $3); }
	;

logical_and_expression
	: inclusive_or_expression
            { $$ = $1; }
	| logical_and_expression AND inclusive_or_expression
            { $$ = new cShortCircuitNode($1, AND, $3, false); }
	;

logical_or_expression
	: logical_and_expression
            { $$ = $1; }
	| logical_or_expression OR logical_and_expression
            { $$ = new cShortCircuitNode($1, OR, $3, true); }

	;

expr
	: logical_or_expression
	;

string_lit: STRING_LIT          { $$ = new cStringLitNode($1); }

%%

int yyerror(const char *msg)
{
    std::cerr << "ERROR: " << msg << " at symbol "
        << yytext << " on line " << yylineno << " of " << yycurrentfile << "\n";

    return 0;
}
int semantic_error(std::string msg)
{
    std::cerr << "ERROR: " << msg << " on line " << yylineno 
        << " of " << yycurrentfile << "\n";
    
    yynerrs++;

    return 0;
}
