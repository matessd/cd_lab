%{
#include "syntax.tab.h"
//#define MY_LEX_DEBUG
#ifdef MY_LEX_DEBUG
#define pf(x) printf("%s; [%s] ; [%d]\n", #x, yytext, yylineno)
#else
#define pf(x) 
#endif

void INIT_non_t(char *name);
%}

%option yylineno
letter [_a-zA-Z]
digit [0-9]

%%
"if" {pf(if); INIT_non_t("IF"); return IF;}
"else" {pf(else); INIT_non_t("ELSE"); return ELSE;}
"while" {pf(while); INIT_non_t("WHILE"); return WHILE;}
"struct" {pf(struct); INIT_non_t("STRUCT"); return STRUCT;}
"return" {pf(return); INIT_non_t("RETURN"); return RETURN; }
"int"|"float" {pf(type); INIT_non_t("TYPE"); return TYPE; }
{letter}+{digit}* { pf(id); INIT_non_t("TYPE"); return ID; }
"0"|([1-9]{digit}*) { pf(INT); INIT_non_t("TYPE"); return INT; }
{digit}+\.{digit}+ { pf(FLOAT); INIT_non_t("TYPE"); return FLOAT; }/* is 00.xx right???*/
({digit}*\.{digit}+|{digit}+\.)[eE][+-]?{digit}+ { pf(eFLOAT); INIT_non_t("TYPE"); return FLOAT; }

([ ]|[\n]|[\r]|[\t])+ {}
("/*".*"*/")|("//".*[\n]) { pf(comment); }
"{" { pf(LC); INIT_non_t("LC"); return LC; }
"}" { pf(RC); INIT_non_t("RC"); return RC; }
"[" { pf(LB); INIT_non_t("LB"); return LB; }
"]" { pf(RB); INIT_non_t("RB"); return RB; }
"(" { pf(LP); INIT_non_t("LP"); return LP; }
")" { pf(RP); INIT_non_t("RP"); return RP; }
">"|"<"|">="|"<="|"=="|"!=" { pf(RELOP); INIT_non_t("RELOP"); return RELOP; }
"+" { pf(PLUS); INIT_non_t("PLUS"); return PLUS; }
"-" { pf(MINUS); INIT_non_t("MINUS"); return MINUS; }
"*" { pf(STAR); INIT_non_t("STAR"); return STAR; }
"/" { pf(DIV); INIT_non_t("DIV"); return DIV; }
"&&" { pf(AND); INIT_non_t("AND"); return AND; }
"||" { pf(OR); INIT_non_t("OR"); return OR; }
"." { pf(DOT); INIT_non_t("DOT"); return DOT; }
"!" { pf(NOT); INIT_non_t("NOT"); return NOT; }
"=" { pf(ASSIGNOP); INIT_non_t("ASSIGINOP"); return ASSIGNOP; }
"," { pf(COMMA); INIT_non_t("COMMA"); return COMMA; }
";" { pf(SEMI); INIT_non_t("SEMI"); return SEMI; }
. { pf(ERROR); } 

%%
void INIT_non_t(char *name){
	printf("word:[%s]\n",name);
	yylval.non_t.child = yylval.non_t.bro = NULL;
	strcpy(yylval.non_t.name, name);
}

