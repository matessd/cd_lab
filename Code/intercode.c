#include "myhead.h"
InterCodes *g_CodesHead=NULL, *g_CodesTail=NULL;
VarNode *g_VarTable=NULL;//global table for variable search
int g_VarNo=0;//remember number for next variable
int g_tempno=0;//remember number for temp variable

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
	cur->var_no = ++g_VarNo;
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

/*new and initial Operand structure*/
Operand *newOperand(int kind, int value){
	Operand *node = malloc(sizeof(Operand));
	node->kind = kind;
	node->u.value = value;
	return node;
}

void addCodesTail(InterCodes *codes){
	if(g_CodesTail==NULL){
		g_CodesHead = g_CodesTail = codes;
	}else{
		g_CodesTail->next = codes;
		g_CodesTail = codes;
	}
}

#define MAX_TEMPNO 100001
int newTemp(){
	return (++g_tempno)%MAX_TEMPNO;
}

/*
Description: translate mode of Exp Node
Args: cur is the current node, 
	place is the label of temp variable
*/
Operand *translate_Exp(node_t *cur, int place){
	node_t *child = cur->child;
	node_t *cbro = child->bro;
	Operand *ret = NULL;
	int var_no = -1;
	switch(child->syntype){
	  case myINT:
		ret = newOperand(CONSTANT, child->iVal);
		break;
	  case myID:
		var_no = VarTable_search(child->cVal);
		myassert(var_no>0);
		ret = newOperand(VARIABLE, var_no);
		break;
	  case myLP:
		ret = translate_Exp(child->bro, place);
		break;
	  case myExp:
		if(cbro->syntype==myPLUS || cbro->syntype==myMINUS 
		|| cbro->syntype==mySTAR || cbro->syntype==myDIV){
			int t1=newTemp(); int t2=newTemp();
			Operand *op1 = translate_Exp(child, t1);
			Operand *op2 = translate_Exp(cbro->bro, t2);
			InterCodes *codes = NULL;
			switch(cbro->syntype){
			  case(myPLUS):
				codes = newInterCodes(ADD, place, TEMP, op1->u.value, op1->kind, op2->u.value, op2->kind);
			  break;
			  case(myMINUS):
				codes = newInterCodes(SUB, place, TEMP, op1->u.value, op1->kind, op2->u.value, op2->kind);
			  break;
			  case(mySTAR):
				codes = newInterCodes(MUL, place, TEMP, op1->u.value, op1->kind, op2->u.value, op2->kind);
			  break;
			  case(myDIV):
				codes = newInterCodes(DIV_L3, place, TEMP, op1->u.value, op1->kind, op2->u.value, op2->kind);
			  break;
			}
			free(op1); free(op2);
			myassert(codes!=NULL);
			addCodesTail(codes);
			ret = newOperand(TEMP, place);
		}else if(cbro->syntype==myASSIGNOP){
			
		}
		break;
	  default: myassert(0);
		break;
	}
	return ret;
}

/*traverse InterCodes list and print*/
void InterCodes_DFS(){
			
}



