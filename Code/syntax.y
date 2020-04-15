%locations
%error-verbose
%{ 
#include "lex.yy.c"
int error_flg;
void yyerror(const char* msg){
	error_flg=1;
	fprintf(stderr, "Error type B at Line %d: %s.\n", yylineno, msg);
}

void tree_insert(int syntype, node_t **fa, YYLTYPE linetype, int n_arg, ...);
void pTree();
void pSubTree(node_t* cur, int depth);
%}
%union {
	struct node_t *node_ptr;
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
Program : ExtDefList { tree_insert(myProgram, &$$,@$, 1, $1); 
		root = $$; 
		//only empty character
		if($1->child == NULL) $$->lineno = $1->lineno;
	}
	;
ExtDefList : ExtDef ExtDefList { tree_insert(myExtDefList, &$$,@$, 2, $1, $2); }
	| /*empty*/{ tree_insert(myExtDefList, &$$,@$, 0); $$->lineno=yylineno; }
	| error RC {}
	;
ExtDef : Specifier ExtDecList SEMI { tree_insert(myExtDef, &$$,@$, 3, $1, $2, $3);}
	| Specifier SEMI {tree_insert(myExtDef, &$$,@$, 2, $1, $2);}
	| Specifier FunDec CompSt {tree_insert(myExtDef, &$$,@$, 3, $1, $2, $3);}
	| Specifier error LC {}
	;
ExtDecList : VarDec {tree_insert(myExtDecList, &$$,@$, 1, $1);}
	| VarDec COMMA ExtDecList {tree_insert(myExtDecList, &$$,@$, 3, $1, $2, $3);}
	;
/*Specifiers*/
Specifier : TYPE {tree_insert(mySpecifier, &$$,@$, 1, $1);}
	| StructSpecifier {tree_insert(mySpecifier, &$$,@$, 1, $1);}
	;
StructSpecifier : STRUCT OptTag LC DefList RC {tree_insert(myStructSpecifier, &$$,@$, 5, $1, $2, $3, $4, $5);}
	| STRUCT Tag {tree_insert(myStructSpecifier, &$$,@$, 2, $1, $2);}
	| STRUCT error RC {}
	;
OptTag : ID {tree_insert(myOptTag, &$$,@$, 1, $1);}
	| /*empty*/{tree_insert(myOptTag, &$$,@$, 0); }
	;
Tag : ID {tree_insert(myTag, &$$,@$, 1, $1);}
	;
/*Declarators*/
VarDec : ID {//printf("%d1\n",(int)(intptr_t)&$$,@$); 
		tree_insert(myVarDec, &$$,@$, 1, $1);
	}
	| VarDec LB INT RB {tree_insert(myVarDec, &$$,@$, 4, $1, $2, $3, $4);}
	| error RB {}
	;
FunDec : ID LP VarList RP {tree_insert(myFunDec, &$$,@$, 4, $1, $2, $3, $4);}
	| ID LP RP {tree_insert(myFunDec, &$$,@$, 3, $1, $2, $3);}
	| error RP {}
	;
VarList : ParamDec COMMA VarList {tree_insert(myVarList, &$$,@$, 3, $1, $2, $3);}
	| ParamDec {tree_insert(myVarList, &$$,@$, 1, $1);}
	| error COMMA {}
	;
ParamDec : Specifier VarDec {tree_insert(myParamDec, &$$,@$, 2, $1, $2);}
	;
/*Statements*/
CompSt : LC DefList StmtList RC {tree_insert(myCompSt, &$$,@$, 4, $1, $2, $3, $4);}
	| error LC {}
	;
StmtList : Stmt StmtList  {tree_insert(myStmtList, &$$,@$, 2, $1, $2);}
	| /*empty*/{tree_insert(myStmtList, &$$,@$, 0);}
	;
Stmt : Exp SEMI  {tree_insert(myStmt, &$$,@$, 2, $1, $2);}
	| CompSt  {tree_insert(myStmt, &$$,@$, 1, $1);}
	| RETURN Exp SEMI  {tree_insert(myStmt, &$$,@$, 3, $1, $2, $3);}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {tree_insert(myStmt, &$$,@$, 5, $1, $2, $3, $4, $5);}
	| IF LP Exp RP Stmt ELSE Stmt {tree_insert(myStmt, &$$,@$, 7, $1, $2, $3, $4, $5, $6, $7);}
	| WHILE LP Exp RP Stmt  {tree_insert(myStmt, &$$,@$, 5, $1, $2, $3, $4, $5);}
	| error SEMI {}
	| Exp error SEMI {}
	;
/*Local Definitions*/
DefList : Def DefList  {tree_insert(myDefList, &$$,@$, 2, $1, $2);}
	| /*empty*/{tree_insert(myDefList, &$$,@$, 0);}
	;
Def : Specifier DecList SEMI {tree_insert(myDef, &$$,@$, 3, $1, $2, $3);}
	| Specifier error SEMI {}
	;
DecList : Dec {tree_insert(myDecList, &$$,@$, 1, $1);} 
	| Dec COMMA DecList {tree_insert(myDecList, &$$,@$, 3, $1, $2, $3);} 
	| error COMMA {}
	;
Dec : VarDec {tree_insert(myDec, &$$,@$, 1, $1);} 
	| VarDec ASSIGNOP Exp {tree_insert(myDec, &$$,@$, 3, $1, $2, $3);}
	| error RP {}
	;
/*Expressions*/
Exp : Exp ASSIGNOP Exp  {tree_insert(myExp, &$$,@$, 3, $1, $2, $3);}
	| Exp AND Exp  {tree_insert(myExp, &$$,@$, 3, $1, $2, $3);}
	| Exp OR Exp  {tree_insert(myExp, &$$,@$, 3, $1, $2, $3);}
	| Exp RELOP Exp {tree_insert(myExp, &$$,@$, 3, $1, $2, $3);}
	| Exp PLUS Exp  {tree_insert(myExp, &$$,@$, 3, $1, $2, $3);}
	| Exp MINUS Exp  {tree_insert(myExp, &$$,@$, 3, $1, $2, $3);}
	| Exp STAR Exp  {tree_insert(myExp, &$$,@$, 3, $1, $2, $3);}
	| Exp DIV Exp  {tree_insert(myExp, &$$,@$, 3, $1, $2, $3);}
	| LP Exp RP {tree_insert(myExp, &$$,@$, 3, $1, $2, $3);}
	| MINUS Exp %prec HIGHER_THAN_MINUS {tree_insert(myExp, &$$,@$, 2, $1, $2);}
	| NOT Exp  {tree_insert(myExp, &$$,@$, 2, $1, $2);}
	| ID LP Args RP {tree_insert(myExp, &$$,@$, 4, $1, $2, $3, $4);}
	| ID LP RP {tree_insert(myExp, &$$,@$, 3, $1, $2, $3);}
	| Exp LB Exp RB {
		tree_insert(myExp, &$$,@$, 4, $1, $2, $3, $4);
	}
	| Exp DOT ID {tree_insert(myExp, &$$,@$, 3, $1, $2, $3);}
	| ID  {tree_insert(myExp, &$$,@$, 1, $1);}
	| INT  {
		tree_insert(myExp, &$$,@$, 1, $1);
	}
	| FLOAT  {
		tree_insert(myExp, &$$,@$, 1, $1); 
	}
	| Exp LB error RB {}
	| error RP {}
	;
Args : Exp COMMA Args {tree_insert(myArgs, &$$,@$, 3, $1, $2, $3);}
	| Exp {tree_insert(myArgs, &$$,@$, 1, $1);} 
	;
%%
void tree_insert(int syntype, node_t** fa,YYLTYPE linetype, int n_arg, ...){
	if(error_flg) return;
	va_list args;
	va_start(args, n_arg);
	*fa	= malloc(sizeof(node_t));
	//printf("insert:[%s], line:%d\n",name, linetype.first_line);
	node_t *cur = *fa;
	cur->child = NULL; 
	cur->bro = NULL;
	cur->mode = (n_arg>0) ? 0 : -1;//mode<0 means empty unit
	cur->lineno = linetype.first_line;
	cur->syntype = syntype;
	for(int i=0; i<n_arg; i++){
		node_t *nxt = va_arg(args, node_t*); 
		//printf("  nxt:[%s], line:%d\n",nxt->name, nxt->lineno);
		if(nxt->mode<0) continue;// empty string
		if(i==0) cur->child = nxt;
		else cur->bro = nxt;
		cur = nxt;
	}
	//pTree(*fa, 1);
	va_end(args);
}

void pTree(){
	pSubTree(root, 0);
}

void pSubTree(node_t* cur, int depth){
	if(error_flg) return;
	//space
	for(int i=0; i<depth; i++) printf("  ");
	//empty case
	printf("%s",mytname[cur->syntype]);
	//case
	switch(cur->mode/4){
		case 0: printf(" (%d)\n",cur->lineno); break;
		case 1: printf("\n"); break;
		case 2:printf(": %d\n",cur->iVal); break;
		case 3: printf(": %f\n",cur->fVal); break;
		case 4: printf(": %s\n",cur->cVal); break;
		default: break;
	}
	node_t *p = cur->child;
	if(p!=NULL){
		pSubTree(p, depth+1);
	}
	p = cur->bro;
	if(p!=NULL){
		pSubTree(p, depth);
	}
}
