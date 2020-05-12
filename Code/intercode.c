#include "myhead.h"
InterCodes *g_CodesHead=NULL, *g_CodesTail=NULL;
VarNode *g_VarTable=NULL;//global table for variable search
int g_VarNo=0;//remember number for next variable
int g_tempno=0;//remember number for temp variable
int g_labelno=0;
/*stored operand in string type, when we print*/
char g_cresult[35], g_cop1[35], g_cop2[35];

/*Description: insert variable into table
Args: cVal is the identifier name
*/
int VarTable_insert(const char *cVal){
	VarNode *cur;
	if(g_VarTable==NULL){
		g_VarTable = malloc(sizeof(VarNode));
		cur = g_VarTable;
		cur->next = NULL;
	}
	else{
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

void addCodesTail(InterCodes *codes){
	if(g_CodesTail==NULL){
		g_CodesHead = g_CodesTail = codes;
	}else{
		g_CodesTail->next = codes;
		g_CodesTail = codes;
	}
}

/*new a InterCodes and return pointer
  and add it to the global InterCodes list*/
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
	addCodesTail(cur);
	return cur;
}

/*new and initial Operand structure*/
Operand *newOperand(int kind, int value){
	Operand *node = malloc(sizeof(Operand));
	node->kind = kind;
	node->u.value = value;
	return node;
}

/*new a Operands structure*/
Operands *newOperands(Operand *op){
	Operands *ops = malloc(sizeof(Operands));
	ops->op = *op;
	ops->prev = ops->next = NULL;
	return ops;
}

/*Description: add p2 to the tail of p1*/
Operands *ConnectOperands(Operands *p1, Operands *p2){
	while(p1->next!=NULL){
		p1=p1->next;
	}
	p1->next = p2;
}

#define MAX_TEMPNO 100001
int newTemp(){
	return (++g_tempno)%MAX_TEMPNO;
}

int newLabel(){
	return ++g_labelno;
}

void translate_Cond(node_t *cur, int label_true, int label_false);
Operands *translate_Args(node_t *cur);

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
	if(child->syntype==myINT)
	{
		ret = newOperand(CONSTANT, child->iVal);
	}
	else if(child->syntype == myID)
	{
		if(cbro==NULL)
		{//ID
			var_no = VarTable_search(child->cVal);
			myassert(var_no>0);
			ret = newOperand(VARIABLE, var_no);
		}
		else if(cbro->bro->syntype==myRP)
		{//ID LP RP 
			if(strcmp(child->cVal, "read")==0)
			{
				newInterCodes(READ, place, TEMP, -1, -1, -1,-1);
			}
			else
			{
				InterCodes *codes = newInterCodes(CALL, place, TEMP, -1, NAME, -1,-1);
				strcpy(codes->code.op1.u.cVal, child->cVal);
			}
		}
		else
		{//ID LP Args RP
			if(strcmp(child->cVal, "write")==0)
			{
				Operand *result = translate_Exp(cbro->bro->child, place);
				newInterCodes(WRITE, result->u.value, result->kind, -1,-1,-1,-1);
				free(result);
			}
			else
			{
				Operands *ops = translate_Args(cbro->bro);
				while(ops!=NULL)
				{
					newInterCodes(ARG, ops->op.u.value, ops->op.kind, -1,-1,-1,-1);
					Operands *p = ops;
					ops = ops->next;
					free(p);
				}
				InterCodes *codes = newInterCodes(CALL, place, TEMP, -1, NAME, -1,-1);
				strcpy(codes->code.op1.u.cVal, child->cVal);
			}
		}
	}
	else if(child->syntype == myLP)
	{
		ret = translate_Exp(child->bro, place);
	}
	else if(child->syntype == myMINUS)
	{
		int t1 = newTemp();
		Operand *op2 = translate_Exp(cbro, t1);
		newInterCodes(SUB, place, TEMP, 0, CONSTANT, op2->u.value, op2->kind);
		free(op2);
		ret = newOperand(TEMP, place);
	}
	else if(child->syntype == myExp || child->syntype==myNOT)
	{
		if(cbro->syntype==myPLUS || cbro->syntype==myMINUS 
				|| cbro->syntype==mySTAR || cbro->syntype==myDIV)
		{
			int t1=newTemp(); int t2=newTemp();
			Operand *op1 = translate_Exp(child, t1);
			Operand *op2 = translate_Exp(cbro->bro, t2);
			switch(cbro->syntype)
			{
				case(myPLUS):
					newInterCodes(ADD, place, TEMP, op1->u.value, op1->kind, op2->u.value, op2->kind);
					break;
				case(myMINUS):
					newInterCodes(SUB, place, TEMP, op1->u.value, op1->kind, op2->u.value, op2->kind);
					break;
				case(mySTAR):
					newInterCodes(MUL, place, TEMP, op1->u.value, op1->kind, op2->u.value, op2->kind);
					break;
				case(myDIV):
					newInterCodes(DIV_L3, place, TEMP, op1->u.value, op1->kind, op2->u.value, op2->kind);
					break;
			}
			free(op1); free(op2);
			ret = newOperand(TEMP, place);
		}
		else if(cbro->syntype==myASSIGNOP)
		{
			int t1 = newTemp(); int t2 = newTemp();
			Operand *result = translate_Exp(cbro->bro, t1);
			Operand *op1 = translate_Exp(cbro->bro, t2);
			newInterCodes(ASSIGN, result->u.value, result->kind, op1->u.value, op1->kind, -1, -1);
			free(result); free(op1);
			ret = newOperand(TEMP, place);
			//TODO
		}
		else if(cbro->syntype==myLB||cbro->syntype==myDOT)
		{
			//TODO
		}
		else if(cbro->syntype==myRELOP 
				||cbro->syntype==myAND || cbro->syntype==myOR
				||child->syntype==myNOT)
		{
			int label1 = newLabel();int label2 = newLabel();
			newInterCodes(ASSIGN, place, TEMP, 0, CONSTANT, -1, -1);
			translate_Cond(cur, label1, label2);
			newInterCodes(LABEL_DEF, label1, TEMP, -1, -1, -1, -1);
			newInterCodes(ASSIGN, place, TEMP, 1, CONSTANT, -1, -1);
			newInterCodes(LABEL_DEF, label2, TEMP, -1, -1, -1, -1);
			ret = newOperand(TEMP, place);
		}
		else{
			myassert(0);
		}
	}
	else{
		myassert(0);
	}
	return ret;
}

/*translate Args and return arg_list*/
Operands *translate_Args(node_t *cur){
	node_t *child = cur->child;
	int t1 = newTemp();
	Operand *op = translate_Exp(child, t1);
	Operands *ops1 = newOperands(op);
	free(op);
	node_t *cbro = child->bro;
	if(cbro!=NULL){//cbro: COMMA
		Operands *ops2 = translate_Args(cbro->bro);
		ConnectOperands(ops1, ops2);
	}
	return ops1;
}

/*translate code for conditional expression*/
void translate_Cond(node_t *cur, int label_true, int label_false){
	node_t *child = cur->child;
	node_t *cbro = child->bro;
	if(child->syntype==myExp){
		if(cbro->syntype==myRELOP){
			int t1=newTemp(); int t2=newTemp();
			Operand *op1 = translate_Exp(child, t1);
			Operand *op2 = translate_Exp(cbro->bro, t2);
			int kind=-1;
			if(strcmp(cbro->cVal,"<")==0){
				kind = JL;
			}else if(strcmp(cbro->cVal,">")==0){
				kind = JG;
			}else if(strcmp(cbro->cVal,"<=")==0){
				kind = JLE;
			}else if(strcmp(cbro->cVal,">=")==0){
				kind = JGE;
			}else if(strcmp(cbro->cVal,"==")==0){
				kind = JE;
			}else if(strcmp(cbro->cVal,"!=")==0){
				kind = JNE;
			}else{
				myassert(0);
			}
			newInterCodes(kind, label_true, LABEL, op1->u.value, op1->kind, op2->u.value, op2->kind);
			newInterCodes(JMP, label_false, LABEL,-1,-1,-1,-1);
			free(op1); free(op2);
			return;
		}
		else if(cbro->syntype==myAND){
			int label1 = newLabel();
			translate_Cond(child, label1, label_false);
			newInterCodes(LABEL_DEF, label1, LABEL, -1,-1,-1,-1);
			translate_Cond(cbro->bro, label_true, label_false);
			return;
		}
		else if(cbro->syntype==myOR){
			int label1 = newLabel();
			translate_Cond(child, label_true, label1);
			newInterCodes(LABEL_DEF, label1, LABEL, -1,-1,-1,-1);
			translate_Cond(cbro->bro, label_true, label_false);
			return;
		}
		//if not above case, goto other case
	}else if(child->syntype==myNOT){
		translate_Cond(cbro, label_false, label_true);
		return;
	}
	/*other case, like Exp PLUS Exp*/
	int t1 = newTemp();
	Operand *op1 = translate_Exp(cur, t1);
	newInterCodes(JNE, label_true, LABEL, op1->u.value, op1->kind, 0, CONSTANT);
	newInterCodes(JMP, label_false, LABEL, -1,-1,-1,-1);
	free(op1);
}

void translate_CompSt(node_t *cur);
void translate_Stmt(node_t *cur){
	node_t *child = cur->child;
	node_t *gchild = child->child;
	if(child->syntype==myExp){
		if(gchild->syntype!=myExp && gchild->syntype!=myID)
			return;
		else if(gchild->bro->syntype!=myASSIGNOP && gchild->bro->syntype!=myLP)
			return;
		translate_Exp(child, -1);
	}
	else if(child->syntype==myCompSt){
		translate_CompSt(child);
	}
	else if(child->syntype==myRETURN){
		int t1 = newTemp();
		Operand *op1 = translate_Exp(child->bro, t1);
		newInterCodes(RET, t1, TEMP, -1,-1,-1,-1);
		free(op1);
	}
	else if(child->syntype==myIF){
		if(child->bro->bro->bro->bro->bro==NULL){
			//case IF LP Exp RP Stmt
			int label1 = newLabel();
			int label2 = newLabel();
			translate_Cond(child->bro->bro, label1, label2);
			newInterCodes(LABEL_DEF, label1, LABEL,-1,-1,-1,-1);
			translate_Stmt(child->bro->bro->bro->bro);
			newInterCodes(LABEL_DEF, label2, LABEL,-1,-1,-1,-1);
		}
		else{
			int label1 = newLabel();
			int label2 = newLabel();
			int label3 = newLabel();
			translate_Cond(child->bro->bro, label1, label2);
			newInterCodes(LABEL_DEF, label1, LABEL,-1,-1,-1,-1);
			translate_Stmt(child->bro->bro->bro->bro);
			newInterCodes(JMP, label3, LABEL,-1,-1,-1,-1);
			newInterCodes(LABEL_DEF, label2, LABEL,-1,-1,-1,-1);
			translate_Stmt(child->bro->bro->bro->bro->bro->bro);
			newInterCodes(LABEL_DEF, label3, LABEL,-1,-1,-1,-1);
		}
	}
	else {
		//case WHILE LP Exp RP Stmt
		int label1 = newLabel();
		int label2 = newLabel();
		int label3 = newLabel();
		newInterCodes(LABEL_DEF, label1, LABEL,-1,-1,-1,-1);
		translate_Cond(child->bro->bro, label2, label3);
		newInterCodes(LABEL_DEF, label2, LABEL,-1,-1,-1,-1);
		translate_Stmt(child->bro->bro->bro->bro);
		newInterCodes(JMP, label1, LABEL,-1,-1,-1,-1);
		newInterCodes(LABEL_DEF, label3, LABEL,-1,-1,-1,-1);
	}
}

void translate_StmtList(node_t *cur);
void translate_DefList(node_t *cur);
void translate_CompSt(node_t *cur){
	node_t *child = cur->child;//LC
	node_t *cbro = child->bro;
	if(cbro->syntype==myDefList){
		translate_DefList(cbro);
		if(cbro->bro->syntype==myStmtList)
			translate_StmtList(cbro->bro);
	}
	else if(cbro->syntype==myStmtList){
		translate_StmtList(cbro);
	}
}

void translate_DefList(node_t *cur){
	//TODO
}

void translate_StmtList(node_t *cur){
	node_t *child = cur->child;
	translate_Stmt(child);
	if(child->bro!=NULL){
		translate_StmtList(child->bro);
	}
}

void translate_VarList(node_t *cur);
void translate_FunDec(node_t *cur){
	node_t *child = cur->child;//ID
	/*insert FUNCTION name:*/
	InterCodes *codes = malloc(sizeof(InterCodes));
	codes->prev = codes->next = NULL;
	codes->code.kind = FUN;
	codes->code.result.kind = NAME;
	strcpy(codes->code.result.u.cVal, child->cVal);
	addCodesTail(codes);
	/*define param*/
	if(child->bro->bro->syntype==myVarList)
		translate_VarList(child->bro->bro);
}

void translate_ParamDec(node_t *cur);
void translate_VarList(node_t *cur){
	node_t *child = cur->child;//ParamDec
	node_t *cbro = child->bro;
	translate_ParamDec(child);
	if(cbro!=NULL){
		translate_VarList(cbro->bro);
	}
}

Operand *translate_VarDec(node_t *cur);
void translate_ParamDec(node_t *cur){
	node_t *cbro = cur->child->bro;
	Operand *op = translate_VarDec(cbro);
	newInterCodes(PARAM, op->u.value, op->kind,-1,-1,-1,-1);
	free(op);
}

/*according to the cVal of VarDec node,
**get the Operand we need
*/
Operand *translate_VarDec(node_t *cur){
	/*the cVal of VarDec is the same as ID
	**Because we copy when call VarDec_DFS in semantic.c*/
	int var_no = VarTable_search(cur->cVal);
	Operand *op =newOperand(VARIABLE, var_no);
	return op;
}

extern node_t *root;
void translate_subTree(node_t *cur);
void translate_root(){
	translate_subTree(root);
}

void translate_subTree(node_t *cur){
	if(cur->syntype==myFunDec){
		translate_FunDec(cur);
		translate_CompSt(cur->bro);
	}
}

void pt_InterCodes(FILE *fp, InterCodes *codes);
/*traverse InterCodes list and print*/
void InterCodes_DFS(char *filename){
	FILE *fp = fopen(filename, "w");
	InterCodes *codes = g_CodesHead;
	while(codes!=NULL){
		pt_InterCodes(fp, codes);
		codes = codes->next;
	}
	fclose(fp);
}

void pt_Operand(Operand *op, char *dst);
void pt_InterCodes(FILE *fp, InterCodes *codes){
	pt_Operand(&(codes->code.result), g_cresult);
	pt_Operand(&(codes->code.op1), g_cop1);
	pt_Operand(&(codes->code.op2), g_cop2);
	switch(codes->code.kind){
	  case ASSIGN:
		fprintf(fp, "%s := %s\n", g_cresult, g_cop1);
		break;
	  case ADD:
		fprintf(fp, "%s := %s + %s\n", g_cresult, g_cop1, g_cop2);
		break;
	  case SUB:
		fprintf(fp, "%s := %s - %s\n", g_cresult, g_cop1, g_cop2);
		break;
	  case MUL: 
		fprintf(fp, "%s := %s * %s\n", g_cresult, g_cop1, g_cop2);
		break;
	  case DIV_L3:
		fprintf(fp, "%s := %s * %s\n", g_cresult, g_cop1, g_cop2);
		break;
	  case LABEL_DEF:
		fprintf(fp, "LABEL %s :\n", g_cresult);
		break;
	  case JMP:
		fprintf(fp, "GOTO %s\n", g_cresult);
		break;
	  case JL:
		fprintf(fp, "IF %s < %s GOTO %s\n", g_cop1, g_cop2, g_cresult);
		break;
	  case JG:
		fprintf(fp, "IF %s > %s GOTO %s\n", g_cop1, g_cop2, g_cresult);
		break;
	  case JLE:
		fprintf(fp, "IF %s <= %s GOTO %s\n", g_cop1, g_cop2, g_cresult);
		break;
	  case JGE:
		fprintf(fp, "IF %s >= %s GOTO %s\n", g_cop1, g_cop2, g_cresult);
		break;
	  case JE: 
		fprintf(fp, "IF %s == %s GOTO %s\n", g_cop1, g_cop2, g_cresult);
		break;
	  case JNE:
		fprintf(fp, "IF %s != %s GOTO %s\n", g_cop1, g_cop2, g_cresult);
		break;
	  case RET:
		fprintf(fp, "RETURN %s\n\n", g_cresult);
		break;
	  case FUN:
		fprintf(fp, "FUNCTION %s :\n", g_cresult);
		break;
	  case CALL:
		fprintf(fp, "%s := CALL %s\n", g_cresult, g_cop1);
		break;
	  case ARG:
		fprintf(fp, "ARG %s\n", g_cresult);
		break;
	  case PARAM:
		fprintf(fp, "PARAM %s\n", g_cresult);
		break;
	  case READ:
		fprintf(fp, "READ %s\n", g_cresult);
		break;
	  case WRITE:
		fprintf(fp, "WRITE %s\n", g_cresult);
		break;
	  default: myassert(0);
		break;
	}
}

void pt_Operand(Operand *op, char *dst){
	switch(op->kind){
	  case VARIABLE:
		sprintf(dst, "v%d", op->u.value);
		break;
	  case CONSTANT:
		sprintf(dst, "#%d", op->u.value);
		break;
	  case ADDRESS:
		//TODO
		break;
	  case TEMP:
		sprintf(dst, "t%d", op->u.value);
		break;
	  case LABEL:
		sprintf(dst, "label%d", op->u.value);
		break;
	  case NAME:
		sprintf(dst, "%s", op->u.cVal);
		break;
	  case -1: 
		break;
	  default: myassert(0);
		break;
	}
}
