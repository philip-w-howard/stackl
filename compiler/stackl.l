%{
// STACKL Scanner
// Phil Howard

#include <limits.h>     // for include path
#include <list>
#include <set>
#include "lex.h"

class File_Def
{
  public:
    char mName[PATH_MAX];
    int  mPrevLine;
};

static std::list<File_Def> FileList ;
static std::set<std::string> OnceList ;
char yycurrentfile[PATH_MAX];
extern char Include_Path[PATH_MAX];

#define Return(a) return do_return(a);
int process_once();
static int process_eof();
static int process_string_lit();
static int process_simple_esc(int c);
static int do_return(int val);
static int proc_identifier(const char *name);
static void open_input(char *name);
static int Main_lineno = 0;
static int File_Depth = 0;

extern int Do_Debug;
%}

%option noyywrap
%option nounput
%option yylineno

/* the "INCLUDE" state is used for picking up the name
   * of an include file
   */
%x INCLUDE
%x C_COMMENT
%x DO_PRAGMA

apost           [']
quote           [\"]
not_quote       [^\"]
period          ['.']
whitespace      [ \t\n\r]
letter          [A-Za-z]
digit           [0-9]
punctuation     [(){}\[\];,\.?:]
operator        [+\-*/%=&|~!^]
integer         0|([1-9][0-9]*)
hexint          0(x|X)([0-9a-fA-F])+
identifier      {letter}({letter}|{digit}|"_")*
%%

"//"(.)*$                   {}
{whitespace}+               {}

"/*"                BEGIN(C_COMMENT);
<C_COMMENT>"*/"     BEGIN(INITIAL);
<C_COMMENT>[ \t\n\r]+       {}
<C_COMMENT>.                {}

"#include"          BEGIN(INCLUDE);
<INCLUDE>[ \t]*      /* eat the whitespace */
<INCLUDE>[^ \t\r\n]+   { /* got the include file name */
                    open_input(yytext);
                    BEGIN(INITIAL);
                  }

"#pragma"            BEGIN(DO_PRAGMA); Return(PRAGMA);
<DO_PRAGMA>"once"       Return(ONCE);
<DO_PRAGMA>"interrupt"  BEGIN(INITIAL); Return(INTERRUPT);
<DO_PRAGMA>"systrap"    BEGIN(INITIAL); Return(SYSTRAP);
<DO_PRAGMA>"startup"    BEGIN(INITIAL); Return(STARTUP);
<DO_PRAGMA>"feature"    BEGIN(INITIAL); Return(FEATURE);
<DO_PRAGMA>"library"    BEGIN(INITIAL); Return(LIBRARY);
<DO_PRAGMA>"stack\_size" BEGIN(INITIAL); Return(STACK_SIZE);
<DO_PRAGMA>[ \t\r]+       {}
<DO_PRAGMA>"\n"         BEGIN(INITIAL);
<DO_PRAGMA>.            Return(JUNK_TOKEN);   // discard anything not recognized

<<EOF>>           { if (process_eof()) yyterminate(); }

{quote}{not_quote}*{quote}  { return process_string_lit(); }
{punctuation}               Return(yytext[0]);
{operator}                  Return(yytext[0]);
"&&"                        Return(AND);
"||"                        Return(OR);
"=="                        Return(EQ);
"!="                        Return(NE);
">="                        Return(GE);
"<="                        Return(LE);
"<"                         Return('<');
">"                         Return('>');
"++"                        Return(INC);
"--"                        Return(DEC);
"+="                        Return(PLUS_EQ);
"-="                        Return(MINUS_EQ);
"*="                        Return(TIMES_EQ);
"/="                        Return(DIVIDE_EQ);
"%="                        Return(MOD_EQ);
"|="                        Return(OR_EQ);
"&="                        Return(AND_EQ);
"^="                        Return(XOR_EQ);
"<<="                       Return(LEFT_EQ);
">>="                       Return(RIGHT_EQ);
"->"                        Return(PTR);
"<<"                        Return(LEFT);
">>"                        Return(RIGHT);
"asm"                       Return(ASM);
"asm2"                      Return(ASM2);
"extern"                    Return(EXTERN);
"static"                    Return(STATIC);
"const"                     Return(CONST);
"#define"                   Return(DEFINE);
"volatile"                  Return(VOLATILE);
"break"                     Return(BREAK);
"case"                      Return(CASE);
"if"                        Return(IF);
"else"                      Return(ELSE);
"for"                       Return(FOR);
"while"                     Return(WHILE);
"do"                        Return(DO);
"switch"                    Return(SWITCH);
"goto"                      Return(GOTO);
"int"                       { yylval.symbol = symbolTableRoot->Lookup("int");
                              Return(TYPE_ID);
                            }
"char"                      { yylval.symbol = symbolTableRoot->Lookup("char");
                              Return(TYPE_ID);
                            }
"void"                      { yylval.symbol = symbolTableRoot->Lookup("void");
                              Return(TYPE_ID);
                            }
"sizeof"                    Return(SIZEOF);
"typedef"                   Return(TYPEDEF);
"struct"                    Return(STRUCT);
"return"                    Return(RETURN);
{integer}                   yylval.int_val = atoi(yytext); Return(INT_VAL);
{hexint}                    { yylval.int_val = (int)strtol(yytext, NULL, 0);
                                Return(INT_VAL);
                            }

"'\\"."'"                   Return(process_simple_esc(yytext[2]));
"'"."'"                     yylval.int_val = yytext[1]; Return(INT_VAL);
{identifier}                Return(proc_identifier(yytext));
.                           Return(JUNK_TOKEN);   // discard anything not recognized

%%

// process identifiers:
// determine if it is a type, var, or new
static int proc_identifier(const char *name)
{
    yylval.symbol = symbolTableRoot->Lookup(name);
    if (yylval.symbol == nullptr)
    {
        yylval.symbol = new cSymbol(name);
    } else {
        cDecl *decl = yylval.symbol->GetDecl();
        if (decl == nullptr) return IDENTIFIER;
        if (decl->IsType()) return TYPE_ID;

        // FIX THIS: var refs should be returned, but
        // Is_Const for the varref should be detectable
        /*
        if (decl->IsVar() && decl->GetVar()->IsConst())
        {
            yylval.int_val = decl->GetVar()->GetInit()->ConstValue();
            return INT_VAL;
        }
        */
    }

    return IDENTIFIER;
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
        printf("Error opening %s\n", path);
        exit(1);
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
static int do_return(int val)
{
    //if (Do_Debug) fprintf(stderr, "Scanned '%s': %d\n", yytext, val);
    return val;
}

// (6.4.4.4) simple-escape-sequence:
//      \’ \" \? \\ \a
//      \b \f \n \r \t \v
static int process_simple_esc(int c)
{
    switch (c)
    {
    case '\'':
        yylval.int_val = '\'';
        break;
    case '\"':
        yylval.int_val = '\"';
        break;
    case '\?':
        yylval.int_val = '\?';
        break;
    case '\\':
        yylval.int_val = '\\';
        break;
    case 'a':
        yylval.int_val = '\a';
        break;
    case 'b':
        yylval.int_val = '\b';
        break;
    case 'f':
        yylval.int_val = '\f';
        break;
    case 'n':
        yylval.int_val = '\n';
        break;
    case 'r':
        yylval.int_val = '\r';
        break;
    case 't':
        yylval.int_val = '\t';
        break;
    case 'v':
        yylval.int_val = '\v';
        break;
    default:
        return JUNK_TOKEN;
    }
    return INT_VAL;
}
static int process_string_lit()
{
    // need to trim the quotes off
    yytext[strlen(yytext)-1] = 0;
    yylval.string = &yytext[1];
    return STRING_LIT;
}
int process_once()
{
    std::string file(yycurrentfile);
    if (OnceList.find(file) != OnceList.end())
    {
        return process_eof();
    }
    else
    {
        OnceList.insert(file);
    }
    return 0;
}

static int process_eof()
{
    File_Def file_item;
    if (FileList.empty())
    {
        return 1;
    }
    else
    {
        file_item = FileList.back();
        FileList.pop_back();
        yylineno = file_item.mPrevLine;
        strcpy(yycurrentfile, file_item.mName);
        yypop_buffer_state();
    }

    return 0;
}
