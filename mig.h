#ifndef MIG_H
#define MIG_H

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

/* top level elements in message definition file */

enum element_type {
  ET_DATATYPE,
  ET_MESSAGE,
  ET_ENUM,
  ET_GROUP
};

struct parameter {
  struct parameter *next;
  const char *name;
  int id;
  const char *type; /*< native data type */
  int optional;
  int repeated;
};

struct enumerator {
  struct enumerator *next;
  const char *name;
  int value;
};

struct datatype {
  const char *name;
  const char *type; /*< native data type */
  int composite;
};

struct message {
  const char *name;
  int id;
  struct parameter *parameters;
  int nparameters;
};

struct enumeration {
  const char *name;
  int id;
  struct enumerator *enumerators;
  int nenumerators;
};

struct group {
  const char *name;
  struct parameter *parameters;
  int nparameters;
};


struct element {
  struct element *next;
  enum element_type type;
  const char *name;
  union {
    struct datatype datatype;
    struct message message;
    struct enumeration enumeration;
    struct group group;
  };
};

struct element *mig_creat_datatype(const char *, const char *, int);
struct element *mig_creat_message(const char *, int, struct parameter *);
struct element *mig_creat_enumeration(const char *, struct enumerator *);
struct element *mig_creat_group(const char *, struct parameter *);
struct enumerator *mig_creat_enumerator(const char *, int);
struct parameter *mig_creat_parameter(const char*, const char *, int, int, int);

void mig_init(const char *, const char *, int);
int mig_find_type(const char *);
int mig_find_msg(int);
int mig_add_element(const struct element *);

void mig_generate_code( struct element *head );

#endif
