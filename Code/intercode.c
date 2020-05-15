#include "myhead.h"
InterCodes *g_CodesHead=NULL, *g_CodesTail=NULL;
VarNode *g_VarHead=NULL, *g_VarTail=NULL;//global table for variable search
int g_VarNo=0;//remember number for next variable
int g_tempno=0;//remember number for temp variable
int g_labelno=0;
/*stored operand in string type, when we print*/
char g_cresult[35], g_cop1[35], g_cop2[35], g_copy[40];
/*set nullop as default Operand*/
Operand *nullop;

InterCodes *newInterCodes(int codekind, Operand *result, Operand *op1, Operand *op2);
Operand *newOperand(int kind, int value, node_t *type, ArrNodes *arrs);
int newTemp();
extern int getTypeSize(node_t *type);//in semantic.c

int if_Operand_Address(Operand *op){
	if(op->kind==V_ADDRESS||op->kind==T_ADDRESS)
		return 1;
	else return 0;
}

/*gen ASSIGN code, 
**for Exp ASSIGN Exp, and VarDec ASSIGN Exp
*/
Operand *assign_Var(Operand *left, Operand *right){
	//never assign value for structure
	myassert(left->type==NULL && right->type==NULL);
	int addflg1 = if_Operand_Address(left);
	int addflg2 = if_Operand_Address(right);
	/*four situations for (addflg1, addflg2):
	**(1,1), (1,0), (0,1), (0,0)
	**(1,1) for case x=a[i]=b[j]=y or a[]=b[] #now no this case
	**(1,0) is the case a[i] = x
	**(0,1) is because x = a[i] = b[j]
	*/
	newInterCodes(ASSIGN, left, right, nullop);
	if(addflg1 && addflg2){//(1,1)
		/*two address assign*/
		Operand *consop = newOperand(CONSTANT, 4, NULL, NULL);
		//myassert(ArrNodes_cmp(left->arrs, right->arrs)==0);
		/*when x=a[i]=b[j]=y, Exp of a and Exp of b 
		** will return address, and only needs assign one byte
		*/
		if(left->arrs==NULL){
			return left;
		}
		/*assign for array*/
		//compute size of byte
		int size1 = (left->arrs->arr.asize * left->arrs->arr.usize)/4;
		int size2 = (right->arrs->arr.asize * right->arrs->arr.usize)/4;
		int size = size1<size2 ? size1:size2;
		//create temp address to store left and right
		int t1=newTemp(); int t2 = newTemp();
		Operand *t1op = newOperand(T_ADDRESS, t1, left->type, left->arrs);
		Operand *t2op = newOperand(T_ADDRESS, t2, right->type, right->arrs);
		newInterCodes(ADD_ASSIGN, t1op, left, nullop);
		newInterCodes(ADD_ASSIGN, t2op, right, nullop);
		//traverse byte and assign
		for(int i=1; i<size; i++){
			newInterCodes(ADD, t1op, t1op, consop);
			newInterCodes(ADD, t2op, t2op, consop);
			newInterCodes(ASSIGN, t1op, t2op, nullop);
		} 
		free(t1op); free(t2op); free(consop);
	}
	/*change (1,0) into (0,0)
	otherwise (a[i]=x)+(b[i]=y) will error*/
	Operand *ret = left;
	if(addflg1 && addflg2==0){
		//printf("1\n");
		int t1 = newTemp();
		ret = newOperand(TEMP, t1, NULL, NULL);
		newInterCodes(ASSIGN, ret, left, nullop);
	}
	return ret;
}

/*Description: insert variable into table
Args: cVal is the identifier name
*/
int VarTable_insert(const char *cVal, sym_t *sym){
	VarNode *cur;
	if(g_VarHead==NULL){
		myassert(g_VarTail==NULL);
		g_VarHead = g_VarTail = malloc(sizeof(VarNode));
		cur = g_VarHead;
	}
	else{
		/*insert tot head of list*/
		cur = malloc(sizeof(VarNode));
		g_VarTail->next = cur;
		g_VarTail = cur;
	}
	cur->var_no = ++g_VarNo;
	cur->sym = sym;
	cur->next = NULL;
	return 0;
}

/*
description: search VarTable
return: when not find target, return -1
*/
VarNode *VarTable_search(const char *cVal){
	VarNode *cur = g_VarHead;
	while(cur->next!=NULL){
		if(strcmp(cur->sym->cVal, cVal)==0)
			return cur;
		cur = cur->next;
	}
	if(strcmp(cur->sym->cVal, cVal)==0)
		return cur;
	return NULL;
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
InterCodes *newInterCodes(int codekind, Operand *result, Operand *op1, Operand *op2){
	InterCodes *cur = malloc(sizeof(InterCodes));
	cur->code.kind = codekind;
	cur->prev = cur->next = NULL;
	cur->code.result = *result;
	cur->code.op1 = *op1;
	cur->code.op2 = *op2;
	addCodesTail(cur);
	return cur;
}

/*new and initial Operand structure*/
Operand *newOperand(int kind, int value, node_t *type, ArrNodes *arrs){
	Operand *node = malloc(sizeof(Operand));
	node->kind = kind;
	node->u.value = value;
	node->type = type;
	node->arrs = arrs;
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
Args: 
**cur: the current node, 
**place:  the label of temp variable
*/
Operand *translate_Exp(node_t *cur, int place){
	pf3(Exp);
	node_t *child = cur->child;
	node_t *cbro = child->bro;
	Operand *ret = NULL;
	int var_no = -1;
	if(child->syntype==myINT)
	{
		pf3(ExpINT);
		//printf("%d\n",cur->lineno);
		ret = newOperand(CONSTANT, child->iVal, NULL, NULL);
		//printf("2\n");
	}
	else if(child->syntype == myID)
	{
		pf3(ExpID);
		if(cbro==NULL)
		{//ID
			pf3(ExpID1);
			VarNode *varnode = VarTable_search(child->cVal);
			myassert(varnode->var_no>0);
			int var_no = varnode->var_no;
			node_t *type = varnode->sym->detail;
			ArrNodes *arrs = varnode->sym->arrs;
			if(type!=NULL || arrs!=NULL)
			{//ret address
				if(varnode->sym->argflg==0){//variable store space
					ret = newOperand(T_ADDRESS, place, type, arrs);
					Operand *op1 = newOperand(VARIABLE, var_no, type, arrs);
					newInterCodes(ADD_ASSIGN, ret, op1, nullop);
					free(op1);
				}
				else
				{//variable store address
					ret = newOperand(V_ADDRESS, var_no, type, arrs);
				}
			}
			else
			{//ret variable
				ret = newOperand(VARIABLE, var_no, NULL, NULL);
			}
			//printf("%s--%d\n",child->cVal,var_no);
		}
		else if(cbro->bro->syntype==myRP)
		{//ID LP RP 
			pf3(ExpID2);
			if(strcmp(child->cVal, "read")==0)
			{
				ret = newOperand(TEMP, place, NULL, NULL);
				newInterCodes(READ, ret, nullop, nullop);
			}
			else
			{
				Operand *op1 = newOperand(NAME, KIND, NULL, NULL);
				ret = newOperand(TEMP, place, NULL, NULL);
				InterCodes *codes = newInterCodes(CALL, ret, op1, nullop);
				strcpy(codes->code.op1.u.cVal, child->cVal);
				free(op1);
			}
		}
		else
		{//ID LP Args RP
			if(strcmp(child->cVal, "write")==0)
			{
				Operand *result = translate_Exp(cbro->bro->child, place);
				newInterCodes(WRITE, result, nullop, nullop);
				free(result);
				ret = newOperand(CONSTANT, 0 ,NULL, NULL);
			}
			else
			{
				Operands *ops = translate_Args(cbro->bro);
				while(ops!=NULL)
				{
					newInterCodes(ARG, &(ops->op), nullop, nullop);
					Operands *p = ops;
					ops = ops->next;
					free(p);
				}
				Operand *op1 = newOperand(NAME, KIND, NULL, NULL);
				strcpy(op1->u.cVal, child->cVal);
				ret = newOperand(TEMP, place, NULL, NULL);
				InterCodes *codes = newInterCodes(CALL, ret, op1, nullop);
				strcpy(codes->code.op1.u.cVal, child->cVal);
				free(op1);
			}
		}
	}
	else if(child->syntype == myLP)
	{
		pf3(ExpLP);
		ret = translate_Exp(child->bro, place);
	}
	else if(child->syntype == myMINUS)
	{
		pf3(ExpMINUS);
		int t1 = newTemp();
		Operand *op1 = newOperand(CONSTANT, 0, NULL, NULL);
		Operand *op2 = translate_Exp(cbro, t1);
		ret = newOperand(TEMP, place, NULL, NULL);
		newInterCodes(SUB, ret, op1, op2);
		free(op2); free(op1);
	}
	else if(child->syntype == myExp || child->syntype==myNOT)
	{
		pf3(ExpExp);
		if(cbro->syntype==myPLUS || cbro->syntype==myMINUS 
				|| cbro->syntype==mySTAR || cbro->syntype==myDIV)
		{
			int t1=newTemp(); int t2=newTemp();
			Operand *op1 = translate_Exp(child, t1);
			Operand *op2 = translate_Exp(cbro->bro, t2);
			ret = newOperand(TEMP, place, NULL, NULL);
			switch(cbro->syntype)
			{
				case(myPLUS):
					newInterCodes(ADD, ret, op1, op2);
					break;
				case(myMINUS):
					newInterCodes(SUB, ret, op1, op2);
					break;
				case(mySTAR):
					newInterCodes(MUL, ret, op1, op2);
					break;
				case(myDIV):
					newInterCodes(DIV_L3, ret, op1, op2);
					break;
			}
			free(op1); free(op2);
		}
		else if(cbro->syntype==myASSIGNOP)
		{
			int t1 = newTemp();
			Operand *result = translate_Exp(child, place);
			Operand *op1 = translate_Exp(cbro->bro, t1);
			ret = assign_Var(result, op1);
			free(op1);
		}
		else if(cbro->syntype==myLB||cbro->syntype==myDOT)
		{
			int t1 = newTemp();
			Operand *left = translate_Exp(child, t1);
			Operand *biasop = NULL;
			//arrs for final Operand we ret
			ArrNodes *nxtarrs = NULL;
			//type for final Operand we ret
			node_t *type = NULL;
			//judge if return basic or address type
			int baseflg = 0;
			
			//switch LB and DOT
			if(cbro->syntype==myLB)
			{//Exp LB Exp RB
				myassert(left->arrs!=NULL);
				int t2 = newTemp();
				Operand *right = translate_Exp(cbro->bro, t2);
				int usize = left->arrs->arr.usize;
				/*compute bias of array*/
				int t3 = newTemp();
				Operand *consop =newOperand(CONSTANT, usize,NULL,NULL);
				biasop = newOperand(TEMP, t3, NULL, NULL);
				//gen bias code
				newInterCodes(MUL, biasop, right, consop);
				free(right); free(consop);
				type = left->type;
				nxtarrs = left->arrs->next;
			}
			else
			{//Exp DOT ID
				//myassert(0);
				myassert(left->type!=NULL && left->arrs==NULL);
				/*compute bias of structure*/
				int bias = getFieldBias(left->type, cbro->bro->cVal);
				node_t *field = search_field(cbro->bro->cVal, left->type);
				biasop = newOperand(CONSTANT, bias, NULL, NULL);
				type = field->detail;
				nxtarrs = field->arrs;
			}
			//merge mode for LB and DOT
			/*the ret variable is not an array 
			**and not a structure*/
			if(type==NULL && nxtarrs==NULL)
				baseflg = 1;
			//exception situation, father Exp like a[i]=x
			if(cur->bro!=NULL)
				if(cur->bro->syntype==myASSIGNOP)
					baseflg = 0;
			if(baseflg)
			{//ret basic variable
				int t = newTemp();
				Operand *op = newOperand(T_ADDRESS, t, NULL, NULL);
				//ptr + bias
				newInterCodes(ADD, op, left, biasop);
				//t1 = *t0, change ptr to real value
				ret = newOperand(TEMP, place, NULL, NULL);
				newInterCodes(ASSIGN, ret, op, nullop);
				free(op);
			}
			else
			{//ret address variable
				ret = newOperand(T_ADDRESS, place, type, nxtarrs);
				newInterCodes(ADD, ret, left, biasop);
			}
			free(biasop); free(left);
		}
		else if(cbro->syntype==myRELOP 
				||cbro->syntype==myAND || cbro->syntype==myOR
				||child->syntype==myNOT)
		{
			int label1 = newLabel();int label2 = newLabel();
			ret = newOperand(TEMP, place, NULL, NULL);
			Operand *op;
			op = newOperand(CONSTANT, 0, NULL, NULL);
			newInterCodes(ASSIGN, ret, op, nullop);
			free(op);
			translate_Cond(cur, label1, label2);
			op = newOperand(LABEL, label1, NULL, NULL);
			newInterCodes(LABEL_DEF, op, nullop, nullop);
			free(op);
			op = newOperand(CONSTANT, 1, NULL, NULL);
			newInterCodes(ASSIGN, ret, op, nullop);
			free(op);
			op = newOperand(LABEL, label2, NULL, NULL);
			newInterCodes(LABEL_DEF, op, nullop, nullop);
			free(op);
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
	pf3(Args);
	node_t *child = cur->child;
	int t1 = newTemp();
	Operand *op = translate_Exp(child, t1);
	Operands *ops1 = newOperands(op);
	free(op);
	node_t *cbro = child->bro;
	if(cbro!=NULL){//cbro: COMMA
		Operands *ops2 = translate_Args(cbro->bro);
		/*notice that Args add to stack 
		  in the opposite direction against Param*/
		ConnectOperands(ops2, ops1);
		return ops2;
	}
	return ops1;
}

/*translate code for conditional expression*/
void translate_Cond(node_t *cur, int label_true, int label_false){
	pf3(Cond);
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
			Operand *labelop = newOperand(LABEL, label_true, NULL, NULL);
			newInterCodes(kind, labelop, op1, op2);
			labelop->u.value = label_false;
			newInterCodes(JMP, labelop, nullop, nullop);
			free(op1); free(op2); free(labelop);
			return;
		}
		else if(cbro->syntype==myAND){
			int label1 = newLabel();
			translate_Cond(child, label1, label_false);
			Operand *labelop = newOperand(LABEL, label1, NULL, NULL);
			newInterCodes(LABEL_DEF, labelop, nullop,nullop);
			free(labelop);
			translate_Cond(cbro->bro, label_true, label_false);
			return;
		}
		else if(cbro->syntype==myOR){
			int label1 = newLabel();
			translate_Cond(child, label_true, label1);
			Operand *labelop = newOperand(LABEL, label1, NULL, NULL);
			newInterCodes(LABEL_DEF, labelop, nullop,nullop);
			free(labelop);
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
	Operand *labelop = newOperand(LABEL, label_true, NULL, NULL);
	Operand *zeroop = newOperand(CONSTANT, 0, NULL, NULL);
	newInterCodes(JNE, labelop, op1, zeroop);
	labelop->u.value = label_false;
	newInterCodes(JMP, labelop, nullop, nullop);
	free(op1); free(labelop); free(zeroop);
}

void translate_CompSt(node_t *cur);
void translate_Stmt(node_t *cur){
	pf3(Stmt);
	node_t *child = cur->child;
	node_t *gchild = child->child;
	if(child->syntype==myExp){
		translate_Exp(child, newTemp());
	}
	else if(child->syntype==myCompSt){
		translate_CompSt(child);
	}
	else if(child->syntype==myRETURN){
		int t1 = newTemp();
		//printf("1\n");
		Operand *op1 = translate_Exp(child->bro, t1);
		newInterCodes(RET, op1, nullop, nullop);
		free(op1);
		//printf("2\n");
	}
	else if(child->syntype==myIF){
		if(child->bro->bro->bro->bro->bro==NULL){
			//case IF LP Exp RP Stmt
			int label1 = newLabel();
			int label2 = newLabel();
			Operand *labelop = newOperand(LABEL, label1, NULL, NULL);
			translate_Cond(child->bro->bro, label1, label2);
			newInterCodes(LABEL_DEF, labelop,nullop, nullop);
			translate_Stmt(child->bro->bro->bro->bro);
			labelop->u.value = label2;
			newInterCodes(LABEL_DEF, labelop,nullop, nullop);
			free(labelop);
		}
		else{
			int label1 = newLabel();
			int label2 = newLabel();
			int label3 = newLabel();
			Operand *labelop = newOperand(LABEL, label1, NULL, NULL);
			translate_Cond(child->bro->bro, label1, label2);
			newInterCodes(LABEL_DEF, labelop, nullop,nullop);
			translate_Stmt(child->bro->bro->bro->bro);

			labelop->u.value = label3;
			newInterCodes(JMP, labelop, nullop, nullop);

			labelop->u.value = label2;
			newInterCodes(LABEL_DEF, labelop, nullop,nullop);
			translate_Stmt(child->bro->bro->bro->bro->bro->bro);

			labelop->u.value = label3;
			newInterCodes(LABEL_DEF, labelop, nullop,nullop);
			free(labelop);
		}
	}
	else {
		//case WHILE LP Exp RP Stmt
		int label1 = newLabel();
		int label2 = newLabel();
		int label3 = newLabel();
		Operand *labelop = newOperand(LABEL, label1, NULL, NULL);
		newInterCodes(LABEL_DEF, labelop, nullop, nullop);
		translate_Cond(child->bro->bro, label2, label3);

		labelop->u.value = label2;
		newInterCodes(LABEL_DEF, labelop, nullop, nullop);
		translate_Stmt(child->bro->bro->bro->bro);

		labelop->u.value = label1;
		newInterCodes(JMP, labelop, nullop, nullop);

		labelop->u.value = label3;
		newInterCodes(LABEL_DEF, labelop, nullop, nullop);
		free(labelop);
	}
}

void translate_StmtList(node_t *cur);
void translate_DefList(node_t *cur);
void translate_CompSt(node_t *cur){
	pf3(CompSt);
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

void translate_Def(node_t *cur);
void translate_DefList(node_t *cur){
	pf3(DefList);
	node_t *child = cur->child;
	translate_Def(child);
	if(child->bro!=NULL){
		translate_DefList(child->bro);
	}
}

void translate_DecList(node_t *cur);
void translate_Def(node_t *cur){
	translate_DecList(cur->child->bro);
}

void translate_Dec(node_t *cur);
void translate_DecList(node_t *cur){
	node_t *child = cur->child;
	translate_Dec(child);
	if(child->bro!=NULL){
		translate_DecList(child->bro->bro);
	}
}

Operand *translate_VarDec(node_t *cur);
void translate_Dec(node_t *cur){
	node_t *child = cur->child;//VarDec
	Operand *op1 = translate_VarDec(child);	
	if(op1->arrs!=NULL || op1->type!=NULL){
		int size;
		if(op1->arrs==NULL) {
			size = getTypeSize(op1->type);
			//printf("%d\n",size);
		}
		else size = op1->arrs->arr.asize * op1->arrs->arr.usize;

		Operand *consop = newOperand(CONSTANT, size, NULL, NULL);
		newInterCodes(DEC_L3, op1, consop, nullop);
		free(consop);
	}
	if(child->bro!=NULL){
		//child->bro: ASSIGNOP
		Operand *op2 = translate_Exp(child->bro->bro, newTemp());
		if(op1->arrs!=NULL || op1->type!=NULL){
			myassert(op1->arrs==NULL);//array can't assign at initial
			Operand *left = newOperand(T_ADDRESS, newTemp(), op1->type, op1->arrs);
			newInterCodes(ADD_ASSIGN, left, op1, nullop);
			assign_Var(left, op2);
			free(left);
		}else{
			assign_Var(op1, op2);
		}
		free(op2);
	}
	free(op1);
}

void translate_StmtList(node_t *cur){
	pf3(StmtList);
	node_t *child = cur->child;
	translate_Stmt(child);
	//pf3(StmtListend);
	if(child->bro!=NULL){
		translate_StmtList(child->bro);
	}
}

void translate_VarList(node_t *cur);
void translate_FunDec(node_t *cur){
	pf3(FunDec);
	node_t *child = cur->child;//ID
	/*insert FUNCTION name:*/
	//printf("1\n");
	Operand *op = newOperand(NAME, KIND, NULL, NULL);
	InterCodes *codes = newInterCodes(FUN, op,nullop,nullop);
	//printf("2\n");
	free(op);
	strcpy(codes->code.result.u.cVal, child->cVal);
	/*define param*/
	if(child->bro->bro->syntype==myVarList)
		translate_VarList(child->bro->bro);
}

void translate_ParamDec(node_t *cur);
void translate_VarList(node_t *cur){
	pf3(VarList);
	node_t *child = cur->child;//ParamDec
	node_t *cbro = child->bro;
	translate_ParamDec(child);
	if(cbro!=NULL){
		translate_VarList(cbro->bro);
	}
}

void translate_ParamDec(node_t *cur){
	node_t *cbro = cur->child->bro;
	Operand *op = translate_VarDec(cbro);
	newInterCodes(PARAM, op, nullop, nullop);
	free(op);
}

/*according to the cVal of VarDec node,
 **get the Operand we need
 */
Operand *translate_VarDec(node_t *cur){
	/*the cVal of VarDec is the same as ID
	 **Because we copy when call VarDec_DFS in semantic.c*/
	VarNode *varnode = VarTable_search(cur->cVal);
	int var_no = varnode->var_no;
	node_t *detail = varnode->sym->detail;
	ArrNodes *arrs = varnode->sym->arrs;
	Operand *op =newOperand(VARIABLE, var_no, detail, arrs);
	return op;
}

extern node_t *root;
void translate_ExtDefList(node_t *cur);
void InterCodes_DFS(char *filename);
void translate_root(char *filename){
	nullop = newOperand(KIND, KIND, NULL, NULL);
	if(root->child!=NULL)
		translate_ExtDefList(root->child);
	//pf3(sdfsd);
	InterCodes_DFS(filename);
	free(nullop);
	//pf3(3);
}

void translate_ExtDef(node_t *cur);
void translate_ExtDefList(node_t *cur){
	node_t *child = cur->child;
	node_t *cbro = child->bro;
	translate_ExtDef(child);
	if(cbro!=NULL){
		translate_ExtDefList(cbro);
	}
}

void translate_ExtDef(node_t *cur){
	node_t *child = cur->child;//Specifier
	//myassert(child->bro->syntype==myFunDec);
	if(child->bro->syntype==myFunDec){
		translate_FunDec(child->bro);
		myassert(child->bro->bro->syntype==myCompSt);
		translate_CompSt(child->bro->bro);
	}
}

void pt_InterCodes(FILE *fp, InterCodes *codes);
/*traverse InterCodes list and print*/
void InterCodes_DFS(char *filename){
	FILE *fp = fopen(filename, "w");
	InterCodes *codes = g_CodesHead;
	int i = 0;
	while(codes!=NULL){
		//printf("%d\n",i++);
		pt_InterCodes(fp, codes);
		codes = codes->next;
	}
	//pf3(1);
	fclose(fp);
	//pf3(2);
}

void pt_Operand(Operand *op, char *dst);
void pt_InterCodes(FILE *fp, InterCodes *codes){
	Operand *left = &(codes->code.result);
	Operand *op1 = &(codes->code.op1);
	Operand *op2 = &(codes->code.op2);
	pt_Operand(left, g_cresult);
	pt_Operand(op1, g_cop1);
	pt_Operand(op2, g_cop2);
	int addflg1, addflg2, addflg3;
	addflg1 = if_Operand_Address(left);
	addflg2 = if_Operand_Address(op1);
	addflg3 = if_Operand_Address(op2);
	switch(codes->code.kind){
		case ASSIGN:
			if(addflg1){
				sprintf(g_copy, "*%s", g_cresult);
				strcpy(g_cresult, g_copy);
			}
			if(addflg2){
				sprintf(g_copy, "*%s", g_cop1);
				strcpy(g_cop1, g_copy);
			}
			fprintf(fp, "%s := %s\n", g_cresult, g_cop1);
			break;
		case ADD_ASSIGN:
			myassert(addflg1==1);
			if(addflg2==0){
				sprintf(g_copy, "&%s", g_cop1);
				strcpy(g_cop1, g_copy);
			}
			fprintf(fp, "%s := %s\n", g_cresult, g_cop1);
			break;
		case ADD:
		case SUB:
		case MUL:
		case DIV_L3:
			//myassert(addflg1==addflg2 && addflg2==addflg3);
			/*if(addflg1==0){
				sprintf(g_copy, "&%s", g_cop1);
				strcpy(g_cop1, g_copy);
			}*/
			fprintf(fp, "%s := %s %s %s\n", g_cresult, g_cop1, g_SignName[codes->code.kind], g_cop2);
			break;
		case LABEL_DEF:
			fprintf(fp, "LABEL %s :\n", g_cresult);
			break;
		case JMP:
			fprintf(fp, "GOTO %s\n", g_cresult);
			break;
		case JL:
		case JG:
		case JLE:
		case JGE:
		case JE:
		case JNE:
			fprintf(fp, "IF %s %s %s GOTO %s\n", g_cop1, g_SignName[codes->code.kind], g_cop2, g_cresult);
			break;
		case DEC_L3:
			fprintf(fp, "DEC %s %s\n", g_cresult, &(g_cop1[1]));
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
			if(left->type==NULL && left->arrs==NULL){
				fprintf(fp, "ARG %s\n", g_cresult);
			}else{
				if(addflg1){
					fprintf(fp, "ARG %s\n", g_cresult);
				}else{
					myassert(0);//not yet
					fprintf(fp, "ARG &%s\n", g_cresult);
				}
			}
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
		default: 
			myassert(0);
			break;
	}
	//printf("%s\n",g_cresult);
}

void pt_Operand(Operand *op, char *dst){
	switch(op->kind){
		case VARIABLE:
		case V_ADDRESS:
			sprintf(dst, "v%d", op->u.value);
			break;
		case CONSTANT:
			sprintf(dst, "#%d", op->u.value);
			break;
		case TEMP:
		case T_ADDRESS:
			sprintf(dst, "t%d", op->u.value);
			break;
		case LABEL:
			sprintf(dst, "label%d", op->u.value);
			break;
		case NAME:
			sprintf(dst, "%s", op->u.cVal);
			break;
		case KIND: 
			break;
		default: myassert(0);
				 break;
	}
}
