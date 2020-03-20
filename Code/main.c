#include<stdio.h>
extern void yyrestart(FILE *input_file);
extern int yyparse(void);
extern int yylineno;
extern int error_flg;
//extern int yydebug;
int main(int argc, char** argv) {
	if(argc <= 1) return 1;
	FILE *f = fopen(argv[1], "r");
	if(!f){
		perror(argv[1]);
		return 1;
	}
	error_flg = 0;
	yylineno = 1;
	yyrestart(f);
	yyparse();
	return 0;
}

