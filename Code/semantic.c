#include"myhead.h"
typedef struct sym_t sym_t;
#define MAX_ENV_DEEP 1024
sym_t *envStack[MAX_ENV_DEEP];
int envTop=0;
sym_t *curEnv=NULL;
void subTree_DFS(node_t *cur, int mode);
node_t *Def_DFS(node_t *cur, int mode);
node_t *StructSpecifier_DFS(node_t *cur);

sym_t Sym_tmp, Sym_tmp1;//for passing value
void DFS(){
	curEnv = envStack[0] = NULL;
	subTree_DFS(root, 0);
}

sym_t *SearchSymList(const char *cVal, sym_t *env, int id_type){
	if(env==NULL) return NULL;
	while(env->nxt!=NULL){
	  //printf("%s:1\n",env->cVal);
	  if(id_type==env->id_type){
		if(cVal==NULL)
		  return env;
		else{
		  if(strcmp(env->cVal, cVal)==0)
			return env;
		}
	  }
	  env = env->nxt;
	}
	if(id_type==env->id_type){
	  if(cVal==NULL)
		return env;
	  else{
		if(strcmp(env->cVal, cVal)==0)
		  return env;
	  }
	}
	return NULL;
}

enum{CUR_MODE, ALL_MODE};
sym_t *search_table(const char *cVal, int mode, int id_type){
	sym_t *env=NULL;
	switch(mode){
		case ALL_MODE:
			for(int i=envTop; i>=0; i--){
				env = SearchSymList(cVal, envStack[i], id_type);
				if(env!=NULL) break;
			}
			break;
		case CUR_MODE:
			env = SearchSymList(cVal, curEnv, id_type);
			break;
		default : break;
	}
	return env;
}

void pt_semantic_error(int errnum, int lineno,const char *cVal){
	switch(errnum){
		case 1:
			fprintf(stderr, "Error type %d at Line %d: Undefined variable \"%s\".\n",errnum, lineno, cVal);
			break;
		case 2:
			fprintf(stderr, "Error type %d at Line %d: Undefined function \"%s\".\n",errnum, lineno, cVal);
			break;
		case 3:
			fprintf(stderr, "Error type %d at Line %d: Redefined variable \"%s\".\n",errnum, lineno, cVal);
			break;
		case 4:
			fprintf(stderr, "Error type %d at Line %d: Redefined function \"%s\".\n",errnum, lineno, cVal);
			break;
		case 5:
			fprintf(stderr, "Error type %d at Line %d: Type mismatched for assignment.\n",errnum, lineno);
			break;
		case 6:
			fprintf(stderr, "Error type %d at Line %d: The left-hand side of an assignment must be a variable.\n",errnum, lineno);
			break;
		case 7:
			fprintf(stderr, "Error type %d at Line %d: Type mismatched for operands.\n",errnum, lineno);
			break;
		case 8:
			fprintf(stderr, "Error type %d at Line %d: Type mismatched for return.\n",errnum, lineno);
			break;
		case 9:
			fprintf(stderr, "Error type %d at Line %d: Args mismatched for function \"%s\".\n",errnum, lineno, cVal);
			break;
		case 10:
			fprintf(stderr, "Error type %d at Line %d: \"%s\" is not an array.\n",errnum, lineno, cVal);
			break;
		case 11:
			fprintf(stderr, "Error type %d at Line %d: \"%s\" is not a function.\n",errnum, lineno, cVal);
			break;
		case 12:
			fprintf(stderr, "Error type %d at Line %d: Not an integer.\n",errnum, lineno);
			break;
		case 13:
			fprintf(stderr, "Error type %d at Line %d: Illegal use of \".\".\n",errnum, lineno);
			break;
		case 14:
			fprintf(stderr, "Error type %d at Line %d: Non-existent field \"%s\".\n",errnum, lineno, cVal);
			break;
		case 15:
			fprintf(stderr, "Error type %d at Line %d: Redefined or initial in field \"%s\".\n",errnum, lineno, cVal);
			break;
		case 16:
			fprintf(stderr, "Error type %d at Line %d: Duplicated name \"%s\".\n",errnum, lineno, cVal);
			break;
		case 17:
			fprintf(stderr, "Error type %d at Line %d: Undefined structure \"%s\".\n",errnum, lineno, cVal);
			break;
		case 18:
			break;
		case 19:
			break;
		default:
			assert(0);
	}
}

enum{LOCAL_MODE, OVER_MODE};//current scope and overall scope
void insert_symbol(int mode, int lineno, sym_t *pSym){
	//check redefine
	//printf("%s:1\n",pSym->cVal);
	sym_t *p1 = NULL, *p2=NULL;
	switch(pSym->id_type){
	  case VAR_TYPE:
	    p1 = search_table(pSym->cVal, CUR_MODE, VAR_TYPE);
		p2 = search_table(pSym->cVal, ALL_MODE, STR_TYPE); 
        if(p1!=NULL || p2!=NULL){
		  pt_semantic_error(3, lineno, pSym->cVal);
		  return;
		}
	    break;
	  case FUN_TYPE:
	    p1 = search_table(pSym->cVal, ALL_MODE, FUN_TYPE);
        if(p1!=NULL){
		  pt_semantic_error(4, lineno, pSym->cVal);
		  return;
		}
	    break;
	  case STR_TYPE:
	    p1 = search_table(pSym->cVal, ALL_MODE, VAR_TYPE);
		p2 = search_table(pSym->cVal, ALL_MODE, STR_TYPE); 
        if(p1!=NULL || p2!=NULL){
		  pt_semantic_error(16, lineno, pSym->cVal);
		  return;
		}
	    break;
	  default: break;
	}
	/*insert val into current symbol table*/
	//assert(curEnv == envStack[envTop]);
	sym_t *env = NULL, *nxt=NULL;
	if(mode==OVER_MODE){
		//assert(0);
		env = envStack[0];
		if(env==NULL){
			env = envStack[0] = malloc(sizeof(sym_t));
			nxt = NULL;
			curEnv = envStack[envTop];
		}
		else{
			env = malloc(sizeof(sym_t));
			nxt = envStack[0];
			envStack[0] = env;
			curEnv = envStack[envTop];
		}
	}else{
		if(curEnv==NULL){
			env = curEnv = envStack[envTop] = malloc(sizeof(sym_t));
			nxt = NULL;
		}
		else{
			env = malloc(sizeof(sym_t));
			nxt = envStack[envTop];
			curEnv = envStack[envTop] = env;
		}
	}
	*env = *pSym;
	env->nxt = nxt;
}

int Type_cmp(node_t *p1, node_t *p2);
int Str_cmp(node_t *p1, node_t *p2){
	myassert(p1->val_type==STR_DEF && p2->val_type==STR_DEF);
	if(p1==p2) return 0;//same structure name
	while(p1->member!=NULL && p2->member!=NULL){
		p1 = p1->member;
		p2 = p2->member;
		if(Type_cmp(p1,p2)!=0)
			return 1;
	}
	if(p1->member!=p2->member) 
		return 1;
	return 0;
}

int Type_cmp(node_t *p1, node_t *p2){
	int ret = 0;
	//printf("%s---%s\n",p1->cVal,p2->cVal);
	//printf("%d---%d\n",p1->val_type,p2->val_type);
	//printf("%d---%d\n",p1->arr_dim,p2->arr_dim);
	//printf("%d---%d\n",p1->id_type,p2->id_type);
	if(p1->id_type!=STR_TYPE || p2->id_type!=STR_TYPE){
	  //printf("1\n");
	  if(p1->id_type!=STR_TYPE && p2->id_type!=STR_TYPE){
		if(p1->val_type!=p2->val_type || p1->arr_dim!=p2->arr_dim){
			//printf("2\n");
			ret = 1;
		}
	  }else{
		//printf("1\n");
	    ret = 1;
	  }
	}else{
		myassert(p1->val_type==STR_VAR && p2->val_type==STR_VAR);
		if(p1->arr_dim!=p2->arr_dim)
			return 1;
		if(p1->detail==p2->detail) 
			return 0;
		myassert(p1->detail!=NULL && p2->detail!=NULL);
		ret = Str_cmp(p1->detail, p2->detail);
	}
	return ret;
}

int Args_cmp(node_t *p1, node_t *p2){
	//printf("1\n");
	if(p1==NULL || p2==NULL) return 1;
	//printf("2\n");
	//myassert(p1->val_type==STR_VAR && p2->val_type==STR_VAR);
	if(p1==p2) return 0;//same structure name
	while(p1->member!=NULL && p2->member!=NULL){
		if(Type_cmp(p1,p2)!=0)
			return 1;
		p1 = p1->member;
		p2 = p2->member;
	}
	//printf("3\n");
	if(p1->member==NULL&&p2->member==NULL){
		return 0;
	}else{
		return 1;
	}
	if(Type_cmp(p1,p2)!=0)
			return 1;
	//printf("4\n");
	return 0;
 }

enum{LOGIC_OP, ASSIGN_OP, ARITH_OP};
int Type_op(int optype, node_t* p1, node_t *p2){
	//compare
	int errflg = 0;
	if(p1->errflg) return 1;//error
	if(p2!=NULL){
		if(p2->errflg) return 1;
	}
	switch(optype){
		//1
		case LOGIC_OP:
		  if(p1->val_type!=INT_TYPE || p1->arr_dim!=0 || p1->id_type==STR_TYPE)
			errflg = 1;
		  if(p2!=NULL){
			if(p2->val_type!=INT_TYPE || p2->arr_dim!=0 || p2->id_type==STR_TYPE)
				errflg = 1;
		  }
		  if(errflg)
			pt_semantic_error(7, p1->lineno, NULL);
		  break;
		//2
		case ASSIGN_OP:
		  errflg = Type_cmp(p1,p2);
		  if(errflg)
			pt_semantic_error(5, p1->lineno, NULL);
		  break;
		//3
		case ARITH_OP:
		  //printf("1\n");
		  if(p1->id_type==STR_TYPE || p1->arr_dim!=0)
			errflg = 1;
		  if(p2!=NULL){
			if(p2->id_type==STR_TYPE || p2->val_type!=p1->val_type || p2->arr_dim!=0)
				errflg = 1;
		  }
		  if(errflg)
			pt_semantic_error(7, p1->lineno, NULL);
		  break;
		default: break;
	}
	return errflg;//correct
}

void Type_cpy(int errflg, node_t *dst, node_t *src){
	if(errflg==0){
		dst->errflg = errflg;
		dst->id_type = src->id_type;
		dst->val_type = src->val_type;
		dst->arr_dim = src->arr_dim;//union?
		dst->arg_num = src->arg_num;
		dst->detail = src->detail;
		dst->member = src->member;
	}else{
		dst->errflg = 1;
	}
}

node_t *find_member_tail(node_t *cur){
	while(cur->member!=NULL)
		cur = cur->member;
	return cur;
}

void *VarDec_DFS(node_t *dst, sym_t *cur){
	node_t *child = cur->child;
	if(child->syntype==myID){
		strcpy(dst->cVal, child->cVal);
	}else{
		VarDec_DFS(dst, child);
	}
	dst->arr_dim++;
}

enum{STR_MODE, VAR_MODE};
node_t *Dec_DFS(node_t *cur, sym_t *type, int val_type, int mode){
	node_t *child = cur->child;//VarDec
	child->arr_dim = -1;
	//printf("%s*\n",child->cVal);
	VarDec_DFS(child, child);
	//printf("%s*\n",child->cVal);
	child->member = child->detail = NULL;
	if(type==NULL){
	  child->id_type = VAR_TYPE;
	  child->val_type = val_type;
	}else{
	  child->id_type = STR_TYPE; 
	  child->val_type = STR_VAR;
	  child->detail = type;
	}
	if(mode==VAR_MODE)
	  insert_symbol(LOCAL_MODE, child->lineno, child);
	if(child->bro!=NULL){
		//ASSIGNOP
		//TODO
		if(mode==STR_MODE)
			pt_semantic_error(15, child->lineno, child->cVal);
		int errflg = Type_op(ASSIGN_OP, child, child->bro->bro);
		Type_cpy(errflg, child, child->bro->bro);
	}
	return child;
}

node_t *DecList_DFS(node_t *cur, sym_t *type, int val_type, int mode){
	node_t *child = cur->child;
	node_t *p1=NULL, *p2=NULL;
	if(child->bro!=NULL){//COMMA
		p1 = Dec_DFS(child, type, val_type, mode);//Dec
		p2 =  DecList_DFS(child->bro->bro, type, val_type, mode);
		node_t *ptmp = find_member_tail(p1);
		myassert(ptmp!=NULL);
		ptmp->member = p2;
	}else{
		p1 = Dec_DFS(child, type, val_type, mode);
	}
	return p1;
}

node_t *Def_DFS(node_t *cur, int mode){
	//printf("1\n");
	node_t *child = cur->child;//Specifier
	//printf("%s1\n",child->child->cVal);
	node_t *gchild = child->child;//TYPE or StructSpecifier
	int val_type;
	node_t *p1=NULL;
	if(gchild->syntype==myTYPE){
		//INT or FLOAT
		//printf("1\n");
		val_type = (strcmp(gchild->cVal,"int")==0)?INT_TYPE:FLOAT_TYPE;
		//printf("Dec1\n");
        p1 = DecList_DFS(child->bro, NULL, val_type, mode);
		//printf("Dec\n");
		return p1;
	}else{
		//TODO
		val_type = STR_VAR;
		node_t *detail = StructSpecifier_DFS(gchild);
		if(detail==NULL) return NULL;
		//printf("2\n");
		p1 = DecList_DFS(child->bro, detail, val_type, mode);
	}
	return p1;
}

node_t *DefList_DFS(node_t *cur, int mode){
	//this function is for structure, and must return strvar list,
	//never return strdef
	//printf("1\n");
	node_t *child = cur->child;
	node_t *p1, *p2;
	if(child->bro==NULL){//Def
		//printf("2\n");
		p1 = Def_DFS(child, mode);
		//myassert(p1!=NULL);
		return p1;
	}
	//printf("def\n");
    p1 = Def_DFS(child, mode);
	p2 = DefList_DFS(child->bro, mode);
	//myassert(p1!=NULL && p2!=NULL);
	//Def may return a member list
	if(p1!=NULL){
		node_t *ptmp = find_member_tail(p1);
		ptmp->member = p2;
	}
	//printf("3\n");
	return p1;
}

int sub_check_str_scope(node_t *p1, node_t *p2){
	int ret = 0;
	myassert(p1!=NULL && p2!=NULL);
	while(p2->member!=NULL){
		if(strcmp(p1->cVal,p2->cVal)==0){
			pt_semantic_error(15, p2->lineno, p2->cVal);
			return 1;
		}
		p2=p2->member;
	}
	if(strcmp(p1->cVal, p2->cVal)==0){
		pt_semantic_error(15, p2->lineno, p2->cVal);
		return 1;
	}
	return 0;
}

int check_str_scope(node_t *type){
	if(type==NULL) return 0;
	type = type->member;
	//printf("1\n");
	while(type->member!=NULL){
		if (sub_check_str_scope(type, type->member)==1){
			return 1;
		}
		//printf("2\n");
		type = type->member;
	}
	//printf("2\n");
	return 0;
} 

node_t *StructSpecifier_DFS(node_t *cur){
	node_t *child = cur->child;
	sym_t *p = NULL;
	//printf("pos1\n");
	if(child->bro->syntype==myTag){
		//instance
		//printf("pos\n");
		p = search_table(child->bro->child->cVal, ALL_MODE, STR_TYPE);
		if(p==NULL){
			pt_semantic_error(17, child->bro->lineno, child->bro->child->cVal);
			return NULL;
		}else {
			return p;
		}
	}else{
		//definition
		//printf("pos2\n");
		p = malloc(sizeof(sym_t));
		p->id_type = STR_TYPE;
		p->detail = p->member = NULL;
		p->val_type = STR_DEF;
		//p->arrdim = -1;
		if(child->bro->syntype==myOptTag){
			strcpy(p->cVal,child->bro->child->cVal);
			if(child->bro->bro->bro->syntype == myDefList){
			  p->member = DefList_DFS(child->bro->bro->bro, STR_MODE);
			}else{
			  //struct a{}
			}
			insert_symbol(OVER_MODE, child->bro->lineno, p);
		}else{
			//unknown name
			p->cVal[0] = '\0';
			if(child->bro->bro->syntype == myDefList){
			  p->member = DefList_DFS(child->bro->bro, STR_MODE);
			}else{
			  //struct {}
			  return p;
			}
		}
		//TODO
		if(check_str_scope(p)!=0){ 
			//printf("1\n");
			return NULL;
		}
	}
	return p;
}

node_t *ParamDec_DFS(node_t *cur){
	//printf("1\n");
	node_t *child = cur->child;//Specifier
	node_t *gchild = child->child;//TYPE or StructSpecifier
	child->bro->arr_dim = -1;
	child->bro->detail = child->bro->member = NULL;
	node_t *type = NULL;
	VarDec_DFS(child->bro, child->bro);
	//printf("2\n");
	if(gchild->syntype==myTYPE){
		//INT or FLOAT
		child->bro->id_type = VAR_TYPE;
		child->bro->val_type = (strcmp(gchild->cVal,"int")==0)?INT_TYPE:FLOAT_TYPE;
		//printf("%s\n",child->bro->cVal);
		insert_symbol(LOCAL_MODE, child->bro->lineno, child->bro);
	}else{
		//StructSpecifier
		//printf("2\n");
		type = StructSpecifier_DFS(gchild);
		//printf("3\n");
		if(type==NULL)
			return NULL;
		child->bro->id_type = STR_TYPE;
		child->bro->val_type = STR_VAR;
		child->bro->detail = type;
		insert_symbol(OVER_MODE, child->bro->lineno, child->bro);
		//printf("4\n");
		//TODO
	}
	//printf("2\n");
	return child->bro;
}

node_t *VarList_DFS(node_t *cur){
	node_t *child = cur->child;//ParamDec
	node_t *p1=NULL, *p2=NULL;
	p1 = ParamDec_DFS(child);
	myassert(p1!=NULL);
	if(child->bro != NULL)
		p2 = VarList_DFS(child->bro->bro);	
	node_t *ptmp = find_member_tail(p1);
	ptmp->member = p2;
	return p1; 
}

void FunDec_DFS(node_t *cur, node_t* type, int val_type){
	node_t *child = cur->child;//ID
	strcpy(cur->cVal, child->cVal);
	//printf("1\n");
	cur->detail = cur->member = NULL;
	cur->id_type = FUN_TYPE;
	//val_type
	if(type==NULL){
		cur->val_type = val_type;
	}else{
		cur->detail = type; 
		cur->val_type = STR_VAR;
	}
	//new sym table
	envTop++; 
	curEnv = envStack[envTop] = NULL;
	if(child->bro->bro->syntype==myVarList){
		cur->member = VarList_DFS(child->bro->bro);
	}else{
		//no args
		cur->member = NULL;
	}
}

void ExtDecList_DFS(node_t *cur, node_t *type, int val_type){
	node_t *child = cur->child;//VarDec
	child->arr_dim = -1;
	child->detail = child->member = NULL;
	VarDec_DFS(child, child);
	if(type==NULL){
		child->id_type = VAR_TYPE;
		child->val_type = val_type;
	}
	else{
		child->id_type = STR_TYPE;
		child->val_type = STR_VAR;
	    child->detail = type; 
	}
	insert_symbol(LOCAL_MODE,child->lineno, child);
	if(child->bro!=NULL){
		node_t *p = child->bro->bro;
		ExtDecList_DFS(p, type, val_type);
	}
}

void ExtDef_DFS(node_t *cur){
	node_t *child = cur->child;//Specifier
	node_t *gchild = child->child;//TYPE or StructSpecifier
	int val_type = -1;
	node_t *type = NULL;
	if(gchild->syntype==myTYPE){
		//INT or FLOAT
		val_type = (strcmp(gchild->cVal,"int")==0)?INT_TYPE:FLOAT_TYPE;
	}else{
		//StructSpecifier
		//may be NULL
		type = StructSpecifier_DFS(gchild);  
		if(type==NULL) return;
	}
	switch(child->bro->syntype){
	  case myExtDecList:
		ExtDecList_DFS(child->bro, type, val_type);
	    break;
	  case myFunDec:
		FunDec_DFS(child->bro, type, val_type);
		insert_symbol(OVER_MODE, child->bro->lineno, child->bro);
		break;
	  default: break;
	}
}

node_t *search_field(char *cVal, node_t *cur){
	myassert(cur->val_type==STR_VAR);
	node_t *type = cur->detail;
	myassert(type!=NULL);
	node_t *member = type->member;
	while(member!=NULL){
		if(strcmp(member->cVal, cVal)==0)
			return member;
		member = member->member;
	}
	return NULL;
}

void Exp_DFS(node_t *cur){
	//printf("%s:1\n",mytname[cur->child->syntype]);
	node_t *child = cur->child;
	sym_t *p = NULL, *p1=NULL, *p2=NULL;
	int errflg = 0;
	switch(child->syntype){
	  case myID:
		if(child->bro==NULL){
			p = search_table(child->cVal, ALL_MODE, VAR_TYPE);
			//printf("%s\n",child->cVal);
			if(p==NULL) {
				p = search_table(child->cVal, ALL_MODE, STR_TYPE);
				if(p==NULL) {
					pt_semantic_error(1, child->lineno, child->cVal);
					errflg = 1;
				}
			}
		}else{
			p = search_table(child->cVal, ALL_MODE, FUN_TYPE);
			p1 = search_table(child->cVal, ALL_MODE, VAR_TYPE);
			p2 = search_table(child->cVal, ALL_MODE, STR_TYPE);
			if(p1!=NULL ||p2!=NULL){
				pt_semantic_error(11, child->lineno, child->cVal);
				errflg = 1;
			}
			else if(p==NULL) {
				pt_semantic_error(2, child->lineno, child->cVal);
				errflg = 1;
			}else if(child->bro->bro->syntype==myArgs){
				if(Args_cmp(p->member, child->bro->bro->child)!=0){
					pt_semantic_error(9, child->lineno, child->cVal);
					errflg = 1;
				}
			}
		}
		if(p!=NULL){
			Type_cpy(errflg, cur, p);
		}else{
			errflg = 1;
			Type_cpy(errflg, cur, NULL);
		}
	    break;
	  case myExp:
		//assign
	    if(child->bro->syntype==myASSIGNOP){
			cur->errflg = Type_op(ASSIGN_OP, child, child->bro->bro);
			Type_cpy(cur->errflg, cur, child);
			//Check errtype 6
			node_t *ptmp = child->child;
			int flg1, flg2, flg3;
			flg1 = flg2 = flg3 = 1;
			if(ptmp->bro==NULL){
				if(ptmp->syntype==myID) flg1 = 0;
			}else{
				if(ptmp->syntype==myExp && ptmp->bro->syntype==myLB)
					flg2 = 0;
				else if(ptmp->syntype==myExp && ptmp->bro->syntype==myDOT)
					flg3 = 0;
			}
			if(flg1 && flg2 && flg3){
				pt_semantic_error(6, child->lineno, NULL);
			}
		}
		//logic
		else if(child->bro->syntype==myAND || child->bro->syntype==myOR || child->bro->syntype==myRELOP){
			cur->errflg = Type_op(LOGIC_OP, child, child->bro->bro);
			Type_cpy(cur->errflg, cur, child);
		}
		//arithmic
		else if(child->bro->syntype==myPLUS || child->bro->syntype==myMINUS || child->bro->syntype==mySTAR || child->bro->syntype==myDIV){
			cur->errflg = Type_op(ARITH_OP, child, child->bro->bro);
			Type_cpy(cur->errflg, cur, child);
		}else if(child->bro->syntype==myLB){
			if(child->arr_dim<=0){
				cur->errflg = 1;
				pt_semantic_error(10, child->lineno, child->cVal);
			}
			if(child->bro->bro->val_type!=INT_TYPE){
				cur->errflg = 1;
				pt_semantic_error(12, child->bro->bro->lineno, NULL);
			}
			Type_cpy(cur->errflg, cur, child);
			cur->arr_dim--;
		}else if(child->bro->syntype==myDOT){
			//printf("%d\n",child->val_type);
			if(child->val_type!=STR_VAR){
				//printf("%s--%s\n",child->cVal,child->bro->bro->cVal);
				pt_semantic_error(13, child->lineno, NULL);
				errflg = 1;
				break;
			}
			p = search_field(child->bro->bro->cVal, child);
			if(p==NULL){
				pt_semantic_error(14, child->lineno, child->bro->bro->cVal);
				errflg = 1;
			}
			Type_cpy(errflg, cur, p);
			cur->member = NULL;
			//TODO
		}
		break;
	  case myLP:
		Type_cpy(child->errflg, cur, child);
	    break;
	  case myMINUS:
		//printf("1\n");
		cur->errflg = Type_op(ARITH_OP, child->bro, NULL);
		//printf("2\n");
		Type_cpy(cur->errflg, cur, child->bro);
		//printf("3\n");
	    break;
	  case myNOT:
		cur->errflg = Type_op(LOGIC_OP, child->bro, NULL);
		Type_cpy(cur->errflg, cur, child->bro);
	    break;
	  case myINT:
		Type_cpy(0, cur, child);
	    break;
	  case myFLOAT:
		Type_cpy(0, cur, child);
	    break;
	  default: break;
	}
}

node_t *Args_DFS(node_t *cur){
	node_t *child = cur->child;
	node_t *p1 = NULL, *p2=NULL;
	p1 = child;//Exp
	if(child->bro!=NULL)
		p2 = Args_DFS(child->bro->bro);
	p1->member = p2;
	/*p1->id_type = STR_TYPE;
	p1->val_type = STR_VAR;
	p1->arr_dim = 0;*/
	return p1;
}

void RETURN_DFS(node_t *cur){
	node_t *bro = cur->bro;
	sym_t *psym = search_table(NULL, ALL_MODE, FUN_TYPE);
	//printf("%s:%d\n",psym->cVal,psym->arr_dim);
	//printf("%s:%d\n",bro->cVal,bro->arr_dim);
    if(Type_cmp(psym, bro)!=0){
		pt_semantic_error(8, cur->lineno, NULL);
	}	
}

void subTree_DFS(node_t* cur, int mode){
	//printf("%s:1\n",mytname[cur->syntype]);
	switch(cur->syntype){
	  case myExtDef: ExtDef_DFS(cur);
	    break;
	  default: break;
	}
	//DFS, traverse child node
	if(cur->child!=NULL){
		//change symbol table
		if(cur->syntype==myCompSt){
			//create symbol table
			if(mode==0){
				envTop++; 
				curEnv = envStack[envTop] = NULL;
			}
			if(cur->child->bro->syntype==myDefList){ 
				DefList_DFS(cur->child->bro, VAR_MODE);
			}
			subTree_DFS(cur->child, 0);
			//delete symbol table
			envTop--;
			curEnv = envStack[envTop];
		}else{
			subTree_DFS(cur->child, 0);
		}
	}
	//printf("%s:2\n",mytname[cur->syntype]);
	switch(cur->syntype){
	  case myExp: Exp_DFS(cur);
	    break;
	  case myStmt: 
		if(cur->child->syntype==myRETURN){
			RETURN_DFS(cur->child);
		}
		break;
	  case myArgs: 
		Args_DFS(cur);
		break;
	  default: break;
	}
	//traverse brother node
	if(cur->bro!=NULL){
		if(cur->syntype==myFunDec)
			subTree_DFS(cur->bro, 1);
		else subTree_DFS(cur->bro, 0);
	}
}
