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
%error-verbose
%locations

%{
  #include "mig.h"
  #include <stdio.h>
  #include <string.h>
  int yylex (void);
  void yyerror (char const *);
  int required, repeated;
%}

%union {
  char *string;
  int  number;
  struct parameter *parameter;
  struct enumerator *enumerator;
  struct element *element;
}

%token <string> IDENTIFIER 
%token <number> INTEGER
%token <string> KW_MESSAGE KW_GROUP KW_ENUM 
%token <number> KW_REQUIRED KW_OPTIONAL KW_REPEATED

%type <parameter> parameter parameters
%type <enumerator> enumerator enumerators
%type <element> elements message enumeration group

%start all 

%%

parameters
  : /* empty */ { $$ = NULL }
  | parameters parameter { $2->next = $1; $$ = $2; }
  ;

parameter
  : occurrence_specifier IDENTIFIER IDENTIFIER '=' INTEGER ';'
    {
      $$ = mig_creat_parameter( $2, $3, $5, required, repeated );
    }
  ;

occurrence_specifier
  : need_specifier count_specifier 
  ;

need_specifier
  : KW_REQUIRED { required = 1; }
  | KW_OPTIONAL { required = 0; }
  ;

count_specifier
  : { repeated = 0; }
  | KW_REPEATED { repeated = 1; }
  ;

message
  : KW_MESSAGE IDENTIFIER INTEGER '{' parameters '}' 
    {
      $$ = mig_creat_message( $2, $3, $5 );
    }
  ;

enumeration
  : KW_ENUM IDENTIFIER '{' enumerators '}'
    {
      $$ = mig_creat_enumeration( $2, $4 );
    }
  ;

enumerators
  : /* empty */ { $$ = NULL }
  | enumerators enumerator { $2->next = $1; $$ = $2; }
  ;

enumerator
  : IDENTIFIER '=' INTEGER ';'
    { 
      $$ = mig_creat_enumerator( $1, $3 );
    }
  ;

group
  : KW_GROUP IDENTIFIER '{' parameters '}'
    {
      $$ = mig_creat_group( $2, $4 );
    }
  ;

elements
  : /* empty */ { $$ = NULL }
  | elements message { $2->next = $1; $$ = $2; }
  | elements enumeration { $2->next = $1; $$ = $2; }
  | elements group { $2->next = $1; $$ = $2; }
  ;

all
  : elements { mig_generate_code( $1 ); }
  ;

%%

/* Called by yyparse on error.  */
void yyerror (char const *msg)
{
  extern int yylineno; /* flex option yylineno */
  fprintf (stderr, "%s, near line %d\n", msg, yylineno);
}

int
main (int argc, char const* argv[])
{
  int i;
  /* Enable parse traces on option -p.  */
  for (i = 1; i < argc; ++i)
    if (!strcmp(argv[i], "-p"))
      yydebug = 1;
  yyparse ();
  return 0;
}

