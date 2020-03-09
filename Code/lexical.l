%{
#define MY_DEBUG

#ifdef MY_DEBUG
#define pf(x) printf("%s:%s||%d\n", #x, yytext, yylineno)
#else
#define pf(x) 
#endif
%}

%option yylineno
letter [_a-zA-Z0-9]
digit [0-9]
digits {digit}+

%%
"if" {pf(if);}
"else" {pf(else);}
"while" {pf(while);}
"struct" {pf(struct);}
"return" {pf(return);}
"int"|"float" {pf(type);}
{letter}+ { pf(id); }
"0"|([1-9]{digits}) { pf(num); }

[ |\n|\t|\r]+ {}
"/*".*"*/" { pf(comment); }
"{" {}
"}" {}
"[" {}
"]" {}
"(" {}
")" {}
">"|"<"|">="|"<="|"=="|"!=" {}
"+" {}
"-" {}
"*" {}
"/" {}
"&&" {}
"||" {}
"." {}
"!" {}
"=" {}
"," {}
";" {}
. {} 

%%
