#include"myhead.h"
struct sym_t{
	struct sym_t *nxt;
	char cVal[33];
	enum{VAR_TYPE, FUN_TYPE} id_type;
	enum{INT_TYPE, FLOAT_TYPE, STR_TYPE} val_type;
	union{
		int arr_dim;//array dimension
		int arg_num;//args number of function
		FieldList *stru;//field for structure
	};
};
typedef struct sym_t sym_t;
#define MAX_ENV_DEEP 1024
sym_t *envStack[MAX_ENV_DEEP];
sym_t *Tail[MAX_ENV_DEEP];
int envTop=0;
sym_t *curEnv=NULL;
void subTree_DFS(node_t* cur);

sym_t Sym_tmp, Sym_tmp1;//for passing value
void DFS(){
	Sym_tmp.nxt = NULL;
	Sym_tmp1.nxt = NULL;
	Tail[envTop] = curEnv = envStack[0] = NULL;
	subTree_DFS(root);
}
enum{CUR_MODE, ALL_MODE};
sym_t *search_table(const char *cVal, int mode, int id_type){
	sym_t *env=NULL;
	switch(mode){
		case ALL_MODE:
			for(int i=envTop; i>=0; i--){
				env = envStack[i];
				if(env==NULL) continue;
				while(env->nxt!=NULL){
					if(strcmp(env->cVal, cVal)==0&&id_type==env->id_type)
						return env;
					env = env->nxt;
				}
				if(strcmp(env->cVal, cVal)==0&&id_type==env->id_type)
					return env;
			}
			break;
		case CUR_MODE: 
			env = curEnv;
			if(env==NULL) break;
			while(env->nxt!=NULL){
				if(strcmp(env->cVal, cVal)==0&&id_type==env->id_type)
					return env;
				env = env->nxt;
			}
			if(strcmp(env->cVal, cVal)==0&&id_type==env->id_type)
				return env;
			break;
		default : break;
	}
	return NULL;
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
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
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
	pf3(insert);
	sym_t *p = NULL;
	switch(pSym->id_type){
	  case VAR_TYPE:
	    p = search_table(pSym->cVal, CUR_MODE, VAR_TYPE);
        if(p!=NULL){
		  pt_semantic_error(3, lineno, pSym->cVal);
		  return;
		}
	    break;
	  case FUN_TYPE:
	    p = search_table(pSym->cVal, CUR_MODE, FUN_TYPE);
        if(p!=NULL){
		  pt_semantic_error(4, lineno, pSym->cVal);
		  return;
		}
	    break;
	  default: break;
	}
	/*insert val into current symbol table*/
	//assert(curEnv == envStack[envTop]);
	sym_t *env = NULL;
	if(mode==OVER_MODE){
		Tail[0]->nxt = malloc(sizeof(sym_t));
		Tail[0] = Tail[0]->nxt;
		env = Tail[0];
	}else{
		if(curEnv==NULL){
			Tail[envTop] = curEnv = envStack[envTop] = malloc(sizeof(sym_t));
			env = curEnv;
		}
		else{
			Tail[envTop]->nxt = malloc(sizeof(sym_t));
			Tail[envTop] = Tail[envTop]->nxt;
			env = Tail[envTop];
		}
	}
	*env = *pSym;
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

void ParamDec_DFS(node_t *cur){
	//printf("1\n");
	node_t *child = cur->child;//Specifier
	node_t *gchild = child->child;//TYPE or StructSpecifier
	Sym_tmp1.id_type = VAR_TYPE;
	if(gchild->syntype==myTYPE){
		//INT or FLOAT
		Sym_tmp1.val_type = (strcmp(gchild->cVal,"int")==0)?INT_TYPE:FLOAT_TYPE;
		Sym_tmp1.arr_dim = -1;
		VarDec_DFS(child->bro, &Sym_tmp1);
		//printf("%s\n",Sym_tmp1.cVal);
		insert_symbol(LOCAL_MODE, child->bro->lineno, &Sym_tmp1);
	}else{
		Sym_tmp1.val_type = STR_TYPE;
		//TODO
	}
}

int count_VarList(node_t *cur){
	pf3(VarList);
	node_t *child = cur->child;//ParamDec
	ParamDec_DFS(child);
	if(child->bro==NULL) return 1;
	else return count_VarList(child->bro->bro)+1;
}

void FunDec_DFS(node_t *cur){
	pf3(FunDec);
	node_t *child = cur->child;
	strcpy(Sym_tmp.cVal, child->cVal);//ID
	if(child->bro->bro->syntype==myVarList){
		//no args
		Sym_tmp.arg_num = count_VarList(child->bro->bro);
	}else{
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
		Sym_tmp.val_type = STR_TYPE;
		//TODO
	}
}

/*void insert_module(node_t* cur){
	//part of subTree_traverse
	//VarDec
	if(cur->syntype==myVarDec && cur->child->syntype==myID){
		sym_t *p = search_table(cur->child->cVal, CUR_MODE, VAR_TYPE);
		if(p!=NULL)
			pt_semantic_error(3, cur->child->lineno, cur->child->cVal);
		else{
			insert_symbol(cur->child->cVal, VAR_TYPE);
		}
	}
	//FunDec
	else if(cur->syntype==myFunDec){
		sym_t *p = search_table(cur->child->cVal, CUR_MODE, FUN_TYPE);
		if(p!=NULL) 
			pt_semantic_error(4, cur->child->lineno, cur->child->cVal);
		else{
			insert_symbol(cur->child->cVal, FUN_TYPE);
		}
	}
}*/

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
	pf3(DecList);
	node_t *child = cur->child;
	if(child->bro!=NULL){//COMMA
		Dec_DFS(child);//Dec
		DecList_DFS(child->bro->bro);
	}else{
		Dec_DFS(child);
	}
}

void Def_DFS(node_t *cur){
	pf3(Def);
	node_t *child = cur->child;//Specifier
	node_t *gchild = child->child;//TYPE or StructSpecifier
	Sym_tmp.id_type = VAR_TYPE;
	if(gchild->syntype==myTYPE){
		//INT or FLOAT
		Sym_tmp.val_type = (strcmp(gchild->cVal,"int")==0)?INT_TYPE:FLOAT_TYPE;
        DecList_DFS(child->bro);
	}else{
		Sym_tmp.val_type = STR_TYPE;
		//TODO
	}
}

void Exp_DFS(node_t *cur){
	node_t *child = cur->child;
	sym_t *p = NULL;
	switch(child->syntype){
	  case myID:
		if(child->bro==NULL){
			p = search_table(child->cVal, ALL_MODE, VAR_TYPE);
			if(p==NULL) pt_semantic_error(1, child->lineno, child->cVal);
		}else{
			p = search_table(child->cVal, ALL_MODE, FUN_TYPE);
			if(p==NULL) pt_semantic_error(2, child->lineno, child->cVal);
		}
	    break;
	  case myINT:
		
	    break;
	  case myFLOAT:
	    break;
	  default: break;
	}
}

void subTree_DFS(node_t* cur){
	switch(cur->syntype){
	  case myExtDef: ExtDef_DFS(cur);
	    break;
	  case myDef: Def_DFS(cur);
	    break;
	  //case myParamDec: ParamDec_DFS(cur);
		//break;
	  default: break;
	}
	//DFS, traverse child node
	if(cur->child!=NULL){
		//change symbol table
		if(cur->syntype==myCompSt){
			//create symbol table
			envTop++; 
			Tail[envTop] = curEnv = envStack[envTop] = NULL;
			subTree_DFS(cur->child);
			//delete symbol table
			envTop--;
			curEnv = envStack[envTop];
		}else{
			subTree_DFS(cur->child);
		}
	}
	switch(cur->syntype){
	  case myExp: Exp_DFS(cur);
	    break;
	  default: break;
	}
	//traverse brother node
	if(cur->bro!=NULL)
		subTree_DFS(cur->bro);
}
