%{
/*union symbol_t{
	struct{
		union symbol_t *child, *bro;
		char name[20];
		int val; 
	}int_t;
	struct{
		union symbol_t *child, *bro;
		char name[20];
		float val;
	}float_t;
	struct{
		union symbol_t *child, *bro;
		char name[20];
		char val[33];
	}char_t;
	struct non_t{
		struct non_t *ptr;
		char name[20];
	}non_t;
};*/
struct Non_t{
	struct Non_t *child, *bro;
	char name[20];
};
typedef struct Non_t non_t;
//typedef union symbol_t symbol_t;

#include "lex.yy.c"
#include <stdarg.h>
#include <string.h>
#include <assert.h>
void yyerror(char* msg){
	fprintf(stderr, "error: %s\n",msg);
}
//#define MY_BISON_DEBUG
#ifdef MY_BISON_DEBUG
#define pf1(x,y) printf("%s:[%s]\n",#x,y)
#else 
#define pf1(x,y)
#endif

non_t *root;
void tree_insert(char *name, non_t **fa, int n_arg, ...);
void pTree(non_t* cur, int depth);

%}
%union{
	non_t *type_non;
}
%nonassoc <type_non> INT 
%nonassoc <type_non> FLOAT
%nonassoc <type_non> ID 
%nonassoc <type_non> TYPE 
%nonassoc <type_non>LC RC
%nonassoc LOWER_THAN_ELSE
%nonassoc <type_non>STRUCT RETURN IF ELSE WHILE 
%nonassoc <type_non>SEMI COMMA 
%right <type_non>ASSIGNOP 
%left <type_non>OR 
%left <type_non>AND
%left <type_non>RELOP 
%left <type_non>PLUS MINUS 
%left <type_non>STAR DIV
%right <type_non>NOT 
%left <type_non>LP RP LB RB DOT 
%type <type_non>Program ExtDefList ExtDef ExtDecList
%type <type_non>Specifier StructSpecifier OptTag Tag
%type <type_non>VarDec FunDec VarList ParamDec
%type <type_non>CompSt StmtList Stmt 
%type <type_non>DefList Def DecList Dec
%type <type_non>Exp Args
%%
/*High-level Definitions*/
Program : ExtDefList { tree_insert("Program", &$$, 1, $1); 
		root = $$; 
		pTree(root, 0); 
	}
	;
ExtDefList : ExtDef { tree_insert("ExtDefList", &$$, 1, $1); }
	| /*empty*/{  }
	;
ExtDef : Specifier ExtDecList SEMI { tree_insert("ExtDef", &$$, 3, $1, $2, $3);}
	| Specifier SEMI {tree_insert("ExtDef", &$$, 2, $1, $2);}
	| Specifier FunDec CompSt {tree_insert("ExtDef", &$$, 3, $1, $2, $3);}
	;
ExtDecList : VarDec {tree_insert("ExtDefList", &$$, 1, $1);}
	| VarDec COMMA ExtDecList {tree_insert("ExtDefList", &$$, 3, $1, $2, $3);}
	;
/*Specifiers*/
Specifier : TYPE {tree_insert("Specifier", &$$, 1, $1);}
	| StructSpecifier {tree_insert("Specifier", &$$, 1, $1);}
	;
StructSpecifier : STRUCT OptTag LC DefList RC {tree_insert("StructSpecifier", &$$, 5, $1, $2, $3, $4, $5);}
	| STRUCT Tag {tree_insert("StructSpecifier", &$$, 2, $1, $2);}
	;
OptTag : ID {tree_insert("OptTag", &$$, 1, $1);}
	| /*empty*/
	;
Tag : ID {tree_insert("Tag", &$$, 1, $1);}
	;
/*Declarators*/
VarDec : ID {//printf("%d1\n",(int)(intptr_t)&$$); 
		tree_insert("VarDec", &$$, 1, $1);
	}
	| VarDec LB INT RB {tree_insert("VarDec", &$$, 4, $1, $2, $3, $4);}
	;
FunDec : ID LP VarList RP {tree_insert("FunDec", &$$, 4, $1, $2, $3, $4);}
	| ID LP RP {tree_insert("FunDec", &$$, 3, $1, $2, $3);}
	;
VarList : ParamDec COMMA VarList {tree_insert("VarList", &$$, 3, $1, $2, $3);}
	| ParamDec {tree_insert("VarList", &$$, 1, $1);}
	;
ParamDec : Specifier VarDec {tree_insert("ParamDec", &$$, 2, $1, $2);}
	;
/*Statements*/
CompSt : LC DefList StmtList RC {tree_insert("CompSt", &$$, 4, $1, $2, $3, $4);}
	;
StmtList : Stmt StmtList  {tree_insert("StmtList", &$$, 2, $1, $2);}
	| /*empty*/ 
	;
Stmt : Exp SEMI  {tree_insert("Stmt", &$$, 2, $1, $2);}
	| CompSt  {tree_insert("Stmt", &$$, 1, $1);}
	| RETURN Exp SEMI  {tree_insert("Stmt", &$$, 3, $1, $2, $3);}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {tree_insert("Stmt", &$$, 5, $1, $2, $3, $4, $5);}
	| IF LP Exp RP Stmt ELSE Stmt {tree_insert("Stmt", &$$, 7, $1, $2, $3, $4, $5, $6, $7);}
	| WHILE LP Exp RP Stmt  {tree_insert("Stmt", &$$, 5, $1, $2, $3, $4, $5);}
	;
/*Local Definitions*/
DefList : Def DefList  {tree_insert("DefList", &$$, 2, $1, $2);}
	| /*empty*/
	;
Def : Specifier DecList SEMI {tree_insert("Def", &$$, 3, $1, $2, $3);}
	;
DecList : Dec {tree_insert("DecList", &$$, 1, $1);} 
	| Dec COMMA DecList {tree_insert("DecList", &$$, 3, $1, $2, $3);} 
	;
Dec : VarDec {tree_insert("Dec", &$$, 1, $1);} 
	| VarDec ASSIGNOP Exp {//printf("%d2\n",(int)(intptr_t)$1);
	tree_insert("Dec", &$$, 3, $1, $2, $3);}
	;
/*Expressions*/
Exp : Exp ASSIGNOP Exp  {tree_insert("Exp", &$$, 3, $1, $2, $3);}
	| Exp AND Exp  {tree_insert("Exp", &$$, 3, $1, $2, $3);}
	| Exp OR Exp  {tree_insert("Exp", &$$, 3, $1, $2, $3);}
	| Exp RELOP Exp {tree_insert("Exp", &$$, 3, $1, $2, $3);}
	| Exp PLUS Exp  {tree_insert("Exp", &$$, 3, $1, $2, $3);}
	| Exp MINUS Exp  {tree_insert("Exp", &$$, 3, $1, $2, $3);}
	| Exp STAR Exp  {tree_insert("Exp", &$$, 3, $1, $2, $3);}
	| Exp DIV Exp  {tree_insert("Exp", &$$, 3, $1, $2, $3);}
	| LP Exp RP {tree_insert("Exp", &$$, 3, $1, $2, $3);}
	| MINUS Exp {tree_insert("Exp", &$$, 2, $1, $2);}
	| NOT Exp  {tree_insert("Exp", &$$, 2, $1, $2);}
	| ID LP Args RP {tree_insert("Exp", &$$, 4, $1, $2, $3, $4);}
	| ID LP RP {tree_insert("Exp", &$$, 3, $1, $2, $3);}
	| Exp LB Exp RB {tree_insert("Exp", &$$, 4, $1, $2, $3, $4);}
	| Exp DOT ID {tree_insert("Exp", &$$, 3, $1, $2, $3);}
	| ID  {tree_insert("Exp", &$$, 1, $1);}
	| INT  {tree_insert("Exp", &$$, 1, $1);}
	| FLOAT  {tree_insert("Exp", &$$, 1, $1);}
	;
Args : Exp COMMA Args {tree_insert("Args", &$$, 3, $1, $2, $3);}
	| Exp {tree_insert("Args", &$$, 1, $1);}
	;
%%
void tree_insert(char *name, non_t** fa, int n_arg, ...){
	va_list args;
	va_start(args, n_arg);
	*fa	= malloc(sizeof(non_t));
	non_t *cur = *fa;
	cur->child = NULL; 
	cur->bro = NULL;
	strcpy(cur->name, name);
	for(int i=0; i<n_arg; i++){
		non_t *nxt = va_arg(args, non_t*); 
		//pf1(nxt,nxt->name);
		if(i==0) cur->child = nxt;
		else cur->bro = nxt;
		cur = nxt;
	}
	va_end(args);
}

void pTree(non_t* cur, int depth){
	for(int i=0; i<depth; i++) printf("  ");
	printf("%s\n",cur->name);
	non_t *p = cur->child;
	if(p!=NULL){
		pTree(p, depth+1);
	}
	p = cur->bro;
	if(p!=NULL){
		pTree(p, depth);
	}
}
