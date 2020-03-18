%{
union symbol_t{
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
	struct{
		union symbol_t *child, *bro;
		char name[20];
	}non_t;
};
typedef union symbol_t symbol_t;
#define YYSTYPE symbol_t 

#include "lex.yy.c"
#include <stdarg.h>
#include <string.h>
#include <assert.h>
void yyerror(char* msg){
	fprintf(stderr, "error: %s\n",msg);
}
#define MY_BISON_DEBUG
#ifdef MY_BISON_DEBUG
#define pf1(x,y) printf("%s: %s\n",#x,#y)
#else 
#define pf1(x)
#endif

YYSTYPE *root;
void tree_insert(char *name, YYSTYPE *fa, int n_arg, ...);
void pTree();

%}
/*%union{
	token_t;
	non_t
}*/

%nonassoc LC RC
%nonassoc LOWER_THAN_ELSE
%nonassoc STRUCT RETURN IF ELSE WHILE 
%nonassoc SEMI COMMA 
%nonassoc <int_t> INT 
%nonassoc <float_t> FLOAT
%nonassoc <char_t> ID 
%nonassoc <char_t> TYPE 
%right ASSIGNOP 
%left OR 
%left AND
%left RELOP 
%left PLUS MINUS 
%left STAR DIV
%right NOT 
%left LP RP LB RB DOT 
%type <non_t>Program ExtDefList 
%%
/*High-level Definitions*/
Program : ExtDefList { tree_insert("Program", (YYSTYPE*)(&$$), 1, (YYSTYPE *)(&$1) ); 
		root = (YYSTYPE*)(&$$);
		pTree(); 
	}
	;
ExtDefList : ExtDef { 
		//printf("%d\n",(int)(intptr_t)&$$);
		//printf("1\n");
		//YYSTYPE *p = (YYSTYPE *)&$$;
		tree_insert("ExtDefList", (YYSTYPE*)(&$$), 0); 
	}
	| /*empty*/
	;
ExtDef : Specifier ExtDecList SEMI {}
	| Specifier SEMI {}
	| Specifier FunDec CompSt {}
	;
ExtDecList : VarDec {}
	| VarDec COMMA ExtDecList {}
	;
/*Specifiers*/
Specifier : TYPE {}
	| StructSpecifier {}
	;
StructSpecifier : STRUCT OptTag LC DefList RC {}
	| STRUCT Tag {}
	;
OptTag : ID {}
	| /*empty*/
	;
Tag : ID {}
	;
/*Declarators*/
VarDec : ID {}
	| VarDec LB INT RB {}
	;
FunDec : ID LP VarList RP {}
	| ID LP RP {}
	;
VarList : ParamDec COMMA VarList {}
	| ParamDec 
	;
ParamDec : Specifier VarDec
	;
/*Statements*/
CompSt : LC DefList StmtList RC
	;
StmtList : Stmt StmtList 
	| /*empty*/ 
	;
Stmt : Exp SEMI 
	| CompSt 
	| RETURN Exp SEMI 
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
	| IF LP Exp RP Stmt ELSE Stmt
	| WHILE LP Exp RP Stmt 
	;
/*Local Definitions*/
DefList : Def DefList 
	| /*empty*/
	;
Def : Specifier DecList SEMI
	;
DecList : Dec 
	| Dec COMMA DecList 
	;
Dec : VarDec 
	| VarDec ASSIGNOP Exp
	;
/*Expressions*/
Exp : Exp ASSIGNOP Exp 
	| Exp AND Exp 
	| Exp OR Exp 
	| Exp RELOP Exp
	| Exp PLUS Exp 
	| Exp MINUS Exp 
	| Exp STAR Exp 
	| Exp DIV Exp 
	| LP Exp RP
	| MINUS Exp
	| NOT Exp 
	| ID LP Args RP
	| ID LP RP
	| Exp LB Exp RB
	| Exp DOT ID
	| ID 
	| INT 
	| FLOAT 
	;
Args : Exp COMMA Args
	| Exp
	;
%%
void tree_insert(char *name, YYSTYPE* fa, int n_arg, ...){
	va_list args;
	va_start(args, n_arg);
	YYSTYPE *cur = fa;
	cur->non_t.child = NULL; 
	cur->non_t.bro = NULL;
	strcpy(cur->non_t.name, name);
	//printf("%d\n",(int)(intptr_t)cur);
	//assert(0);
	for(int i=0; i<n_arg; i++){
		YYSTYPE *p = va_arg(args, YYSTYPE*); 
		cur->non_t.child = p;
		cur = p;
	}
	va_end(args);
}

void pTree(){
	printf("%s\n",root->non_t.name);
	YYSTYPE *p = root->non_t.child;
	while(p!=NULL){
		printf("%s\n",p->non_t.name);
		p = p->non_t.child;
	}
}
