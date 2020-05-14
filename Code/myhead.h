#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
enum mytokentype
  {
    myINT,
    myFLOAT,
    myID,
    myTYPE,
    myLC,
    myRC,
    myLOWER_THAN_ELSE,
    mySTRUCT,
    myRETURN,
    myIF,
    myELSE,
    myWHILE,
    mySEMI,
    myCOMMA,
    myASSIGNOP,
    myOR,
    myAND,
    myRELOP,
    myPLUS,
    myMINUS,
    mySTAR,
    myDIV,
    myNOT,
    myHIGHER_THAN_MINUS,
    myLP,
    myRP,
    myLB,
    myRB,
	myDOT,
  myProgram, myExtDefList, myExtDef, myExtDecList, mySpecifier,
  myStructSpecifier, myOptTag, myTag, myVarDec, myFunDec, myVarList,
  myParamDec, myCompSt, myStmtList, myStmt, myDefList, myDef, myDecList,
  myDec, myExp, myArgs
};
static const char *const mytname[] =
{
  "INT", "FLOAT", "ID", "TYPE", "LC", "RC",
  "LOWER_THAN_ELSE", "STRUCT", "RETURN", "IF", "ELSE", "WHILE", "SEMI",
  "COMMA", "ASSIGNOP", "OR", "AND", "RELOP", "PLUS", "MINUS", "STAR",
  "DIV", "NOT", "HIGHER_THAN_MINUS", "LP", "RP", "LB", "RB", "DOT",
  "Program", "ExtDefList", "ExtDef", "ExtDecList", "Specifier",
  "StructSpecifier", "OptTag", "Tag", "VarDec", "FunDec", "VarList",
  "ParamDec", "CompSt", "StmtList", "Stmt", "DefList", "Def", "DecList",
  "Dec", "Exp", "Args"
};

struct ArrNode{
	int asize;//size for array,int a[5] is 5
	int usize;//size for one unit type, int is 4
};
typedef struct ArrNode ArrNode;
struct ArrNodes{
	ArrNode arr;
	struct ArrNodes *next;
};
typedef struct ArrNodes ArrNodes;

//ArrNodes* newArrNodes(int size);
//ArrNodes *connectArrNodes(ArrNodes *p1, ArrNodes *p2);

typedef struct node_t node_t;
#define sym_t node_t
struct node_t{
	struct node_t *child, *bro;
	int syntype;
	int mode, lineno;
	union{
		int iVal;
		float fVal;
		char cVal[33];
	};
	/*Lab2*/
	sym_t *nxt;
	int errflg, vis;
	enum{VAR_TYPE, /*FUN_TYPE*/ STR_TYPE} id_type;
	enum{INT_TYPE, FLOAT_TYPE, STR_VAR, STR_DEF} val_type;
	int arr_dim;//array dimension cnt
	ArrNodes *arrs;//stored array size
	int argflg;//if this variable is Param

	struct node_t *member, *detail;
};
node_t *root;

//#define MY_PF3_DEBUG
#ifdef MY_PF3_DEBUG
#define pf3(x) printf("%s\n", #x)
#else
#define pf3(x) 
#endif

#define MY_ASSERT_DEBUG
#ifdef MY_ASSERT_DEBUG 
#define myassert assert
#else
#define myassert
#endif 

/*Lab3*/
/*define -1 as the default*/
#define KIND -1
/*definition of operand number*/
struct Operand{
	enum{VARIABLE, CONSTANT, TEMP, LABEL, NAME,
		 V_ADDRESS, T_ADDRESS} kind;
	union{
		//int var_no;
		int value;
		char cVal[33];//for function name
	}u;
	node_t *type;//for structue ptr
	ArrNodes *arrs;//for array ptr
};
typedef struct Operand Operand;
/*Operand link-list*/
struct Operands{
	Operand op;
	struct Operands *prev, *next;
};
typedef struct Operands Operands;

static const char *const g_SignName[] ={
	"=", "+", "-", "*", "/",
	"label", "to", "<", ">", "<=", ">=", "==", "!="
};
/*a line of intercode*/
struct InterCode{
	/*DIV redeclaration*/
	enum{ASSIGN/*assign for basic var*/, ADD, SUB, MUL, DIV_L3, 
	     LABEL_DEF, JMP, JL, JG, JLE, JGE, JE, JNE,
		 RET, FUN, CALL, ARG, PARAM, READ, WRITE,
		 ADD_ASSIGN/*assign for two pointer*/,DEC_L3} kind;
	/*union{
		struct {Operand left, right;} assisn;
		struct {Operand result, op1, op2;} binop;
	}u;*/
	Operand result, op1, op2;
};
typedef struct InterCode InterCode;
/*link-list for intercodes*/
struct InterCodes{
	InterCode code;
	struct InterCodes *prev, *next;
};
typedef struct InterCodes InterCodes;
/**/
struct VarNode{
	//char cVal[33];//variable name
	sym_t *sym;
	int var_no;//corresponding count, like v1,v2,v3,...
	struct VarNode *next;
};
typedef struct VarNode VarNode;
