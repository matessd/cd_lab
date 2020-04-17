#include"myhead.h"
typedef struct sym_t sym_t;
#define MAX_ENV_DEEP 1024
sym_t *envStack[MAX_ENV_DEEP];
int envTop=0;
sym_t *curEnv=NULL;
void subTree_DFS(node_t *cur);
void Def_DFS(node_t *cur);

sym_t Sym_tmp, Sym_tmp1;//for passing value
void DFS(){
	curEnv = envStack[0] = NULL;
	subTree_DFS(root);
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
			fprintf(stderr, "Error type %d at Line %d: \"%s\" is not an integer.\n",errnum, lineno, cVal);
			break;
		case 13:
			break;
		case 14:
			break;
		case 15:
			break;
		case 16:
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
	//printf("%d\n",lineno);
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
	    p1 = search_table(pSym->cVal, CUR_MODE, FUN_TYPE);
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

int Type_cmp(node_t *p1, node_t *p2){
	int ret = 0;
	if(p1->id_type!=STR_TYPE || p2->id_type!=STR_TYPE){
	  if(p1->id_type!=STR_TYPE && p2->id_type!=STR_TYPE){
		if(p1->val_type!=p2->val_type || p1->arr_dim!=p2->arr_dim)
			ret = 1;
	  }else{
	    ret = 1;
	  }
	}else{
		//TODO
	}
	return ret;
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

void Type_cpy(int errflg,node_t *dst, node_t *src){
	if(errflg==0){
		dst->errflg = errflg;
		dst->id_type = src->id_type;
		dst->val_type = src->val_type;
		dst->arr_dim = src->arr_dim;//union?
	}else{
		dst->errflg = 1;
	}
}

int VarDec_DFS(node_t *cur, sym_t *pSym){
	pf3(VarDec);
	node_t *child = cur->child;
	if(child->syntype==myID){
		strcpy(pSym->cVal, child->cVal);
	}else{
		VarDec_DFS(child, pSym);
	}
	pSym->arr_dim++;
}

void Dec_DFS(node_t *cur){
	pf3(Dec);
	node_t *child = cur->child;
	Sym_tmp.arr_dim = -1;
	VarDec_DFS(child, &Sym_tmp);
	insert_symbol(LOCAL_MODE, child->lineno, &Sym_tmp);
	if(child->bro!=NULL){
		//ASSIGNOP
		//TODO
	}
}

void DecList_DFS(node_t *cur){
	node_t *child = cur->child;
	if(child->bro!=NULL){//COMMA
		Dec_DFS(child);//Dec
		DecList_DFS(child->bro->bro);
	}else{
		Dec_DFS(child);
	}
}

node_t *DefList_DFS(sym_t *stru, node_t *cur){
    	
	return NULL;
}

node_t *StructSpecifier_DFS(node_t *cur){
	sym_t *p = NULL;
	if(cur->bro->syntype==myTag){
		//instance
		p = search_table(cur->bro->child->cVal, ALL_MODE, STR_TYPE);
		if(p==NULL){
			pt_semantic_error(17, cur->bro->lineno, cur->bro->child->cVal);
			return NULL;
		}else {
			return p;
		}
	}else{
		//definition
		sym_t *stru = malloc(sizeof(sym_t));
		stru->id_type = STR_TYPE;
		stru->detail = stru->member = NULL;
		stru->var_def = STR_DEF;
		//TODO
		if(cur->bro->syntype==myOptTag){
			strcpy(stru->cVal)
			if(cur->bro->bro->bro->syntype == myDefList){
			  p = DefList_DFS(cur->bro->bro->bro);
			  insert_symbol(OVER_MODE, cur->bro->lineno, p);
			}else{
			  //struct{}
			  return stru;
			}
		}else{
			//unknown name
			p = DefList_DFS(cur->bro->bro);
			insert_symbol(OVER_MODE, cur->bro->lineno, p);;
		}
	}
	return p;
}

void Def_DFS(node_t *cur){
	node_t *child = cur->child;//Specifier
	node_t *gchild = child->child;//TYPE or StructSpecifier
	if(gchild->syntype==myTYPE){
		//INT or FLOAT
		Sym_tmp.id_type = VAR_TYPE;
		Sym_tmp.val_type = (strcmp(gchild->cVal,"int")==0)?INT_TYPE:FLOAT_TYPE;
        DecList_DFS(child->bro);
	}else{
		Sym_tmp.id_type = STR_TYPE;
		//TODO
	}
}

void ParamDec_DFS(node_t *cur){
	node_t *child = cur->child;//Specifier
	node_t *gchild = child->child;//TYPE or StructSpecifier
	if(gchild->syntype==myTYPE){
		//INT or FLOAT
		Sym_tmp1.id_type = VAR_TYPE;
		Sym_tmp1.val_type = (strcmp(gchild->cVal,"int")==0)?INT_TYPE:FLOAT_TYPE;
		Sym_tmp1.arr_dim = -1;
		VarDec_DFS(child->bro, &Sym_tmp1);
		//printf("%s\n",Sym_tmp1.cVal);
		insert_symbol(LOCAL_MODE, child->bro->lineno, &Sym_tmp1);
	}else{
		//StructSpecifier
		Sym_tmp1.id_type = STR_TYPE;
		/*Sym_tmp1.arr_dim = -1;
		VarDec_DFS(child->bro, &Sym_tmp1);
		node_t *pstru = gchild->child;//Struct
		node_t *p = NULL;
	     if(pstru->bro->syntype==myTag){
			p = search_table(pstru->bro->child->cVal, ALL_MODE, VAR_TYPE);	
			if(p==NULL){
			  pt_semantic_error(17, pstru->bro->lineno, pstru->bro->child->cVal);
			}
			Sym_tmp1.
		}*/
		//TODO
	}
}

int count_VarList(node_t *cur){
	pf3(VarList);
	node_t *child = cur->child;//ParamDec
	ParamDec_DFS(child);
	//for structue and args type
	/* *detail = malloc(sizeof(node_t));
	**detail = Sym_tmp1;
	(*detail)->detail = (*detail)->member = NULL;*/
	if(child->bro==NULL) return 1;
	else {
		/*if((*detail)->val_type==STR_TYPE)
		  return count_VarList(child->bro->bro, &((*detail)->detail))+1;
		else 
		  return count_VarList(child->bro->bro, &((*detail)->member))+1;*/
		return count_VarList(child->bro->bro); 
	}
}

void FunDec_DFS(node_t *cur){
	pf3(FunDec);
	node_t *child = cur->child;
	strcpy(Sym_tmp.cVal, child->cVal);//ID
	if(child->bro->bro->syntype==myVarList){
		Sym_tmp.arg_num = count_VarList(child->bro->bro);
	}else{
		//no args
		Sym_tmp.arg_num = 0;
	}
}

void ExtDecList_DFS(node_t *cur){
	pf3(ExtDecList);
	node_t *child = cur->child;//VarDec
	Sym_tmp.arr_dim = -1;
	VarDec_DFS(child, &Sym_tmp);
	insert_symbol(LOCAL_MODE,child->lineno, &Sym_tmp);
	if(child->bro!=NULL){
		node_t *p = child->bro->bro;
		ExtDecList_DFS(p);
	}
}

void ExtDef_DFS(node_t *cur){
	pf3(ExtDef);
	node_t *child = cur->child;//Specifier
	node_t *gchild = child->child;//TYPE or StructSpecifier
	//int val_type = -1;
	//node_t *p =NULL;
	if(gchild->syntype==myTYPE){
		//INT or FLOAT
		Sym_tmp.val_type = (strcmp(gchild->cVal,"int")==0)?INT_TYPE:FLOAT_TYPE;
		switch(child->bro->syntype){
		  case myExtDecList:
		    Sym_tmp.id_type = VAR_TYPE;
		    ExtDecList_DFS(child->bro);
			break;
		  case myFunDec:
		    Sym_tmp.id_type = FUN_TYPE;
			FunDec_DFS(child->bro);
			insert_symbol(LOCAL_MODE, child->bro->lineno, &Sym_tmp);
			break;
		  default: break;
		}
	}else{
		//StructSpecifier
		Sym_tmp.id_type = STR_TYPE;
		//StructSpecifier_DFS(gchild);
		//TODO
	}
}

void Exp_DFS(node_t *cur){
	//printf("%s:1\n",mytname[cur->child->syntype]);
	node_t *child = cur->child;
	sym_t *p = NULL;
	switch(child->syntype){
	  case myID:
		if(child->bro==NULL){
			p = search_table(child->cVal, ALL_MODE, VAR_TYPE);
			//printf("%s\n",child->cVal);
			if(p==NULL) {
				p = search_table(child->cVal, ALL_MODE, STR_TYPE);
				if(p==NULL) pt_semantic_error(1, child->lineno, child->cVal);
			}
		}else{
			p = search_table(child->cVal, ALL_MODE, FUN_TYPE);
			if(p==NULL) pt_semantic_error(2, child->lineno, child->cVal);
		}
		if(p!=NULL){
			Type_cpy(0, cur, p);
		}else{
			Type_cpy(1, cur, NULL);
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
	//printf("2\n");
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

void subTree_DFS(node_t* cur){
	//printf("%s:1\n",mytname[cur->syntype]);
	switch(cur->syntype){
	  case myExtDef: ExtDef_DFS(cur);
	    break;
	  case myDef: Def_DFS(cur);
	    break;
	  default: break;
	}
	//DFS, traverse child node
	if(cur->child!=NULL){
		//change symbol table
		if(cur->syntype==myCompSt){
			//create symbol table
			envTop++; 
			curEnv = envStack[envTop] = NULL;
			subTree_DFS(cur->child);
			//delete symbol table
			envTop--;
			curEnv = envStack[envTop];
		}else{
			subTree_DFS(cur->child);
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
	  default: break;
	}
	//traverse brother node
	if(cur->bro!=NULL)
		subTree_DFS(cur->bro);
}
