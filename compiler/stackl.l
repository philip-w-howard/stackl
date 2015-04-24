%{
// Calc Parser
// Phil Howard

#include <limits.h>     // for include path
#include <list>
#include "lex.h"

class File_Def
{
  public:
    char mName[PATH_MAX];
    int  mPrevLine;
};

static std::list<File_Def> FileList ;
char yycurrentfile[PATH_MAX];
extern char Include_Path[PATH_MAX];

#define RETURN(a) return Return(a);
static int Return(int val);
static int proc_identifier(const char *name);
static int proc_unsupported();
static void open_input(char *name);
static int Main_lineno = 0;
static int File_Depth = 0;
%}

%option noyywrap
%option nounput
%option yylineno

/* the "incl" state is used for picking up the name
   * of an include file
   */
%x incl
%x c_comment

apost           [']
quote           [\"]
not_quote       [^\"]
period          ['.']
whitespace      [ \t\n\r]
letter          [A-Za-z]
digit           [0-9]
punctuation     [(){}\[\];,\.]
operator        [+\-*/%=&]
integer         0|([1-9][0-9]*)
hexint          0x([0-9a-fA-F])+
identifier      {letter}({letter}|{digit}|"_")*
%%

"//"(.)*$                   {}
{whitespace}+               {}

"/*"                BEGIN(c_comment);
<c_comment>"*/"     BEGIN(INITIAL);
<c_comment>.        {}

"#include"          BEGIN(incl);
<incl>[ \t]*      /* eat the whitespace */
<incl>[^ \t\n]+   { /* got the include file name */
                    open_input(yytext);
                    BEGIN(INITIAL);
                  }

<<EOF>>           {
                    File_Def file_item;
                    if (FileList.empty())
                        yyterminate();
                    else
                    {
                        file_item = FileList.back();
                        FileList.pop_back();
                        yylineno = file_item.mPrevLine;
                        strcpy(yycurrentfile, file_item.mName);
                        yypop_buffer_state();
                    }
                    /*
                    if (--File_Depth == 0) yylineno = Main_lineno;

                    //if ( !YY_CURRENT_BUFFER )
                    if (File_Depth < 0)
                    {
                        File_Depth = 0;
                        yyterminate();
                    } else {
                        yypop_buffer_state();
                    }
                    */
                  }

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
"asm"                       RETURN(ASM);
"static"                    { RETURN(proc_unsupported()); }
"const"                     { RETURN(CONST); }
"#define"                   { RETURN(DEFINE); }
"volatile"                  { RETURN(proc_unsupported()); }
"break"                     { RETURN(proc_unsupported()); }
"switch"                    { RETURN(proc_unsupported()); }
"case"                      { RETURN(proc_unsupported()); }
"void"                      { RETURN(proc_unsupported()); }
"if"                        RETURN(IF);
"else"                      RETURN(ELSE);
"for"                       RETURN(FOR);
"while"                     RETURN(WHILE);
  /*
"print"                     RETURN(PRINT);
"printc"                    RETURN(PRINTC);
"gets"                      RETURN(GETS);
"getl"                      RETURN(GETL);
"geti"                      RETURN(GETI);
  */
"int"                       { yylval.symbol = symbolTableRoot->Lookup("int");
                              RETURN(TYPE_ID);
                            }
"char"                      { yylval.symbol = symbolTableRoot->Lookup("char");
                              RETURN(CHAR);
                            }
"sizeof"                    RETURN(SIZE_OF);
"typedef"                   RETURN(TYPEDEF);
"struct"                    RETURN(STRUCT);
"return"                    RETURN(RETURN);
{integer}                   yylval.int_val = atoi(yytext); RETURN(INT_VAL);
{hexint}                    { yylval.int_val = (int)strtol(yytext, NULL, 0); 
                                RETURN(INT_VAL);
                            }
                                
"'"."'"                      yylval.int_val = yytext[1]; RETURN(INT_VAL); 
"'\\n'"                      yylval.int_val = '\n'; RETURN(INT_VAL);
 /* 
L?'(\\.|[^\\'])+'           yylval.int_val = yytext[1]; RETURN(INT_VAL);
 */
{identifier}                RETURN(proc_identifier(yytext));
  /* doesn't work
<<EOF>>                     { strcpy(yytext, "End of File"); RETURN(0); }
  */
.                           RETURN(JUNK_TOKEN);   // discard anything not recognized

%%

// process identifiers: 
// determine if it is a type, var, or new
static int proc_identifier(const char *name)
{
    yylval.symbol = symbolTableRoot->Lookup(name);
    if (yylval.symbol == NULL) yylval.symbol = new cSymbol(name);
    if (yylval.symbol->GetType() == NULL) return IDENTIFIER;
    if (yylval.symbol->GetType()->IsType()) return TYPE_ID;
    if (yylval.symbol->GetType()->IsConst()) 
    {
        yylval.int_val = yylval.symbol->GetType()->GetValue();
        return INT_VAL;
    }

    return IDENTIFIER;
}
static int proc_unsupported()
{
    yylval.string = yytext;
    return UNSUPPORTED;
}
static void open_input(char *name)
{
    char *ptr;
    char path[PATH_MAX] = "";

    if (*name == '"')
    {
        ptr = strrchr(name, '"');
        if (ptr != NULL) *ptr = 0;
        name++;
    }
    else if (*name == '<')
    {
        ptr = strrchr(name, '>');
        if (ptr != NULL) *ptr = 0;
        name++;
        strcpy(path, Include_Path);
    }

    strcat(path, name);
    yyin = fopen( path, "r" );

    if ( ! yyin )
    {
        printf("Error opening %s\n", yytext);
    } else {
        File_Def fileDef;
        strcpy(fileDef.mName, yycurrentfile);
        fileDef.mPrevLine = yylineno;
        FileList.push_back(fileDef);
        strcpy(yycurrentfile, path);
        yylineno = 1;

        if (File_Depth++ == 0) Main_lineno = yylineno;
        yypush_buffer_state(yy_create_buffer( yyin, YY_BUF_SIZE ));
    }
}
static int Return(int val)
{
    //printf("Scanned '%s': %d\n", yytext, val);
    return val;
}

