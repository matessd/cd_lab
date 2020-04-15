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
void subTree_traverse(node_t* cur);

sym_t Sym_tmp;//for passing value
void tree_traverse(){
	Sym_tmp.nxt = NULL;
	Tail[envTop] = curEnv = envStack[0] = NULL;
	subTree_traverse(root);
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

void insert_symbol(){
	/*insert val into current symbol table*/
	sym_t *tail = Tail[envTop];
	//assert(curEnv == envStack[envTop]);
	sym_t *env = NULL;
	if(curEnv==NULL){
		Tail[envTop] = curEnv = envStack[envTop] = malloc(sizeof(sym_t));
		env = curEnv;
	}
	else{
		tail->nxt = malloc(sizeof(sym_t));
		Tail[envTop] = tail->nxt;
		env = tail->nxt;
	}
	*env = Sym_tmp;
	env->nxt = NULL;
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

int VarDec_DFS(node_t *cur){
	node_t *child = cur->child;
	if(child->syntype==myID){
		strcpy(Sym_tmp.cVal, child->cVal);
	}else{
		VarDec_DFS(child);
	}
	Sym_tmp.arr_dim++;
}

void ExtDecList_DFS(node_t *cur){
	node_t *child = cur->child;//VarDec
	Sym_tmp.arr_dim = -1;
	VarDec_DFS(child);
	insert_symbol();
	if(child->bro!=NULL){
		node_t *p = child->bro->bro;
		ExtDecList_DFS(p);
	}
}

void ExtDef_DFS(node_t *cur){
	node_t *child = cur->child;//Specifier
	node_t *gchild = child->child;//TYPE or StructSpecifier
	//int val_type = -1;
	//node_t *p =NULL;
	if(gchild->syntype==myTYPE){
		//INT or FLOAT
		Sym_tmp.val_type = (gchild->child->syntype==myINT)?INT_TYPE:FLOAT_TYPE;
		switch(child->bro->syntype){
		  case myExtDecList:
		    Sym_tmp.id_type = VAR_TYPE;
		    ExtDecList_DFS(child->bro);
			break;
		  case myFunDec:
		    Sym_tmp.id_type = FUN_TYPE;
			break;
		  default: break;
		}
	}else{
		Sym_tmp.val_type = STR_TYPE;
	}
}

void insert_module(node_t* cur){
	/*part of subTree_traverse*/
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
}

void subTree_traverse(node_t* cur){
	//DFS, traverse child node
	if(cur->child!=NULL){
		//change symbol table
		if(cur->syntype==myCompSt){
			//create symbol table
			envTop++; 
			Tail[envTop] = curEnv = envStack[envTop] = NULL;
			subTree_traverse(cur->child);
			//delete symbol table
			envTop--;
			curEnv = envStack[envTop];
		}else{
			subTree_traverse(cur->child);
		}
	}
	//insert symbol table
	insert_module(cur);
	//search symbol table
	if(cur->syntype==myExp && cur->child->syntype==myID){
		if(cur->child->bro==NULL){
			sym_t *p = search_table(cur->child->cVal, ALL_MODE, VAR_TYPE);
			if(p==NULL) pt_semantic_error(1, cur->child->lineno, cur->child->cVal);
		}else{
			sym_t *p = search_table(cur->child->cVal, ALL_MODE, FUN_TYPE);
			if(p==NULL) pt_semantic_error(2, cur->child->lineno, cur->child->cVal);
		}
	}else if(1){
	
	}
	//traverse brother node
	if(cur->bro!=NULL)
		subTree_traverse(cur->bro);
}
