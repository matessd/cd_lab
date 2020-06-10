#include"myhead.h"
static const char g_mips_head[] =
{
".data\n_prompt: .asciiz \"Enter an integer:\"\n\
_ret: .asciiz \"\\n\"\n.globl main\n.text\n\
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
 jr $ra\n\n"
};
int g_reg_using[8];//judge register $t0-$t7 is using
RegInfo *g_reginfo[8];

RegInfo *g_InfoHead=NULL, *g_InfoTail=NULL;
int g_fp_offset=-8;//offset to $fp for var in funciton

/*Description: insert reginfo into table
*/
RegInfo *InfoTable_insert(Operand *op, int offset){
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
	return cur;
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

void clear_InfoTable(){
	RegInfo *cur = g_InfoHead;
	while(cur!=NULL){
		free(cur);
		cur = cur->next;
	}
	g_InfoHead = NULL;
	g_InfoTail = NULL;
	g_fp_offset = -8;
}

//in intercode.c
extern InterCodes *g_CodesHead;
extern int if_Operand_Address(Operand *op);
extern void pt_InterCodes(FILE *fp, InterCodes *codes);

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
	int i=0;
	for(i=0; i<8; i++){
		if(g_reg_using[i]==0){
			g_reg_using[i] = 1;
			break;
		}
	}
	if(i==8){
		myassert(0);
		i = 0;
	}
	RegInfo *info = NULL;
	if(op->kind==TEMP || op->kind==VARIABLE 
		||op->kind==T_ADDRESS || op->kind==V_ADDRESS)
	{//op is a variable or temp variable
		info = InfoTable_search(op);
		if(info == NULL)
		{	
			g_fp_offset = g_fp_offset - 4;
			info = InfoTable_insert(op, g_fp_offset);
			fprintf(g_mips_fp, " addi $sp, $sp, -4\n");
		}
		else
		{
			fprintf(g_mips_fp, " lw $t%d, %d($fp)\n",i,info->offset);	
		}
	}else if(op->kind==CONSTANT)
	{
		fprintf(g_mips_fp, " li $t%d, %d\n", i, op->u.value);
	}
	g_reginfo[i] = info;
	return i;
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
	fprintf(g_mips_fp, " sw $t%d, %d($fp)\n", reg_idx, info->offset);
}

#define FRAME_SIZE 8
void trans_to_mips(InterCodes *codes){
	/*translate intercodes to mips32 instructions*/
	//printf("%s\n",g_SignName[codes->code.kind]);
	myassert(codes!=NULL);
	Operand *left = &(codes->code.result);
	Operand *op1 = &(codes->code.op1);
	Operand *op2 = &(codes->code.op2);
	int rr, r1, r2;
	int addflg1, addflg2, addflg3;
	addflg1 = if_Operand_Address(left);
	addflg2 = if_Operand_Address(op1);
	addflg3 = if_Operand_Address(op2);
	//DEBUG
	fprintf(g_mips_fp, "#");
	pt_InterCodes(g_mips_fp, codes);

	switch(codes->code.kind){
		case LABEL_DEF:
			fprintf(g_mips_fp, "label%d:\n", left->u.value);
			break;
		case JMP:
			fprintf(g_mips_fp, " j label%d\n", left->u.value);
			break;
		case FUN:
			fprintf(g_mips_fp, "%s:\n", left->u.cVal);
			if(strcmp("main",left->u.cVal)==0){
				fprintf(g_mips_fp, " move $fp, $sp\n subu $sp, $sp, %d\n", FRAME_SIZE);
			}
			break;
		case RET:
			rr = get_reg(left);
			fprintf(g_mips_fp, " move $v0, $t%d\n jr $ra\n\n", rr);
			reg_free(rr);
			clear_InfoTable();
			break;
		case ASSIGN:
			rr = get_reg(left); r1 = get_reg(op1);
			if(addflg1 && addflg2){
				myassert(0);
				r2 = get_reg(op2);
				fprintf(g_mips_fp, " lw $t%d, 0($t%d)\n", r2, r1);
				fprintf(g_mips_fp, " sw $t%d, 0($t%d)\n", r2, rr);
				spill(rr);
				reg_free(r2);
			}else if(addflg1){
				fprintf(g_mips_fp, " sw $t%d, 0($t%d)\n", r1, rr);
			}else if(addflg2){
				fprintf(g_mips_fp, " lw $t%d, 0($t%d)\n", rr, r1);
				spill(rr);
			}else{
				fprintf(g_mips_fp, " move $t%d, $t%d\n", rr, r1);
				spill(rr);
			}
			reg_free(rr); reg_free(r1);
			break;
		case ADD_ASSIGN:
			rr = get_reg(left); r1 = get_reg(op1);
			if(addflg1 && addflg2){
				fprintf(g_mips_fp, " move $t%d, $t%d\n", rr, r1);	
			}else if(addflg1){
				fprintf(g_mips_fp, " addi $t%d, $fp, %d\n", rr, g_reginfo[r1]->offset);
			}else{
				myassert(0);
			}
			spill(rr);
			reg_free(rr); reg_free(r1);
			break;
		case DEC_L3:
			g_fp_offset = g_fp_offset - op1->u.value;
			InfoTable_insert(left, g_fp_offset);
			fprintf(g_mips_fp, " subu $sp, $sp, %d",op1->u.value);
			//myassert(0);
			break;
		case ADD:
		case SUB:
		case MUL:
		case DIV_L3:
			rr=get_reg(left); r1=get_reg(op1); r2=get_reg(op2);
			fprintf(g_mips_fp, " %s $t%d, $t%d, $t%d\n", g_arithIns[codes->code.kind], rr, r1, r2);
			spill(rr);
			reg_free(rr); reg_free(r1); reg_free(r2);
			break;
		case JL:
		case JG:
		case JLE:
		case JGE:
		case JE:
		case JNE:
			r1=get_reg(op1); r2=get_reg(op2);
			fprintf(g_mips_fp, " %s $t%d, $t%d, label%d\n", g_arithIns[codes->code.kind], r1, r2, left->u.value);
			reg_free(r1); reg_free(r2);
			break;
		case CALL:
		case READ:
		case WRITE:
			if(codes->code.kind==WRITE){
				rr = get_reg(left);
				fprintf(g_mips_fp, " move $a0, $t%d\n",rr);
				reg_free(rr);
			}
			fprintf(g_mips_fp, " subu $sp, $sp, %d\n",FRAME_SIZE);
			fprintf(g_mips_fp, " sw $ra, %d($sp)\n",FRAME_SIZE-4);
			fprintf(g_mips_fp, " sw $fp, %d($sp)\n",FRAME_SIZE-8);
			fprintf(g_mips_fp, " addi $fp, $sp, %d\n",FRAME_SIZE);

			if(codes->code.kind==READ){
				fprintf(g_mips_fp, " jal read\n");
			}else if(codes->code.kind==WRITE){
				fprintf(g_mips_fp, " jal write\n");
			}else{
				fprintf(g_mips_fp, " jal %s\n", op1->u.cVal);
			}

			fprintf(g_mips_fp, " move $sp, $fp\n");
			fprintf(g_mips_fp, " lw $ra, -4($fp)\n");
			fprintf(g_mips_fp, " lw $fp, -8($fp)\n");
			if(codes->code.kind != WRITE){
				rr = get_reg(left);
				fprintf(g_mips_fp, " move $t%d, $v0\n", rr);
				spill(rr);
				reg_free(rr);
			}
			break;
		case ARG:
			myassert(0);
			break;
		case PARAM:
			break;
		default: 
			myassert(0);
			break;
	}
}
