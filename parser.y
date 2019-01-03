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
  int optional, repeated;
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
%token <number> KW_MESSAGE KW_GROUP KW_ENUM KW_DATATYPE
%token <number> KW_OPTIONAL KW_REPEATED KW_VAR

%type <parameter> parameter parameters
%type <enumerator> enumerator enumerators
%type <element> elements datatype message enumeration group
%type <number> alloc

%start all 

%%

datatype
  : KW_DATATYPE IDENTIFIER '=' alloc ';'
    {
      if ( mig_find_type($2) )
          yyerror("Duplicate type name");
      $$ = mig_creat_datatype( $2, $4 );
      mig_add_element( $$ );
    }
  ;

alloc
  : INTEGER { $$ = $1; }
  | KW_VAR { $$ = -1; }
  ;

parameters
  : /* empty */ { $$ = NULL }
  | parameters parameter { $2->next = $1; $$ = $2; }
  ;

parameter
  : IDENTIFIER IDENTIFIER '=' INTEGER attribute_spec ';'
    {
      if ( !mig_find_type($1) )
          yyerror("Unknown typename");
      $$ = mig_creat_parameter( $1, $2, $4, optional, repeated );
    }
  ;

attribute_spec
  : /* empty */ { optional = 0, repeated = 0 } 
  | '[' attributes ']' 
  ;

attributes
  : attribute
  | attributes ',' attribute
  ;

attribute
  : opt_spec
  | rpt_spec 
  ;

opt_spec
  : KW_OPTIONAL { optional = 1; }
  ;

rpt_spec
  : KW_REPEATED { repeated = 1; }
  ;

message
  : KW_MESSAGE IDENTIFIER '=' INTEGER '{' parameters '}' 
    {
      if ( mig_find_type($2) )
          yyerror("Duplicate type name");
      if ( mig_find_msg($4) )
          yyerror("Duplicate message id");
      $$ = mig_creat_message( $2, $4, $6 );
      mig_add_element($$);
    }
  ;

enumeration
  : KW_ENUM IDENTIFIER '{' enumerators '}'
    {
      if (mig_find_type($2))
          yyerror("Duplicate type name");
      $$ = mig_creat_enumeration( $2, $4 );
      mig_add_element($$);
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
      if (mig_find_type($2))
          yyerror("Duplicate type name");
      $$ = mig_creat_group( $2, $4 );
      mig_add_element($$);
    }
  ;

elements
  : /* empty */          { $$ = NULL }
  | elements datatype    { $2->next = $1; $$ = $2; }
  | elements message     { $2->next = $1; $$ = $2; }
  | elements enumeration { $2->next = $1; $$ = $2; }
  | elements group       { $2->next = $1; $$ = $2; }
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
  mig_init();
  yyparse ();
  return 0;
}

