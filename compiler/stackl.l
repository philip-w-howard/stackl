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


period          ['.']
whitespace      [' ''\t''\n']
letter          [A-Za-z]
digit           [0-9]
punctuation     [(){}\[\];,\.]
operator        [+\-*/%=]
integer         -?0|(-?[1-9][0-9]*)
float           {integer}|({integer}\.{digit}+)
xfloat           (0{period}0)|(0{period}{digit}*[1-9])|([1-9]{digit}*{period}{digit}*[1-9])
identifier      {letter}({letter}|{digit}|"_")*
%%

"//"(.)+$                   {}
{whitespace}+               {}
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
"if"                        RETURN(IF);
"else"                      RETURN(ELSE);
"while"                     RETURN(WHILE);
"print"                     RETURN(PRINT);
"scan"                      RETURN(SCAN);
"int"                       { yylval.symbol = symbolTableRoot->Lookup("int");
                              RETURN(TYPE_ID);
                            }
"float"                     { yylval.symbol = symbolTableRoot->Lookup("float");
                              RETURN(TYPE_ID);
                            }
"char"                      { yylval.symbol = symbolTableRoot->Lookup("char");
                              RETURN(TYPE_ID);
                            }
"struct"                    RETURN(STRUCT);
"array"                     RETURN(ARRAY);
"return"                    RETURN(RETURN);
{integer}                   yylval.int_val = atoi(yytext); RETURN(INT_VAL);
{integer}|({integer}\.{digit}+) yylval.float_val = atof(yytext); RETURN(FLOAT_VAL);
{identifier}                RETURN(proc_identifier(yytext));
.                           RETURN(JUNK_TOKEN);   // discard anything not recognized

%%

static int Return(int val)
{
    //printf("Scanned '%s': %d\n", yytext, val);
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
