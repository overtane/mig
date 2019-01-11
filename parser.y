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
  #include <ctype.h>
  #include <unistd.h>
  int yylex (void);
  void yyerror (char const *);
  extern FILE * yyin;
  int optional, repeated, compound;
%}

%union {
  char *string;
  int  number;
  struct parameter *parameter;
  struct enumerator *enumerator;
  struct element *element;
}

%token <string> IDENTIFIER SCOPED 
%token <number> INTEGER
%token <string> KW_MESSAGE KW_GROUP KW_ENUM KW_DATATYPE
%token <number> KW_OPTIONAL KW_REPEATED KW_COMPOUND

%type <parameter> parameter parameters
%type <enumerator> enumerator enumerators
%type <element> elements datatype message enumeration group
%type <string> typename

%start all 

%%

datatype
  : KW_DATATYPE IDENTIFIER '=' typename compound_spec ';'
    {
      if ( mig_find_type($2) )
          yyerror("Duplicate type name");
      $$ = mig_creat_datatype( $2, $4, compound );
      mig_add_element( $$ );
    }
  ;

typename
  : IDENTIFIER
  | SCOPED
  ;

compound_spec
  : /* empty */ { compound = 0; }
  | '[' KW_COMPOUND ']' { compound = 1; }
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
main (int argc, char *argv[])
{
  int c;
  int dump = 0;
  char *outname = NULL;
  FILE *inf;

  while ((c = getopt (argc, argv, "dho:p")) != -1)
    switch (c) {
      case 'd':
        dump = 1;
        break;
      case 'p':
        yydebug = 1;
        break;
      case 'o':
        outname = optarg;
        break;
      case '?':
        if (optopt == 'o')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      case 'h':
          fprintf (stdout, "%s [-dp] [-o outfile] infile\n", argv[0]);
          fprintf (stdout, "  -d dump parsed elements and exit\n");
          fprintf (stdout, "  -p lexical scanner debug output\n");
      default:
        goto error;
    }

  if (optind >= argc) {
    fprintf (stderr, "Missing input filename\n");
    goto error;
  }

  inf = fopen(argv[optind], "r");
  if (!inf) {
    fprintf (stderr, "Cannot open file %s for input\n", argv[optind]);
    goto error;
  }

  mig_init(argv[optind], outname, dump);

  yyin = inf;
  yyparse ();

  return 0;

error:
  return -1;
}

