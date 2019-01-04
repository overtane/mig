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

#define HASH_TABLE_SIZE 32

union hash_key {
  int id;
  const char *name;
};

struct hash_node {
  struct hash_node *next;
  union hash_key key;
};

typedef struct hash_node *hash_table[HASH_TABLE_SIZE];

static struct parameter *flip_parameters(struct parameter *, int *);
static struct enumerator *flip_enumerators(struct enumerator *, int *);
static struct element *flip_elements(struct element *, int *);
static void dump_elements( struct element *head );

typedef int (*comp_func)(union hash_key *, union hash_key *);
typedef int (*hash_func)(union hash_key *);

static void hash_table_init(hash_table);
static void *hash_table_search(hash_table, union hash_key *, hash_func, comp_func);
static int hash_table_add(hash_table, union hash_key *, hash_func);

static hash_table type_table;
static hash_table msg_table;


static int name2hash(union hash_key *key)
{
  int k = 0;
  const char *str = key->name;
  size_t n = strlen(str);
 
  while (n--)
    k += *str++;

  return k % HASH_TABLE_SIZE;
}

static int id2hash(union hash_key *key)
{
  return key->id % HASH_TABLE_SIZE;
}

static int namecmp(union hash_key *key1, union hash_key *key2)
{
  const char *name1 = key1->name;
  const char *name2 = key2->name;

  size_t n1 = strlen(name1);
  size_t n2 = strlen(name2);

  //printf("Compare %s==%s\n",name1, name2);
  return strncmp(name1, name2, (n1<n2)?n1:n2);
}

static int idcmp(union hash_key *key1, union hash_key *key2) 
{
  int ret;
 
  if (key1->id == key2->id)
    ret = 0;
  else if (key1->id < key2->id)
    ret = -1;
  else
    ret = 1;
  
  return ret;
}

void hash_table_init(hash_table ht) 
{
  int i;

  for (i=0; i<HASH_TABLE_SIZE; i++)
    ht[i] = NULL;
}

void *hash_table_search(hash_table ht, union hash_key *key, hash_func h, comp_func c)
{
  int hash = (*h)(key);

  // assert hash < HASH_TABLE_SIZE

  struct hash_node *node = ht[hash];

  while (node) {
    if (!(*c)(key, &node->key))
      return &node->key;
    node = node->next;
  }

  return NULL;
}

int hash_table_add(hash_table ht, union hash_key *key, hash_func h)
{
  int hash = (*h)(key);

  // assert hash < HASH_TABLE_SIZE

  struct hash_node *node = malloc(sizeof(*node));
  
  if (node) {
    node->next = NULL;
    memcpy((void *)&node->key, (void *)key, sizeof(*key));
  
    if (!ht[hash]) {
      ht[hash] = node;
    } else {
      struct hash_node *p = ht[hash];
      while (p->next)
        p = p->next;
      p->next = node;
    }
  }
  return (node)?1:0;
}

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
              (pp->repeated)? "required,repeated" : "required"
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

void mig_init(void) {
  hash_table_init(type_table);
  hash_table_init(msg_table);
}

int mig_find_msg(int id)
{
  void *ret;
  union hash_key key;

  key.id = id;

  ret = hash_table_search(msg_table, &key, id2hash, idcmp);
  //printf("%s(%d)=%p\n",__func__,id,ret);
  return (ret)?1:0;
}

int mig_find_type(const char *name)
{
  void *ret;
  union hash_key key;

  key.name = name;

  ret = hash_table_search(type_table, &key, name2hash, namecmp);
  //printf("%s(%s)=%p\n",__func__,name,ret);
  return (ret)?1:0;
}

int mig_add_element(const struct element *ep)
{
  int ret = 0;
  const char *typename;
  union hash_key typekey;
  
  if (ep) {
    typename = strdup(ep->name);
    typekey.name = typename;
    ret = hash_table_add(type_table, &typekey, name2hash);
    //printf("%s(%s)=%d\n",__func__,typekey.name,ret);
    if (ep->type == ET_MESSAGE) {
      union hash_key msgkey;
      msgkey.id = ep->message.id;
      hash_table_add(msg_table, &msgkey, id2hash);
    }
  }
  return ret;
}

struct element *
mig_creat_datatype(const char *name, int size)
{
  struct element *ep = malloc(sizeof(*ep));

  if (ep) {
    ep->next = NULL;
    ep->type = ET_DATATYPE;
    ep->name = strdup(name);

    ep->datatype.name = ep->name;
    ep->datatype.size = size;
  }
  
  return ep;
}


struct element *
mig_creat_message(const char *name, int id, struct parameter *parameters)
{
  struct element *ep = malloc(sizeof(*ep));

  if (ep) {
    ep->next = NULL;
    ep->type = ET_MESSAGE;
    ep->name = strdup(name);

    ep->message.name = ep->name;
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
    ep->name = strdup(name);

    ep->enumeration.name = ep->name;
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
    ep->name = strdup(name);

    ep->group.name = ep->name;
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
