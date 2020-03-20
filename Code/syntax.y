%locations
%{
struct node_t{
	struct node_t *child, *bro;
	char name[16];
	int mode, lineno;
	union{
		int iVal;
		float fVal;
		char cVal[33];
	};
};
typedef struct node_t node_t;

#include "lex.yy.c"
#include <stdarg.h>
#include <string.h>
#include <assert.h>
void yyerror(char* msg){
	fprintf(stderr, "error: %s.\n",msg);
}
#define MY_BISON_DEBUG
#ifdef MY_BISON_DEBUG
#define pf1(x,y) printf("%s:[%s]\n",#x,y)
#else 
#define pf1(x,y)
#endif

node_t *root;
void tree_insert(char *name, node_t **fa, YYLTYPE linetype, int n_arg, ...);
void pTree(node_t* cur, int depth);
%}
%union{
	node_t *node_ptr;
}
%nonassoc <node_ptr> INT 
%nonassoc <node_ptr> FLOAT
%nonassoc <node_ptr> ID 
%nonassoc <node_ptr> TYPE 
%nonassoc <node_ptr>LC RC
%nonassoc LOWER_THAN_ELSE
%nonassoc <node_ptr>STRUCT RETURN IF ELSE WHILE 
%nonassoc <node_ptr>SEMI COMMA 
%right <node_ptr>ASSIGNOP 
%left <node_ptr>OR 
%left <node_ptr>AND
%left <node_ptr>RELOP 
%left <node_ptr>PLUS MINUS 
%left <node_ptr>STAR DIV
%right <node_ptr>NOT HIGHER_THAN_MINUS 
%left <node_ptr>LP RP LB RB DOT 
/*non_terminal*/
%type <node_ptr>Program ExtDefList ExtDef ExtDecList
%type <node_ptr>Specifier StructSpecifier OptTag Tag
%type <node_ptr>VarDec FunDec VarList ParamDec
%type <node_ptr>CompSt StmtList Stmt 
%type <node_ptr>DefList Def DecList Dec
%type <node_ptr>Exp Args
%%
/*High-level Definitions*/
Program : ExtDefList { tree_insert("Program", &$$,@$, 1, $1); 
		root = $$; 
		//only empty character
		if($1->child == NULL) $$->lineno = $1->lineno;
		pTree(root, 0); 
	}
	;
ExtDefList : ExtDef ExtDefList { tree_insert("ExtDefList", &$$,@$, 2, $1, $2); }
	| /*empty*/{ tree_insert("ExtDefList", &$$,@$, 0); $$->lineno=yylineno; }
	;
ExtDef : Specifier ExtDecList SEMI { tree_insert("ExtDef", &$$,@$, 3, $1, $2, $3);}
	| Specifier SEMI {tree_insert("ExtDef", &$$,@$, 2, $1, $2);}
	| Specifier FunDec CompSt {tree_insert("ExtDef", &$$,@$, 3, $1, $2, $3);}
	;
ExtDecList : VarDec {tree_insert("ExtDecList", &$$,@$, 1, $1);}
	| VarDec COMMA ExtDecList {tree_insert("ExtDecList", &$$,@$, 3, $1, $2, $3);}
	;
/*Specifiers*/
Specifier : TYPE {tree_insert("Specifier", &$$,@$, 1, $1);}
	| StructSpecifier {tree_insert("Specifier", &$$,@$, 1, $1);}
	;
StructSpecifier : STRUCT OptTag LC DefList RC {tree_insert("StructSpecifier", &$$,@$, 5, $1, $2, $3, $4, $5);}
	| STRUCT Tag {tree_insert("StructSpecifier", &$$,@$, 2, $1, $2);}
	;
OptTag : ID {tree_insert("OptTag", &$$,@$, 1, $1);}
	| /*empty*/{tree_insert("OptTag", &$$,@$, 0); }
	;
Tag : ID {tree_insert("Tag", &$$,@$, 1, $1);}
	;
/*Declarators*/
VarDec : ID {//printf("%d1\n",(int)(intptr_t)&$$,@$); 
		tree_insert("VarDec", &$$,@$, 1, $1);
	}
	| VarDec LB INT RB {tree_insert("VarDec", &$$,@$, 4, $1, $2, $3, $4);}
	;
FunDec : ID LP VarList RP {tree_insert("FunDec", &$$,@$, 4, $1, $2, $3, $4);}
	| ID LP RP {tree_insert("FunDec", &$$,@$, 3, $1, $2, $3);}
	;
VarList : ParamDec COMMA VarList {tree_insert("VarList", &$$,@$, 3, $1, $2, $3);}
	| ParamDec {tree_insert("VarList", &$$,@$, 1, $1);}
	;
ParamDec : Specifier VarDec {tree_insert("ParamDec", &$$,@$, 2, $1, $2);}
	;
/*Statements*/
CompSt : LC DefList StmtList RC {tree_insert("CompSt", &$$,@$, 4, $1, $2, $3, $4);}
	;
StmtList : Stmt StmtList  {tree_insert("StmtList", &$$,@$, 2, $1, $2);}
	| /*empty*/{tree_insert("StmtList", &$$,@$, 0);}
	;
Stmt : Exp SEMI  {tree_insert("Stmt", &$$,@$, 2, $1, $2);}
	| CompSt  {tree_insert("Stmt", &$$,@$, 1, $1);}
	| RETURN Exp SEMI  {tree_insert("Stmt", &$$,@$, 3, $1, $2, $3);}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {tree_insert("Stmt", &$$,@$, 5, $1, $2, $3, $4, $5);}
	| IF LP Exp RP Stmt ELSE Stmt {tree_insert("Stmt", &$$,@$, 7, $1, $2, $3, $4, $5, $6, $7);}
	| WHILE LP Exp RP Stmt  {tree_insert("Stmt", &$$,@$, 5, $1, $2, $3, $4, $5);}
	;
/*Local Definitions*/
DefList : Def DefList  {tree_insert("DefList", &$$,@$, 2, $1, $2);}
	| /*empty*/{tree_insert("DefList", &$$,@$, 0);}
	;
Def : Specifier DecList SEMI {tree_insert("Def", &$$,@$, 3, $1, $2, $3);}
	;
DecList : Dec {tree_insert("DecList", &$$,@$, 1, $1);} 
	| Dec COMMA DecList {tree_insert("DecList", &$$,@$, 3, $1, $2, $3);} 
	;
Dec : VarDec {tree_insert("Dec", &$$,@$, 1, $1);} 
	| VarDec ASSIGNOP Exp {//printf("%d2\n",(int)(intptr_t)$1);
	tree_insert("Dec", &$$,@$, 3, $1, $2, $3);}
	;
/*Expressions*/
Exp : Exp ASSIGNOP Exp  {tree_insert("Exp", &$$,@$, 3, $1, $2, $3);}
	| Exp AND Exp  {tree_insert("Exp", &$$,@$, 3, $1, $2, $3);}
	| Exp OR Exp  {tree_insert("Exp", &$$,@$, 3, $1, $2, $3);}
	| Exp RELOP Exp {tree_insert("Exp", &$$,@$, 3, $1, $2, $3);}
	| Exp PLUS Exp  {tree_insert("Exp", &$$,@$, 3, $1, $2, $3);}
	| Exp MINUS Exp  {tree_insert("Exp", &$$,@$, 3, $1, $2, $3);}
	| Exp STAR Exp  {tree_insert("Exp", &$$,@$, 3, $1, $2, $3);}
	| Exp DIV Exp  {tree_insert("Exp", &$$,@$, 3, $1, $2, $3);}
	| LP Exp RP {tree_insert("Exp", &$$,@$, 3, $1, $2, $3);}
	| MINUS Exp %prec HIGHER_THAN_MINUS {tree_insert("Exp", &$$,@$, 2, $1, $2);}
	| NOT Exp  {tree_insert("Exp", &$$,@$, 2, $1, $2);}
	| ID LP Args RP {tree_insert("Exp", &$$,@$, 4, $1, $2, $3, $4);}
	| ID LP RP {tree_insert("Exp", &$$,@$, 3, $1, $2, $3);}
	| Exp LB Exp RB {tree_insert("Exp", &$$,@$, 4, $1, $2, $3, $4);}
	| Exp DOT ID {tree_insert("Exp", &$$,@$, 3, $1, $2, $3);}
	| ID  {tree_insert("Exp", &$$,@$, 1, $1);}
	| INT  {tree_insert("Exp", &$$,@$, 1, $1);}
	| FLOAT  {tree_insert("Exp", &$$,@$, 1, $1);}
	;
Args : Exp COMMA Args {tree_insert("Args", &$$,@$, 3, $1, $2, $3);}
	| Exp {tree_insert("Args", &$$,@$, 1, $1);}
	;
%%
void tree_insert(char *name, node_t** fa,YYLTYPE linetype, int n_arg, ...){
	va_list args;
	va_start(args, n_arg);
	*fa	= malloc(sizeof(node_t));
	//pf1(insert, name);
	node_t *cur = *fa;
	cur->child = NULL; 
	cur->bro = NULL;
	cur->mode = (n_arg>0) ? 0 : -1;//mode<0 means empty unit
	cur->lineno = linetype.first_line;
	strcpy(cur->name, name);
	for(int i=0; i<n_arg; i++){
		node_t *nxt = va_arg(args, node_t*); 
		//pf1(nxt, nxt->name);
		if(nxt->mode<0) continue;// empty string
		if(i==0) cur->child = nxt;
		else cur->bro = nxt;
		cur = nxt;
	}
	//pTree(*fa, 1);
	va_end(args);
}

void pTree(node_t* cur, int depth){
	//space
	for(int i=0; i<depth; i++) printf("  ");
	//empty case
	printf("%s",cur->name);
	//case
	switch(cur->mode/4){
		case 0: printf(" (%d)\n",cur->lineno); break;
		case 1: printf("\n"); break;
		case 2:printf(": %d\n",cur->iVal); break;
		case 3: printf(": %f\n",cur->fVal); break;
		case 4: printf(": %s\n",cur->cVal); break;
		default: assert(0); break;
	}
	node_t *p = cur->child;
	if(p!=NULL){
		pTree(p, depth+1);
	}
	p = cur->bro;
	if(p!=NULL){
		pTree(p, depth);
	}
}
