
%option yylineno
%{
#include "syntax.tab.h"
//#define MY_LEX_DEBUG
#ifdef MY_LEX_DEBUG
#define pf(x) printf("%s; [%s] ; [%d]\n", #x, yytext, yylineno)
#else
#define pf(x) 
#endif
void INIT_node_t(char *name, int mode);
int yycolumn = 1;
#define YY_USER_ACTION \
yylloc.first_line = yylloc.last_line = yylineno; \
yylloc.first_column = yycolumn; \
yylloc.last_column = yycolumn + yyleng - 1; \
yycolumn += yyleng;

#define MODE1 4
%}

letter [_a-zA-Z]
digit [0-9]

%%
"if" {pf(if); INIT_node_t("IF", MODE1); return IF;}
"else" {pf(else); INIT_node_t("ELSE", MODE1); return ELSE;}
"while" {pf(while); INIT_node_t("WHILE", MODE1); return WHILE;}
"struct" {pf(struct); INIT_node_t("STRUCT", MODE1); return STRUCT;}
"return" {pf(return); INIT_node_t("RETURN", MODE1); return RETURN; }
"int"|"float" {pf(type); INIT_node_t("TYPE", 16); return TYPE; }
{letter}+{digit}* { pf(id); INIT_node_t("ID", 16); return ID; }
"0"|([1-9]{digit}*) { pf(INT); INIT_node_t("INT", 8); return INT; }
0[0-7]+ {INIT_node_t("INT", 8); return INT;}
{digit}+\.{digit}+ { pf(FLOAT); INIT_node_t("FLOAT", 12); return FLOAT; } /*is 00.xx right??? */
({digit}*\.{digit}+|{digit}+\.)[eE][+-]?{digit}+ { pf(eFLOAT); INIT_node_t("FLOAT", 12); return FLOAT; }
([ ]|[\t]|[\r])+ {}
\n {yycolumn=1;}
("/*".*"*/")|("//".*[\n]) { pf(comment); }
"{" { pf(LC); INIT_node_t("LC", MODE1); return LC; }
"}" { pf(RC); INIT_node_t("RC", MODE1); return RC; }
"[" { pf(LB); INIT_node_t("LB", MODE1); return LB; }
"]" { pf(RB); INIT_node_t("RB", MODE1); return RB; }
"(" { pf(LP); INIT_node_t("LP", MODE1); return LP; }
")" { pf(RP); INIT_node_t("RP", MODE1); return RP; }
">"|"<"|">="|"<="|"=="|"!=" { pf(RELOP); INIT_node_t("RELOP", 0); return RELOP; }
"+" { pf(PLUS); INIT_node_t("PLUS", MODE1); return PLUS; }
"-" { pf(MINUS); INIT_node_t("MINUS", MODE1); return MINUS; }
"*" { pf(STAR); INIT_node_t("STAR", MODE1); return STAR; }
"/" { pf(DIV); INIT_node_t("DIV", MODE1); return DIV; }
"&&" { pf(AND); INIT_node_t("AND", MODE1); return AND; }
"||" { pf(OR); INIT_node_t("OR", MODE1); return OR; }
"." { pf(DOT); INIT_node_t("DOT", MODE1); return DOT; }
"!" { pf(NOT); INIT_node_t("NOT", MODE1); return NOT; }
"=" { pf(ASSIGNOP); INIT_node_t("ASSIGINOP", MODE1); return ASSIGNOP; }
"," { pf(COMMA); INIT_node_t("COMMA", MODE1); return COMMA; }
";" { pf(SEMI); INIT_node_t("SEMI", MODE1); return SEMI; }
. { printf("Error type A at Line %d: Mysterious character %c\n", yylineno, yytext[0]); } 

%%
void INIT_node_t(char *name, int mode){
	yylval.node_ptr = malloc(sizeof(node_t));
	yylval.node_ptr->child = yylval.node_ptr->bro = NULL;
	yylval.node_ptr->mode = mode;
	strcpy(yylval.node_ptr->name, name);
	switch(mode){
		case 0: break;
		case 1: break;
		case 4: yylval.node_ptr->iVal=atoi(yytext);
			break;
		case 8: yylval.node_ptr->fVal=atof(yytext);
			break;
		case 12: strcpy(yylval.node_ptr->cVal, yytext);
			break;
		default : break;
	}
}

