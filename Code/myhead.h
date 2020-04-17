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
	int errflg;
	enum{VAR_TYPE, FUN_TYPE, STR_TYPE} id_type;
	enum{INT_TYPE, FLOAT_TYPE, STR_VAR, STR_DEF} val_type;
	//enum{STR_VAR, STR_DEF} var_def;//instanc or definition
	int arr_dim;//array dimension
	int arg_num;//args number of function
	struct node_t *member, *detail;
};
/*struct sym_t{
	struct sym_t *nxt;
	char cVal[33];
	enum{VAR_TYPE, FUN_TYPE} id_type;
	enum{INT_TYPE, FLOAT_TYPE, STR_TYPE} val_type;
	union{
		int arr_dim;//array dimension
		int arg_num;//args number of function
	};
	node_t *member, *detail;//detail used for define structure member
};*/
node_t *root;
//#define MY_SEMANTIC_DEBUG
#ifdef MY_SEMANTIC_DEBUG
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
