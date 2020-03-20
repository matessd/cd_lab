
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
extern int error_flg;
%}

%x COMMENT
letter [_a-zA-Z]
digit [0-9]

%%
"if" {pf(if); INIT_node_t("IF", MODE1); return IF;}
"else" {pf(else); INIT_node_t("ELSE", MODE1); return ELSE;}
"while" {pf(while); INIT_node_t("WHILE", MODE1); return WHILE;}
"struct" {pf(struct); INIT_node_t("STRUCT", MODE1); return STRUCT;}
"return" {pf(return); INIT_node_t("RETURN", MODE1); return RETURN; }
"int"|"float" {pf(type); INIT_node_t("TYPE", 16); return TYPE; }
{letter}[_a-zA-Z0-9]* { pf(id); INIT_node_t("ID", 16); return ID; }
"0"|([1-9]{digit}*) { pf(INT); INIT_node_t("INT", 8); return INT; }
0[0-7]+ {INIT_node_t("INT", 9); return INT;}
(0x|0X)[0-9A-Fa-f]+ {INIT_node_t("INT", 10); return INT;}
{digit}+\.{digit}+ { pf(FLOAT); INIT_node_t("FLOAT", 12); return FLOAT; } /*is 00.xx right??? */
({digit}*\.{digit}+|{digit}+\.)[eE][+-]?{digit}+ { pf(eFLOAT); INIT_node_t("FLOAT", 12); return FLOAT; }
([ ]|[\t]|[\r])+ {}
\n {yycolumn=1;}
"/*"[/]*[*]*([*]*([^*/]+[/]*)*)*"*/" { }
"//"[^\n]* { pf(line_comment); }
"{" { pf(LC); INIT_node_t("LC", MODE1); return LC; }
"}" { pf(RC); INIT_node_t("RC", MODE1); return RC; }
"[" { pf(LB); INIT_node_t("LB", MODE1); return LB; }
"]" { pf(RB); INIT_node_t("RB", MODE1); return RB; }
"(" { pf(LP); INIT_node_t("LP", MODE1); return LP; }
")" { pf(RP); INIT_node_t("RP", MODE1); return RP; }
">"|"<"|">="|"<="|"=="|"!=" { pf(RELOP); INIT_node_t("RELOP", MODE1); return RELOP; }
"+" { pf(PLUS); INIT_node_t("PLUS", MODE1); return PLUS; }
"-" { pf(MINUS); INIT_node_t("MINUS", MODE1); return MINUS; }
"*" { pf(STAR); INIT_node_t("STAR", MODE1); return STAR; }
"/" { pf(DIV); INIT_node_t("DIV", MODE1); return DIV; }
"&&" { pf(AND); INIT_node_t("AND", MODE1); return AND; }
"||" { pf(OR); INIT_node_t("OR", MODE1); return OR; }
"." { pf(DOT); INIT_node_t("DOT", MODE1); return DOT; }
"!" { pf(NOT); INIT_node_t("NOT", MODE1); return NOT; }
"=" { pf(ASSIGNOP); INIT_node_t("ASSIGNOP", MODE1); return ASSIGNOP; }
"," { pf(COMMA); INIT_node_t("COMMA", MODE1); return COMMA; }
";" { pf(SEMI); INIT_node_t("SEMI", MODE1); return SEMI; }
.	{ 
		error_flg = 1;
		fprintf(stderr, "Error type A at Line %d: Mysterious character \"%c\"\n", yylineno, yytext[0]); 
	} 

%%

int Oct_trans(char *src){
	int len = strlen(src);
	int ret = 0;
	for(int i=1; i<len; i++){
		ret *= 8;
		ret += src[i]-'0';
	}
	return ret;
}

int Hex_trans(char *src){
	int len = strlen(src);
	int ret = 0;
	for(int i=2; i<len; i++){
		ret *=16;
		if(src[i]>=48 && src[i]<=57) 
			ret += src[i]-'0';
		if(src[i]>=65 && src[i]<=70)
			ret += src[i]-'A'+10;
		else if(src[i]>=97 && src[i]<=102)
			ret += src[i]-'a'+10;
		else ;//assert(0);
	}
	return ret;
}

void INIT_node_t(char *name, int mode){
	//printf(" word:[%s]:[%s]\n",name,yytext);
	yylval.node_ptr = malloc(sizeof(node_t));
	yylval.node_ptr->child = yylval.node_ptr->bro = NULL;
	yylval.node_ptr->mode = mode;
	yylval.node_ptr->lineno = yylineno;
	strcpy(yylval.node_ptr->name, name);
	switch(mode){
		case 0: break;
		case 4: break;
		case 8: yylval.node_ptr->iVal=atoi(yytext);
			break;
		case 9: yylval.node_ptr->iVal=Oct_trans(yytext);
			break;
		case 10: yylval.node_ptr->iVal =Hex_trans(yytext);
			break;
		case 12: yylval.node_ptr->fVal=atof(yytext);
			break;
		case 16: strcpy(yylval.node_ptr->cVal, yytext);
			break;
		default : break;
	}
}

