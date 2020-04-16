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

struct FieldList{
	int a;
};
typedef struct FieldList FieldList;
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
	int val_type;
	union{
		int arr_dim;//array dimension
		int arg_num;//args number of function
		FieldList *stru;//field for structure
	};
};
typedef struct node_t node_t;
node_t *root;
//#define MY_SEMANTIC_DEBUG
#ifdef MY_SEMANTIC_DEBUG
#define pf3(x) printf("%s\n", #x)
#else
#define pf3(x) 
#endif
