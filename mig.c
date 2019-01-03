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
#include "mig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct parameter *flip_parameters(struct parameter *, int *);
static struct enumerator *flip_enumerators(struct enumerator *, int *);
static struct element *flip_elements(struct element *, int *);
static void dump_elements( struct element *head );

void dump_elements( struct element *head ) {

  struct element *ep = head;

  while (ep) {
    switch (ep->type) {
    case ET_MESSAGE: {
        struct parameter *pp = ep->message.parameters;
        printf("Message %s (Ox%04X)\n", ep->message.name, ep->message.id);
        while (pp) {
          printf("- parameter %s %s (%d) [%s]\n", 
            pp->type,
            pp->name,
            pp->id,
            (pp->optional)?
              (pp->repeated)? "optional,repeated" : "optional" :
              (pp->repeated)? "repeated" : ""
          );
          pp = pp->next;
        }
        break;
        }
    
    case ET_GROUP: {
        struct parameter *pp = ep->group.parameters;
        printf("Group %s\n", ep->group.name);
        while (pp) {
          printf("- %s %s (%d)\n", pp->type, pp->name, pp->id);
          pp = pp->next;
        }
        break;
        }
 
    case ET_ENUM: {
        struct enumerator *pp = ep->enumeration.enumerators;;
        printf("Enum %s\n", ep->enumeration.name);
        while (pp) {
          printf("- %s = %d\n", pp->name, pp->value);
          pp = pp->next;
        }
        break;
        }

    default:
        break;
    }
    ep = ep->next;
  }

}

#define TRAVERSE_AND_FLIP_LIST(this, next, prev, n) \
  { \
    while (this) { \
      n++; \
      next = this->next; \
      this->next = prev; \
      prev = this; \
      this = next; \
    } \
  }

struct element *
flip_elements(struct element *head, int *nparameters)
{
  int n = 0;
  struct element *pp = head, *next=NULL, *prev=NULL;

  TRAVERSE_AND_FLIP_LIST(pp, next, prev, n);

  *nparameters = n;
  return prev;
}


struct parameter *
flip_parameters(struct parameter *head, int *nparameters)
{
  int n = 0;
  struct parameter *pp = head, *next, *prev=NULL;

  TRAVERSE_AND_FLIP_LIST(pp, next, prev, n);

  *nparameters = n;
  return prev;
}

struct enumerator *
flip_enumerators(struct enumerator *head, int *nenumerators)
{
  int n = 0;
  struct enumerator *pp = head, *next, *prev=NULL;

  TRAVERSE_AND_FLIP_LIST(pp, next, prev, n);

  *nenumerators = n;
  return prev;
}


struct element *
mig_creat_message(const char *name, int id, struct parameter *parameters)
{

  struct element *ep = malloc(sizeof(*ep));

  if (ep) {
    ep->next = NULL;
    ep->type = ET_MESSAGE;

    ep->message.name = strdup(name);
    ep->message.id = id;
    ep->message.parameters = flip_parameters(parameters, &ep->message.nparameters);
  }
  
  return ep;
}

struct element *
mig_creat_enumeration(const char *name, struct enumerator *enumerators)
{

  struct element *ep = malloc(sizeof(*ep));

  if (ep) {
    ep->next = NULL;
    ep->type = ET_ENUM;

    ep->enumeration.name = strdup(name);
    ep->enumeration.enumerators = flip_enumerators(enumerators, &ep->enumeration.nenumerators);
  }

  return ep;
}

struct element *
mig_creat_group(const char *name, struct parameter *parameters)
{

  struct element *ep = malloc(sizeof(*ep));

  if (ep) {
    ep->next = NULL;
    ep->type = ET_GROUP;

    ep->group.name = strdup(name);
    ep->group.parameters = flip_parameters(parameters, &ep->group.nparameters);
  }

  return ep;
}

struct enumerator *
mig_creat_enumerator(const char *name, int value)
{

  struct enumerator *ep = malloc(sizeof(*ep));
 
  if (ep) {
    ep->next = NULL;
    ep->name = strdup(name);
    ep->value = value;
  }

  return ep;
}

struct parameter *
mig_creat_parameter(const char *type,
                    const char *name,
                    int id,
                    int optional,
                    int repeated )
{

  struct parameter *ep = malloc(sizeof(*ep));

  if (ep) {
    ep->next = NULL;
    ep->name = strdup(name);
    ep->type = strdup(type);
    ep->id = id;
    ep->optional = optional;
    ep->repeated = repeated;
  }

  return ep;
}


void mig_generate_code( struct element *head ) {

  int n = 0;
  struct element *ep = flip_elements( head, &n );

  dump_elements(ep);
}
