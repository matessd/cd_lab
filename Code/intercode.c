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

/*
description: search VarTable
return: when not find target, return -1
*/
int VarTable_search(const char *cVal){
	VarNode *cur = g_VarTable;
	while(cur->next!=NULL){
		if(strcmp(cur->cVal, cVal)==0)
			return cur->var_no;
		cur = cur->next;
	}
	if(strcmp(cur->cVal, cVal)==0)
		return cur->var_no;
	return -1;
}

/*new a InterCodes and return pointer*/
InterCodes *newInterCodes(int codekind, int result, int resultkind, int opvalue1, int kind1, int opvalue2, int kind2){
	InterCodes *cur = malloc(sizeof(InterCodes));
	cur->code.kind = codekind;
	cur->prev = NULL;
	cur->next = NULL;
	cur->code.result.kind = resultkind;
	cur->code.result.u.value = result;
	cur->code.result.kind = kind1;
	cur->code.result.u.value = opvalue1;
	cur->code.result.kind = kind2;
	cur->code.result.u.value = opvalue2;
	return cur;
}

/*
Description: translate mode of Exp Node
Args: cur is the current node, 
	place is the label of temp variable
*/
InterCodes *translate_EXP(node_t *cur, int place){
	node_t *child = cur->child;
	InterCodes *ret = NULL;
	int var_no = -1;
	switch(child->syntype){
	  case myINT:
		ret = newInterCodes(ASSIGN, place, TEMP, child->iVal, CONSTANT, -1, -1);
		break;
	  case myID:
		var_no = VarTable_search(child->cVal);
		myassert(var_no>0);
		ret = newInterCodes(ASSIGN, place, TEMP, var_no, VARIABLE, -1, -1);
		break;
	}
	return ret;
}

/*traverse InterCodes list and print*/
void InterCodes_DFS(){
			
}



