%option noyywrap
%{
/*****************声明和选项设置  begin*****************/
#include <stdio.h>
#include <stdlib.h>

#include "syntax_tree.h"
#include "syntax_analyzer.h"

int lines = 1;
int pos_start = 1;
int pos_end = 1;

void pass_node(char *text){
     yylval.node = new_syntax_tree_node(text);
}

/*****************声明和选项设置  end*****************/

%}


%%
 /* to do for students */
 /* two cases for you, pass_node will send flex's token to bison */
 /****请在此补全所有flex的模式与动作  end******/
\n   {lines+=1; pos_end=1;}
\t   {pos_start=pos_end; pos_end=pos_start + 1;}
" "  {pos_start=pos_end; pos_end=pos_start + 1;}
\r   {pos_start=pos_end; pos_end=pos_start + 1;}
\s   {pos_start=pos_end; pos_end=pos_start + 1;}
else {pos_start=pos_end; pos_end=pos_start + 4; pass_node(yytext); return ELSE;}
if   {pos_start=pos_end; pos_end=pos_start + 2; pass_node(yytext); return IF;}
int  {pos_start=pos_end; pos_end=pos_start + 3; pass_node(yytext); return INT;}
return    {pos_start=pos_end; pos_end=pos_start + 6; pass_node(yytext); return RETURN;}
void {pos_start=pos_end; pos_end=pos_start + 4; pass_node(yytext); return VOID;}
while     {pos_start=pos_end; pos_end=pos_start + 5; pass_node(yytext); return WHILE;}
float     {pos_start=pos_end; pos_end=pos_start + 5; pass_node(yytext); return FLOAT;}
\+   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return ADD;}
\-   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return SUB;}
\*   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return MUL;}
\/   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return DIV;}
\<   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return LT;}
"<=" {pos_start = pos_end; pos_end += 2; pass_node(yytext); return LE;}
\>   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return GT;}
">=" {pos_start = pos_end; pos_end += 2; pass_node(yytext); return GE;}
"==" {pos_start = pos_end; pos_end += 2; pass_node(yytext); return EQ;}
"!=" {pos_start = pos_end; pos_end += 2; pass_node(yytext); return NE;}
\=   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return ASSIGN;}
\;   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return SEMICOLON;}
\,   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return COMMA;}
\(   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return LPAREN;}
\)   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return RPAREN;}
\[   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return LSQUARE;}
\]   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return RSQUARE;}
\{   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return LBRACE;}
\}   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return RBRACE;}
[a-zA-Z]+ {pos_start = pos_end; pos_end = pos_start + strlen(yytext); pass_node(yytext); return ID;}
[0-9]+    {pos_start = pos_end; pos_end = pos_start + strlen(yytext); pass_node(yytext); return INTEGER;}
[0-9]+\.  {pos_start = pos_end; pos_end = pos_start + strlen(yytext); pass_node(yytext); return FLOATPOINT;}
[0-9]*\.[0-9]+ {pos_start = pos_end; pos_end = pos_start + strlen(yytext); pass_node(yytext); return FLOATPOINT;}
\/\*[\/]*([^\/]*[^\/^\*][\/]*)*[^\/]*\*\/ {
     int i = 0;
     for(i = 0; i < strlen(yytext); i++){
          if(yytext[i] == '\n'){
               lines+=1;
               pos_end = 1;
          }
          else{
               pos_end++;
          }
     }
     }

.    {pos_start = pos_end; pos_end++; return ERROR;}
%%
