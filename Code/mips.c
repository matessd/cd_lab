#include"myhead.h"
static const char g_mips_head[] =
{
".data\n_prompt: .asciiz \"Enter an integer:\"\n\
_ret: .asciiz \"\n\"\n.globl main\n.text\n\
read:\n\
 li $v0, 4\n\
 la $a0, _prompt\n\
 syscall\n\
 li $v0, 5\n\
 syscall\n\
 jr $ra\n\n\
write:\n\
 li $v0, 1\n\
 syscall\n\
 li $v0, 4\n\
 la $a0, _ret\n\
 syscall\n\
 move $v0, $0\n\
 jr $ra\n"
};
int g_reg_using[8];//judge register $t0-$t7 is using
RegInfo *g_reginfo[8];

RegInfo *g_InfoHead=NULL, *g_InfoTail=NULL;
/*Description: insert reginfo into table
*/
int InfoTable_insert(Operand *op, int offset){
	RegInfo *cur;
	if(g_InfoHead==NULL){
		myassert(g_InfoTail==NULL);
		g_InfoHead = g_InfoTail = malloc(sizeof(RegInfo));
		cur = g_InfoHead;
	}
	else{
		/*insert tot head of list*/
		cur = malloc(sizeof(RegInfo));
		g_InfoTail->next = cur;
		g_InfoTail = cur;
	}
	cur->vartype = op->kind;
	cur->value = op->u.value;
	cur->offset = offset;
	cur->next = NULL;
	return 0;
}

/*
description: search InfoTable
return: when not find target, return NULL
*/
RegInfo *InfoTable_search(Operand *op){
	RegInfo *cur = g_InfoHead;
	if(g_InfoHead==NULL)
		return NULL;
	while(cur->next!=NULL){
		if(cur->vartype==op->kind && cur->value==op->u.value)
			return cur;
		cur = cur->next;
	}
	if(cur->vartype==op->kind && cur->value==op->u.value)
		return cur;
	return NULL;
}

void free_InfoTable(){
	RegInfo *cur = g_InfoHead;
	while(cur!=NULL){
		free(cur);
		cur = cur->next;
	}
	g_InfoHead = NULL;
	g_InfoTail = NULL;
}

//in intercode.c
extern InterCodes *g_CodesHead;
extern int if_Operand_Address(Operand *op);

void trans_to_mips(InterCodes *codes);
int get_reg(Operand *op);
FILE *g_mips_fp;
void output_mips(char *filename){
	/*output mips32 instructions*/
	g_mips_fp = fopen(filename, "w");
	fprintf(g_mips_fp, "%s", g_mips_head);
	InterCodes *codes = g_CodesHead;
	memset(g_reg_using, 0, sizeof(g_reg_using));
	while(codes!=NULL){
		trans_to_mips(codes);
		codes = codes->next;
	}
	fclose(g_mips_fp);
	return;
}

int get_reg(Operand *op){
	/*get the number of reg t_i*/
	RegInfo *info = NULL;
	if(op->kind==TEMP || op->kind==VARIABLE 
		||op->kind==T_ADDRESS || op->kind==V_ADDRESS)
	{//op is a variable or temp variable
		info = InfoTable_search(op);
		if(info == NULL)
		{	
			InfoTable_insert(op, info->offset);
		}
		else
		{
			
		}
	}
	for(int i=0; i<8; i++){
		if(g_reg_using[i]==0){
			g_reg_using[i] = 1;
			g_reginfo[i] = info;
			return i;
		}
	}
	g_reginfo[0] = NULL;
	return 0;
}

void spill(int reg_idx);
void reg_free(int reg_idx){
	myassert(reg_idx>=0 && reg_idx<8);
	g_reg_using[reg_idx] = 0;
	g_reginfo[reg_idx] = NULL;
}

void spill(int reg_idx){
	/*sw value in reg_idx to memory*/	
	RegInfo *info = g_reginfo[reg_idx];
}

void trans_to_mips(InterCodes *codes){
	/*translate intercodes to mips32 instructions*/
	myassert(codes!=NULL);
	Operand *left = &(codes->code.result);
	Operand *op1 = &(codes->code.op1);
	Operand *op2 = &(codes->code.op2);
	int rr, r1, r2;
	int addflg1, addflg2, addflg3;
	addflg1 = if_Operand_Address(left);
	addflg2 = if_Operand_Address(op1);
	addflg3 = if_Operand_Address(op2);
	switch(codes->code.kind){
		case LABEL_DEF:
			fprintf(g_mips_fp, "label%d:\n", left->u.value);
		case JMP:
			fprintf(g_mips_fp, " j label%d\n", left->u.value);
			break;
		case FUN:
			fprintf(g_mips_fp, "%s:\n", left->u.cVal);
			break;
		case ASSIGN:
			rr = get_reg(left); r1 = get_reg(op1);
			if(addflg1 && addflg2){
				myassert(0);
				r2 = get_reg(op2);
				fprintf(g_mips_fp, " lw $%d, 0($%d)\n", r2, r1);
				fprintf(g_mips_fp, " sw $%d, 0($%d)\n", r2, rr);
				spill(rr);
				reg_free(r2);
			}else if(addflg1){
				fprintf(g_mips_fp, " sw $%d, 0($%d)\n", r1, rr);
			}else if(addflg2){
				fprintf(g_mips_fp, " lw $%d, 0($%d)\n", rr, r1);
				spill(rr);
			}else{
				fprintf(g_mips_fp, " move $%d, $%d\n", rr, r1);
				spill(rr);
			}
			reg_free(rr); reg_free(r1);
			break;
		case ADD_ASSIGN:
			myassert(0);
			break;
		case ADD:
		case SUB:
		case MUL:
		case DIV_L3:
			break;
		case JL:
		case JG:
		case JLE:
		case JGE:
		case JE:
		case JNE:
			break;
		case DEC_L3:
			break;
		case RET:
			break;
		case CALL:
			break;
		case ARG:
			break;
		case PARAM:
			break;
		case READ:
			break;
		case WRITE:
			break;
		default: 
			myassert(0);
			break;
	}
}
