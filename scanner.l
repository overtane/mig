/* 
   Messaging Interface Generator

   Copyright 2019 Olli Vertanen

   Permission is hereby granted, free of charge, to any person obtaining a 
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
 
*/
%option yylineno
%x multi_comment single_comment

%{
#include "parser.h"
#include <string.h>
#include <stdlib.h>
%}

letter [a-zA-Z]
digit [0-9]
hexdigit  [0-9a-fA-F]
alfanumeric [a-zA-Z0-9_]
whitespace [ \t\n]


%%

"/*"                          BEGIN(multi_comment);
<multi_comment>[^*]*          /* eat anything that's not a '*' */
<multi_comment>"*"+[^*/]*     /* eat up '*'s not followed by '/'s */
<multi_comment>"*"+"/"        BEGIN(0);
"//"                          BEGIN(single_comment);
<single_comment>[^\n]*         /* eat anything until line feed */
<single_comment>\n            BEGIN(0);


message  { 
  yylval.number = KW_MESSAGE;
  return KW_MESSAGE;
}

group    { 
  yylval.number = KW_GROUP;
  return KW_GROUP;
}

enum     {
  yylval.number = KW_ENUM;
  return KW_ENUM;
}

optional  {
  yylval.number = KW_OPTIONAL;
  return KW_OPTIONAL;
}

repeated  {
  yylval.number = KW_REPEATED;
  return KW_REPEATED;
}

-?{digit}+  {
  yylval.number = strtol(yytext, NULL, 10);
  /* TODO check error */
  return INTEGER;
}

0[xX]{hexdigit}+  {
  yylval.number = strtol(yytext, NULL, 0);
  /* TODO check error */
  return INTEGER;
}

{letter}{alfanumeric}+ {
  yylval.string = strdup(yytext);
  /* TODO check error */
  return IDENTIFIER;
}
[,={};[\]] {
  return yytext[0];
}

{whitespace}+ /* skip */ 

%%

#if 0
int yyFlexLexer::yywrap() { return 0; }

int main() {
  FlexLexer* lexer = new yyFlexLexer;
  while(lexer->yylex() != 0)
    ;
  return 0;
}
#endif
