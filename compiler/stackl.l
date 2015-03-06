%{
// Calc Parser
// Phil Howard

#include "lex.h"

#define RETURN(a) return Return(a);
static int Return(int val);
static int proc_identifier(const char *name);

%}

%option noyywrap
%option nounput
%option yylineno


apost           [']
quote           [\"]
not_quote       [^\"]
period          ['.']
whitespace      [ \t\n]
letter          [A-Za-z]
digit           [0-9]
punctuation     [(){}\[\];,\.]
operator        [+\-*/%=]
integer         0|([1-9][0-9]*)
identifier      {letter}({letter}|{digit}|"_")*
%%

"//"(.)*$                   {}
{whitespace}+               {}
{quote}{not_quote}*{quote}  {
                                yylval.string = yytext;
                                RETURN(STRING_LIT);
                            }
{punctuation}               RETURN(yytext[0]);
{operator}                  RETURN(yytext[0]);
"&&"                        RETURN(AND);
"||"                        RETURN(OR);
"=="                        RETURN(EQ);
"!="                        RETURN(NE);
">="                        RETURN(GE);
"<="                        RETURN(LE);
"<"                         RETURN(LT);
">"                         RETURN(GT);
"++"                        RETURN(INC);
"--"                        RETURN(DEC);
"+="                        RETURN(PLUS_EQ);
"-="                        RETURN(MINUS_EQ);
"*="                        RETURN(TIMES_EQ);
"/="                        RETURN(DIVIDE_EQ);
"static"                    { yylval.string = yytext; RETURN(UNSUPPORTED); }
"const"                     { yylval.string = yytext; RETURN(UNSUPPORTED); }
"volatile"                  { yylval.string = yytext; RETURN(UNSUPPORTED); }
"break"                     { yylval.string = yytext; RETURN(UNSUPPORTED); }
"switch"                    { yylval.string = yytext; RETURN(UNSUPPORTED); }
"case"                      { yylval.string = yytext; RETURN(UNSUPPORTED); }
"if"                        RETURN(IF);
"else"                      RETURN(ELSE);
"for"                       RETURN(FOR);
"while"                     RETURN(WHILE);
"print"                     RETURN(PRINT);
"printc"                    RETURN(PRINTC);
"scan"                      RETURN(SCAN);
"int"                       { yylval.symbol = symbolTableRoot->Lookup("int");
                              RETURN(TYPE_ID);
                            }
"char"                      { yylval.symbol = symbolTableRoot->Lookup("char");
                              RETURN(CHAR);
                            }
"struct"                    RETURN(STRUCT);
"array"                     RETURN(ARRAY);
"return"                    RETURN(RETURN);
{integer}                   yylval.int_val = atoi(yytext); RETURN(INT_VAL);
"'"."'"                       yylval.int_val = yytext[1]; RETURN(INT_VAL); 
"'\\n'"                      yylval.int_val = '\n'; RETURN(INT_VAL);
 /* 
L?'(\\.|[^\\'])+'           yylval.int_val = yytext[1]; RETURN(INT_VAL);
 */
{identifier}                RETURN(proc_identifier(yytext));
.                           RETURN(JUNK_TOKEN);   // discard anything not recognized

%%

static int Return(int val)
{
//    printf("Scanned '%s': %d\n", yytext, val);
    return val;
}

// process identifiers: 
// determine if it is a type, var, or new
static int proc_identifier(const char *name)
{
    yylval.symbol = symbolTableRoot->Lookup(name);
    if (yylval.symbol == NULL) yylval.symbol = new cSymbol(name);
    if (yylval.symbol->GetType() == NULL) return IDENTIFIER;
    if (yylval.symbol->GetType()->IsType()) RETURN(TYPE_ID);

    RETURN(IDENTIFIER);
}
