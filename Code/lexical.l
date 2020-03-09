%{
#define MY_DEBUG

#ifdef MY_DEBUG
#define pf(x) printf("%s:%s||%d\n", #x, yytext, yylineno)
#else
#define pf(x) 
#endif
%}

%option yylineno
letter [_a-zA-Z]
digit [0-9]

%%
"if" {pf(if);}
"else" {pf(else);}
"while" {pf(while);}
"struct" {pf(struct);}
"return" {pf(return);}
"int"|"float" {pf(type);}
{letter}+{digit}* { pf(id); }
"0"|([1-9]{digit}*) { pf(INT); }
{digit}+\.{digit}+ { pf(FLOAT); }/* is 00.xx right???*/
({digit}*\.{digit}+|{digit}+\.)[eE][+-]?{digit}+ { pf(eFLOAT); }

([ ]|[\n]|[\r]|[\t])+ {}
"/*".*"*/" { pf(comment); }
"{" { pf(LC); }
"}" { pf(RC); }
"[" { pf(LB); }
"]" { pf(RB); }
"(" { pf(LP); }
")" { pf(RP); }
">"|"<"|">="|"<="|"=="|"!=" { pf(RELOP); }
"+" { pf(PLUS); }
"-" { pf(MINUS); }
"*" { pf(MUL); }
"/" { pf(DIV); }
"&&" { pf(AND); }
"||" { pf(OR); }
"." { pf(DOT); }
"!" { pf(NOT); }
"=" { pf(ASSIGINOP); }
"," { pf(COMMA); }
";" { pf(SEMI); }
. { pf(ERROR); } 

%%
