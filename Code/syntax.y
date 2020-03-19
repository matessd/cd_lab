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
	struct non_t{
		union symbol_t *child, *bro;
		char name[20];
	}non_t;
};
typedef struct non_t non_t;
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
#define pf1(x,y) printf("%s:[%s]\n",#x,y)
#else 
#define pf1(x,y)
#endif

YYSTYPE *root;
void tree_insert(char *name, non_t *fa, int n_arg, ...);
void pTree();

%}

%nonassoc <int_t> INT 
%nonassoc <float_t> FLOAT
%nonassoc <char_t> ID 
%nonassoc <char_t> TYPE 
%nonassoc <non_t>LC RC
%nonassoc LOWER_THAN_ELSE
%nonassoc <non_t>STRUCT RETURN IF ELSE WHILE 
%nonassoc <non_t>SEMI COMMA 
%right <non_t>ASSIGNOP 
%left <non_t>OR 
%left <non_t>AND
%left <non_t>RELOP 
%left <non_t>PLUS MINUS 
%left <non_t>STAR DIV
%right <non_t>NOT 
%left <non_t>LP RP LB RB DOT 
%type <non_t>Program ExtDefList ExtDef ExtDecList
%type <non_t>Specifier StructSpecifier OptTag Tag
%type <non_t>VarDec FunDec VarList ParamDec
%type <non_t>CompSt StmtList Stmt 
%type <non_t>DefList Def DecList Dec
%type <non_t>Exp Args
%%
/*High-level Definitions*/
Program : ExtDefList { tree_insert("Program", &$$, 1, &$1); 
		root = (YYSTYPE*)&$$; 
		pTree(root, 0); 
	}
	;
ExtDefList : ExtDef { tree_insert("ExtDefList", &$$, 1, &$1); }
	| /*empty*/{  }
	;
ExtDef : Specifier ExtDecList SEMI { tree_insert("ExtDef", &$$, 3, &$1, &$2, &$3);}
	| Specifier SEMI {tree_insert("ExtDef", &$$, 2, &$1, &$2);}
	| Specifier FunDec CompSt {tree_insert("ExtDef", &$$, 3, &$1, &$2, &$3);}
	;
ExtDecList : VarDec {tree_insert("ExtDefList", &$$, 1, &$1);}
	| VarDec COMMA ExtDecList {tree_insert("ExtDefList", &$$, 3, &$1, &$2, &$3);}
	;
/*Specifiers*/
Specifier : TYPE {tree_insert("Specifier", &$$, 1, &$1);}
	| StructSpecifier {tree_insert("Specifier", &$$, 1, &$1);}
	;
StructSpecifier : STRUCT OptTag LC DefList RC {tree_insert("StructSpecifier", &$$, 5, &$1, &$2, &$3, &$4, &$5);}
	| STRUCT Tag {tree_insert("StructSpecifier", &$$, 2, &$1, &$2);}
	;
OptTag : ID {tree_insert("OptTag", &$$, 1, &$1);}
	| /*empty*/
	;
Tag : ID {tree_insert("Tag", &$$, 1, &$1);}
	;
/*Declarators*/
VarDec : ID {tree_insert("VarDec", &$$, 1, &$1);}
	| VarDec LB INT RB {tree_insert("VarDec", &$$, 4, &$1, &$2, &$3, &$4);}
	;
FunDec : ID LP VarList RP {tree_insert("FunDec", &$$, 4, &$1, &$2, &$3, &$4);}
	| ID LP RP {tree_insert("FunDec", &$$, 3, &$1, &$2, &$3);}
	;
VarList : ParamDec COMMA VarList {tree_insert("VarList", &$$, 3, &$1, &$2, &$3);}
	| ParamDec {tree_insert("VarList", &$$, 1, &$1);}
	;
ParamDec : Specifier VarDec {tree_insert("ParamDec", &$$, 2, &$1, &$2);}
	;
/*Statements*/
CompSt : LC DefList StmtList RC {tree_insert("CompSt", &$$, 4, &$1, &$2, &$3, &$4);}
	;
StmtList : Stmt StmtList  {tree_insert("StmtList", &$$, 2, &$1, &$2);}
	| /*empty*/ 
	;
Stmt : Exp SEMI  {tree_insert("Stmt", &$$, 2, &$1, &$2);}
	| CompSt  {tree_insert("Stmt", &$$, 1, &$1);}
	| RETURN Exp SEMI  {tree_insert("Stmt", &$$, 3, &$1, &$2, &$3);}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {tree_insert("Stmt", &$$, 5, &$1, &$2, &$3, &$4, &$5);}
	| IF LP Exp RP Stmt ELSE Stmt {tree_insert("Stmt", &$$, 7, &$1, &$2, &$3, &$4, &$5, &$6, &$7);}
	| WHILE LP Exp RP Stmt  {tree_insert("Stmt", &$$, 5, &$1, &$2, &$3, &$4, &$5);}
	;
/*Local Definitions*/
DefList : Def DefList  {tree_insert("DefList", &$$, 2, &$1, &$2);}
	| /*empty*/
	;
Def : Specifier DecList SEMI {tree_insert("Def", &$$, 3, &$1, &$2, &$3);}
	;
DecList : Dec {tree_insert("DecList", &$$, 1, &$1);} 
	| Dec COMMA DecList {tree_insert("DecList", &$$, 3, &$1, &$2, &$3);} 
	;
Dec : VarDec {tree_insert("Dec", &$$, 1, &$1);} 
	| VarDec ASSIGNOP Exp {tree_insert("Dec", &$$, 3, &$1, &$2, &$3);}
	;
/*Expressions*/
Exp : Exp ASSIGNOP Exp  {tree_insert("Exp", &$$, 3, &$1, &$2, &$3);}
	| Exp AND Exp  {tree_insert("Exp", &$$, 3, &$1, &$2, &$3);}
	| Exp OR Exp  {tree_insert("Exp", &$$, 3, &$1, &$2, &$3);}
	| Exp RELOP Exp {tree_insert("Exp", &$$, 3, &$1, &$2, &$3);}
	| Exp PLUS Exp  {tree_insert("Exp", &$$, 3, &$1, &$2, &$3);}
	| Exp MINUS Exp  {tree_insert("Exp", &$$, 3, &$1, &$2, &$3);}
	| Exp STAR Exp  {tree_insert("Exp", &$$, 3, &$1, &$2, &$3);}
	| Exp DIV Exp  {tree_insert("Exp", &$$, 3, &$1, &$2, &$3);}
	| LP Exp RP {tree_insert("Exp", &$$, 3, &$1, &$2, &$3);}
	| MINUS Exp {tree_insert("Exp", &$$, 2, &$1, &$2);}
	| NOT Exp  {tree_insert("Exp", &$$, 2, &$1, &$2);}
	| ID LP Args RP {tree_insert("Exp", &$$, 4, &$1, &$2, &$3, $4);}
	| ID LP RP {tree_insert("Exp", &$$, 3, &$1, &$2, &$3);}
	| Exp LB Exp RB {tree_insert("Exp", &$$, 4, &$1, &$2, &$3, &$4);}
	| Exp DOT ID {tree_insert("Exp", &$$, 3, &$1, &$2, &$3);}
	| ID  {tree_insert("Exp", &$$, 1, &$1);}
	| INT  {tree_insert("Exp", &$$, 1, &$1);}
	| FLOAT  {tree_insert("Exp", &$$, 1, &$1);}
	;
Args : Exp COMMA Args {tree_insert("Args", &$$, 3, &$1, &$2, &$3);}
	| Exp {tree_insert("Args", &$$, 1, &$1);}
	;
%%
void tree_insert(char *name, non_t* fa, int n_arg, ...){
	va_list args;
	va_start(args, n_arg);
	pf1(insert, name);
	YYSTYPE *cur = (YYSTYPE*)fa;
	cur->non_t.child = NULL; 
	cur->non_t.bro = NULL;
	strcpy(cur->non_t.name, name);
	//printf("%s**\n",name);
	for(int i=0; i<n_arg; i++){
		YYSTYPE *nxt = va_arg(args, YYSTYPE*); 
		//pf1(nxt,nxt->non_t.name);
		if(i==0) cur->non_t.child = nxt;
		else cur->non_t.bro = nxt;
		cur = nxt;
	}
	//pf1(cur**,cur->non_t.name);
	va_end(args);
}

void pTree(YYSTYPE* cur, int depth){
	for(int i=0; i<depth; i++) printf("  ");
	printf("%s\n",cur->non_t.name);
	YYSTYPE *p = cur->non_t.child;
	if(p!=NULL){
	//printf("1\n");
		pTree(p, depth+1);
	}
	p = cur->non_t.bro;
	if(p!=NULL){
	//printf("2\n");
		pTree(p, depth);
	}
}
