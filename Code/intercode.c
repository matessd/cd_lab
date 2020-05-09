#include "myhead.h"
InterCodes *Head=NULL;
VarNode *g_VarTable=NULL;//global table for variable search
int VarNo=0;//remember number for next variable

/*Description: insert variable into table
Args: cVal is the identifier name
*/
int VarTable_insert(const char *cVal){
	VarNode *cur;
	if(g_VarTable==NULL){
		g_VarTable = malloc(sizeof(VarNode));
		cur = g_VarTable;
		cur->next = NULL;
	}else{
		/*insert tot head of list*/
		cur = malloc(sizeof(VarNode));
		cur->next = g_VarTable;
		g_VarTable = cur;
	}
	strcpy(cur->cVal, cVal);
	cur->var_no = ++VarNo;
	return 0;
}

translate_EXP(){
	
}

/*traverse InterCodes list and print*/
InterCodes_DFS(){
	
}


