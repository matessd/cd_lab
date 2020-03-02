%{
#define pf() printf("%s||%d\n", yytext, yylineno)
%}

%option yylineno
letter [_a-zA-Z]
digit [0-9]
digits {digit}+

%%
{letter}+ { pf(); }
{digits} { pf(); }
[ |\n|\t|\r]+ {}
. { pf(); } 

%%
