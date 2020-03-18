%{
 #include "syntax.tab.h"
 //#define MY_LEX_DEBUG

 #ifdef MY_LEX_DEBUG
 #define pf(x) printf("%s; [%s] ; [%d]\n", #x, yytext, yylineno)
 #else
 #define pf(x) 
 #endif
%}

%option yylineno
letter [_a-zA-Z]
digit [0-9]

%%
"if" {pf(if); return IF;}
"else" {pf(else); return ELSE;}
"while" {pf(while); return WHILE;}
"struct" {pf(struct); return STRUCT;}
"return" {pf(return); return RETURN; }
"int"|"float" {pf(type); return TYPE; }
{letter}+{digit}* { pf(id); return ID; }
"0"|([1-9]{digit}*) { pf(INT); return INT; }
{digit}+\.{digit}+ { pf(FLOAT); return FLOAT; }/* is 00.xx right???*/
({digit}*\.{digit}+|{digit}+\.)[eE][+-]?{digit}+ { pf(eFLOAT); return FLOAT; }

([ ]|[\n]|[\r]|[\t])+ {}
("/*".*"*/")|("//".*[\n]) { pf(comment); }
"{" { pf(LC); return LC; }
"}" { pf(RC); return RC; }
"[" { pf(LB); return LB; }
"]" { pf(RB); return RB; }
"(" { pf(LP); return LP; }
")" { pf(RP); return RP; }
">"|"<"|">="|"<="|"=="|"!=" { pf(RELOP); return RELOP; }
"+" { pf(PLUS); return PLUS; }
"-" { pf(MINUS); return MINUS; }
"*" { pf(STAR); return STAR; }
"/" { pf(DIV); return DIV; }
"&&" { pf(AND); return AND; }
"||" { pf(OR); return OR; }
"." { pf(DOT); return DOT; }
"!" { pf(NOT); return NOT; }
"=" { pf(ASSIGNOP); return ASSIGNOP; }
"," { pf(COMMA); return COMMA; }
";" { pf(SEMI); return SEMI; }
. { pf(ERROR); } 

%%
