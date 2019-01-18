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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef DEBUG
#include <assert.h>
#else
#define assert(p)
#endif

#define HASH_TABLE_SIZE 32

union hash_key {
  int id;
  const char *name;
};

struct hash_node {
  struct hash_node *next;
  union hash_key key;
  void *item;
};

typedef int (*comp_func)(union hash_key *, union hash_key *);
typedef int (*hash_func)(union hash_key *);

struct hash_table {
  int nnodes;
  comp_func comp;
  hash_func hash;
  struct hash_node *table[HASH_TABLE_SIZE];
};

static struct parameter *flip_parameters(struct parameter *, int *);
static struct enumerator *flip_enumerators(struct enumerator *, int *);
static struct element *flip_elements(struct element *, int *);
static void dump_elements( struct element *head );

static struct hash_table *hash_table_new(hash_func, comp_func);
static void hash_table_delete(struct hash_table *);
static struct hash_node *hash_table_search(struct hash_table *, union hash_key *);
static struct hash_node *hash_table_add(struct hash_table *, union hash_key *, void *);

static void generate_cpp(struct element *head, FILE *of);


/* symbol tables */ 
static struct hash_table *type_table;
static struct hash_table *msg_table;

/*! \brief Count the number of nodes in a hash table 
 * 
 * Note: Slow, mainly for testing purposes
 * \param pointer to a hash table
 * \return number of nodes in the table 
 */
static int hash_table_size(struct hash_table *ht)
{
  int i, n=0;
  if (ht) {
    for (i=0; i<HASH_TABLE_SIZE; i++) {
      struct hash_node *node = ht->table[i];
      while (node) {
        n++;
        node = node->next;
      }
    }
  }
  return n;
}

/*! \brief Calculate hash value for a string key */
static int name2hash(union hash_key *key)
{
  int k = 0;
  const char *str = key->name;
  size_t n = strlen(str);
 
  while (n--)
    k += *str++;

  return k % HASH_TABLE_SIZE;
}

/*! \brief Calculate hash value for integer key */
static int id2hash(union hash_key *key)
{
  return key->id % HASH_TABLE_SIZE;
}

/*!  */
static int namecmp(union hash_key *key1, union hash_key *key2)
{
  const char *name1 = key1->name;
  const char *name2 = key2->name;

  size_t n1 = strlen(name1);
  size_t n2 = strlen(name2);

  //printf("Compare %s==%s\n",name1, name2);
  return strncmp(name1, name2, (n1<n2)?n1:n2);
}

/* */
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

/*! */
struct hash_table *hash_table_new(hash_func hash, comp_func comp) 
{
  int i, n;

  struct hash_table *ht = (struct hash_table *)malloc(sizeof(*ht));

  if (ht) {
    ht->nnodes = 0;
    ht->hash = hash;
    ht->comp = comp;

    for (i=0; i<HASH_TABLE_SIZE; i++)
      ht->table[i] = NULL;

    n=hash_table_size(ht);
  }
  assert(n==0);

  return ht;
}

static void hash_table_delete(struct hash_table *ht)
{
  int i;

  for (i=0; i<HASH_TABLE_SIZE; i++) {
    struct hash_node *curr, *next;
    
    curr = ht->table[i];
    while (curr) {
      next = curr->next;
      free(curr);
      curr = next;
    }
  }

  free(ht);
}

struct hash_node *hash_table_search(struct hash_table *ht, union hash_key *key)
{
  if (ht) {

    int hash = (*ht->hash)(key);
    struct hash_node *node; 

    assert(hash>=0 && hash<HASH_TABLE_SIZE);

    node = ht->table[hash];

    while (node) {
      if (!(*ht->comp)(key, &node->key))
        return node;
      node = node->next;
    }
  }
  return NULL;
}

/*! */
struct hash_node *hash_table_add(struct hash_table *ht, union hash_key *key, void *item)
{
  struct hash_node *node = NULL;

  if (ht) {
    int hash = (*ht->hash)(key);

    assert(hash >=0 && hash<HASH_TABLE_SIZE);

    node = (struct hash_node *)malloc(sizeof(*node));
    if (node) {
      node->next = NULL;
      memcpy((void *)&node->key, (void *)key, sizeof(*key));
      node->item = item;
      node->next = ht->table[hash];
      ht->table[hash]=node;
      ht->nnodes++;
    }
  }
  return node;
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


/*
 * Elements are added to list in stack order (revers)
 * Here we flip pointers in order to preserve source code order.
 */

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

struct {
  const char *out;
  const char *in;
  int dump;
} migpars;

void mig_init(const char *in, const char *out, int dump) {
  type_table = hash_table_new(name2hash, namecmp);
  msg_table = hash_table_new(id2hash, idcmp);

  migpars.out = out;
  migpars.in = in;
  migpars.dump = dump;
}

int mig_find_msg(int id)
{
  void *ret;
  union hash_key key;

  key.id = id;

  ret = hash_table_search(msg_table, &key);
  //printf("%s(%d)=%p\n",__func__,id,ret);
  return (ret)?1:0;
}

int mig_find_type(const char *name)
{
  void *ret;
  union hash_key key;

  key.name = name;

  ret = hash_table_search(type_table, &key);
  return (ret)?1:0;
}

int mig_add_element(const struct element *ep)
{
  const char *name;
  union hash_key key;
  
  if (ep) {
    name = ep->name;
    key.name = name;
    hash_table_add(type_table, &key, (void *)ep);
    if (ep->type == ET_MESSAGE) {
      union hash_key msgkey;
      msgkey.id = ep->message.id;
      hash_table_add(msg_table, &msgkey, (void *)ep);
    }
    return 0;
  }
  return -1;
}

struct element *
mig_creat_datatype(const char *name, const char *native_name, int is_var)
{
  struct element *ep = (struct element *)malloc(sizeof(*ep));

  if (ep) {
    ep->next = NULL;
    ep->type = ET_DATATYPE;
    ep->name = strdup(name);

    ep->datatype.name = ep->name;
    ep->datatype.type = strdup(native_name);
    ep->datatype.var = is_var;
  }

  return ep;
}


struct element *
mig_creat_message(const char *name, int id, struct parameter *parameters)
{
  struct element *ep = (struct element *)malloc(sizeof(*ep));

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
  struct element *ep = (struct element *)malloc(sizeof(*ep));

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
  struct element *ep = (struct element *)malloc(sizeof(*ep));

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
  struct enumerator *ep = (struct enumerator *)malloc(sizeof(*ep));
 
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
  struct parameter *ep = (struct parameter *)malloc(sizeof(*ep));

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

static void generate_m_params_vector(FILE *of, struct parameter *pp) 
{
  fprintf(of, "    const std::map<int, ::mig::parameter&> m_params = {\n");
  while (pp) {
    fprintf(of, "      {%d, %s},\n", pp->id, pp->name);
    pp = pp->next;
  }
  fprintf(of, "    };\n");
}

static void generate_parameters(FILE *of, struct parameter *pp) 
{
  while (pp) {
    union hash_key key = { .name = pp->type };
    struct hash_node *np = hash_table_search(type_table, &key);
    struct element *ep = (struct element *)np->item;

    const char *partype = NULL;
    const char *datatype = pp->type;
    const char *optional = (pp->optional)? ", ::mig::OPTIONAL" : "";

    if (ep->type == ET_GROUP)
      partype = "group";
    else if (ep->type == ET_DATATYPE) {
      datatype = ep->datatype.type; 
      partype = (ep->datatype.var)? "var" : "scalar";
    } else if (ep->type == ET_ENUM) 
      partype = "enum";

    if (partype)
      fprintf(of, "    ::mig::%s_parameter<%s> %s{%d%s};\n",
        partype, datatype, pp->name, pp->id, optional);

    pp = pp->next;
  }
}

void mig_generate_code( struct element *head ) {

  FILE *of = stdout;
  int n = 0;
  struct element *ep = flip_elements( head, &n );
 
  if (migpars.dump)
    dump_elements(ep);
  else if (migpars.out)
    of = fopen(migpars.out, "w");
   
  if (of)
    generate_cpp(ep, of);
  else
    fprintf(stderr, "Could not open output file (%s)\n", migpars.out);

  hash_table_delete(type_table);
  hash_table_delete(msg_table);
}

void generate_cpp( struct element *ep, FILE *of)
{
  time_t t = time(NULL);
  struct tm *tm = localtime(&t);

  char *upper = strdup(migpars.in);
  char *c = upper;

  while (*c != '\0') {
    if (isalpha(*c))
      *c = toupper(*c);
    else
      *c = '_';
    c++;
  }

  fprintf(of, "//  %s\n", migpars.out);
  fprintf(of, "//  This is an automatically generated file\n");
  fprintf(of, "//  Please do not edit\n");
  fprintf(of, "//  Source:  %s\n", migpars.in);
  fprintf(of, "//  %s\n", asctime(tm));
  fprintf(of, "#ifndef _%s_H_\n", upper);
  fprintf(of, "#define _%s_H_\n\n", upper);
  fprintf(of, "#include \"migmsg.h\"\n\n");

  while (ep) {
 
    switch (ep->type) {
    case ET_MESSAGE: {
        struct parameter *pp = ep->message.parameters;
        fprintf(of, "class %s : public ::mig::Message {\n\n", ep->message.name);

        fprintf(of, "  public:\n");
        fprintf(of, "    %s() : ::mig::Message(0x%x, m_params) {}\n",
          ep->message.name, ep->message.id);
        //fprintf(of, "    virtual ~%s() {}\n\n", ep->message.name);
        if (pp)
          generate_parameters(of, pp);
        fprintf(of, "\n");

        fprintf(of, "  private:\n");
        generate_m_params_vector(of, pp);

        fprintf(of, "};\n\n\n");
        break;
    }
        
    case ET_GROUP: {
        struct parameter *pp = ep->group.parameters;
        fprintf(of, "struct %s : ::mig::GroupBase {\n\n", ep->group.name);

        fprintf(of, "  public:\n");
        fprintf(of, "    %s() : ::mig::GroupBase(m_params) {}\n", ep->group.name);
        //fprintf(of, "    virtual ~%s() {}\n\n", ep->group.name);
        if (pp)
          generate_parameters(of, pp);
        fprintf(of, "\n");

        fprintf(of, "  private:\n");
        generate_m_params_vector(of, pp);

        fprintf(of, "};\n\n\n");
        break;
    }
        
    case ET_ENUM: {
        int value = 0;
        struct enumerator *pp = ep->enumeration.enumerators;
        fprintf(of, "enum class %s : ::mig::enum_t {\n", ep->enumeration.name);

        while (pp) {
          fprintf(of, "  %s = %d,\n", pp->name, pp->value);
          if (pp->value > value)
            value = pp->value;
          pp = pp->next;
        }
        fprintf(of, "  Count = %d\n", ++value);

        fprintf(of, "};\n\n\n");
        break;
    }
        
    default:
        break;
    }

    ep = ep->next;
  }
 
  fprintf(of, "#endif // ifndef _%s_H_\n", upper);

}
