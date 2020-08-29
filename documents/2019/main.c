#define RBTREE
#ifndef RB_H
#define RB_H

#include <stdbool.h>

#define comp_t(name) int (*name)(const void*,const void*)

enum color { BLACK, RED };

typedef struct rbnode {
  struct rbnode* p;
  struct rbnode* left;
  struct rbnode* right;
  void* key;
  enum color c;
} rbnode;

typedef struct rbtree {
  rbnode* root;
  rbnode* nil;
  comp_t(compar);
} rbtree;

void init_rbtree(rbtree* t, comp_t(compar));
void destroy_rbtree(rbtree* t, void (*destroy)(void*));
void destroy_rbsubtree(rbtree* t, rbnode* n, void (*destroy)(void*));

static inline bool rb_is_empty(rbtree* t) { return t->root == t->nil; }

void* rbfind_key(rbtree* t, void* key);
rbnode* rbinsert_key(rbtree* t, void* key);
void rb_delete_key(rbtree* t, void (*destroy)(void*), void* key);

rbnode* rbsearch(rbtree* t, void* key);
rbnode* rbinsert(rbtree* t, rbnode* z);
void rbdelete(rbtree* t, rbnode* z);

rbnode* make_rbnode(rbtree* t, enum color c, void* key);
void destroy_rbnode(rbnode* n, void (*destroy)(void*));

#endif
#ifndef SLIST_H
#define SLIST_H

typedef struct snode {
  struct snode* next;
  struct snode* prev;
  void* data;
} snode;

typedef struct slist {
  snode* first;
  snode* last;
  int (*compar)(const void*, const void*);
} slist;

void init_slist(slist* sl, int (*compar)(const void*,const void*));
snode* make_snode(void* data);
void destroy_slist_nodes(slist* sl, void (*destroy)(void*));

snode* slist_insert(slist* sl, void* data);
void slist_insert_before(slist* sl, snode* next, snode* new_snode);
void slist_delete(slist* sl, snode* sn, void (*destroy)(void*));
void slist_remove(slist* sl, snode* sn);
snode* slist_find(slist* sl, void* data);

#endif
#ifndef RELATION_H
#define RELATION_H

#include <stdbool.h>


/*** Key Type ***/
typedef char* name_t;

/*** Degree Index ***/
typedef slist deg_list;
typedef snode deg_node;

struct entity;
typedef struct {
  struct entity* first;
  unsigned value;
} degree;

int compar_degree(const void* x, const void* y);
#define INIT_DEG_LIST(dl) init_slist(&dl, compar_degree)
#define DESTROY_DEG_LIST(dl) destroy_slist_nodes(&dl, free)
#define GET_FIRST_DEGREE(dl) (dl.first)
#define GET_LAST_DEGREE(dl) (dl.last)
#define GET_DEGREE(dn) ((degree*) dn->data)
#define GET_LOWER_DEG_NODE(dn) (dn->next)
#define GET_HIGHER_DEG_NODE(dn) (dn->prev)
#define DELETE_DEG_NODE(dl,dn) slist_delete(&dl, dn, free)
deg_node* insert_new_indegree_before(deg_list* dl, deg_node* next, unsigned new_indeg);

degree* make_degree(struct entity* first, unsigned value);

int compar_ent(const void* x, const void* y);

/*** Adjacency source list ***/
#ifdef RBTREE
typedef rbtree adj_list;
typedef rbnode adj_node;

#define INIT_ADJ_LIST(al) init_rbtree(&al, compar_ent)
#define DESTROY_ADJ_LIST(al) destroy_rbtree(&al, NULL)
#define ADJ_LIST_INSERT(al, sent) rbinsert_key(&al, sent)
#define ADJ_LIST_FIND(al, sent) rbsearch(&al, sent)
#define ADJ_LIST_DELETE(al, an) rbdelete(&al, an); destroy_rbnode(an, NULL)
#define ADJ_NODE_GET(an) ((struct entity*) an->key)

#else
typedef slist adj_list;
typedef snode adj_node;

#define INIT_ADJ_LIST(sl) init_slist(&sl, compar_ent)
#define DESTROY_ADJ_LIST(sl) destroy_slist_nodes(&sl, NULL)
#define ADJ_LIST_INSERT(sl, sent) slist_insert(&sl, sent)
#define ADJ_LIST_FIND(sl, sent) slist_find(&sl, sent)
#define ADJ_LIST_DELETE(sl, sn) slist_delete(&sl, sn, NULL)
#define ADJ_LIST_FIRST(sl) sl.first
#define ADJ_NODE_NEXT(sn) sn->next
#define ADJ_NODE_GET(sn) ((struct entity*) sn->data)
#endif


/*** Entity and Relation ***/
typedef struct entity {
  name_t name;
  adj_list sources;
  adj_list destinations;
  deg_node* indegree;
  struct entity* next;
  struct entity* prev;
} entity;

typedef struct {
  name_t name;
  entity* ent_first;
  entity* ent_last;
  deg_list degree_index;
#ifdef RBTREE
  rbtree ent_index;
#endif
} relation;


relation* make_relation(name_t name);
void destroy_relation(relation* rel);

bool is_empty_rel(relation* rel);
entity* insert_entity(relation* rel, name_t name);
void delete_entity(relation* rel, name_t name);
void insert_edge(relation* rel, name_t src, name_t dst);
void delete_edge(relation* rel, name_t src, name_t dst);
void delete_edge_ent(relation* r, entity* src_ent, entity* dst_ent);

void insert_entity_new_indegree(relation* r, entity* next_indeg,
				entity* new_ent, unsigned new_indeg);
/*
 * Insert new_ent in alphabetical order between begin and end.
 * begin is the beginning of the subsequence of the same indegree of new_ent,
 * end is the element after the last of the sequence (NULL if end of list).
 */
int insert_entity_same_indegree(relation* r, entity* begin, entity* end, entity* new_ent);
void insert_entity_before(relation* r, entity* next, entity* new_ent);
void remove_entity(relation* r, entity* e);
entity* find_entity(relation* r, name_t name);

entity* make_entity(name_t name);
void destroy_entity(entity* ent);

void report_relation(relation* r);
void report_name(const char* name);

#endif
#ifndef RELSET_H
#define RELSET_H

#include <stdlib.h>


/*** Relation list ***/
typedef slist rellist;
typedef snode relnode;

relnode* rellist_find(rellist* rl, const char* name);
int compar_rel(const void* x, const void* y);
void destroy_relation_void(void* r);
#define INIT_RELLIST(rs) init_slist(&rs, compar_rel)
#define DESTROY_RELLIST(rs) destroy_slist_nodes(&rs, destroy_relation_void)
#define RELLIST_FIRST(rl) (rl)->first
#define RELLIST_NEXT(rn) (rn)->next
#define GET_RELATION(rn) ((relation*) rn->data)
#define RELLIST_FIND(rl, name) rellist_find(&rl, name)
#define RELLIST_INSERT(rl, rel) slist_insert(&(rl), rel)

/*** Name Set ***/
#ifdef RBTREE
typedef rbtree nameset;
typedef rbnode namenode;

int compar_name(const void* x, const void* y);
#define INIT_NAMESET(ns) init_rbtree(ns, compar_name)
#define DESTROY_NAMESET(ns) destroy_rbtree(ns, free)
#define NAMESET_INSERT(ns, name) rbinsert_key(ns, name)
#define NAMESET_FIND(ns, name) rbsearch(ns, name)
#define NAMESET_DELETE(ns, name) rb_delete_key(ns, free, name)

#else
typedef slist nameset;
typedef snode namenode;

void nameset_delete(nameset* ns, char* name);
int compar_name(const void* x, const void* y);
#define INIT_NAMESET(nl) init_slist(nl, compar_name)
#define DESTROY_NAMESET(nl) destroy_slist_nodes(nl, free)
#define NAMESET_INSERT(ns, name) slist_insert(ns, name)
#define NAMESET_FIND(ns, name) slist_find(ns, name)
#define NAMESET_DELETE(ns, name) nameset_delete(ns, name)
#endif

typedef struct {
  rellist rl;
  nameset entset;
} relset;

void init_relset(relset* rs);
void destroy_relset(relset* rs);

void addent(relset* rs, char* ent_name);
void delent(relset* rs, char* ent_name);
void addrel(relset* rs, char* src_name, char* dst_name, char* rel_name);
void delrel(relset* rs, char* src_name, char* dst_name, char* rel_name);
void report(relset* rs);

#endif

#include <stdlib.h>
#include <assert.h>

void rb_insert_fixup(rbtree* t, rbnode* z);
void rb_delete_fixup(rbtree* t, rbnode* x);
void rb_transplant(rbtree* t, rbnode* u, rbnode* v);
void left_rotate(rbtree* t, rbnode* x);
void right_rotate(rbtree* t, rbnode* x);
rbnode* tree_minimum(rbtree* t, rbnode* x);

rbnode* make_rbnode(rbtree* t, enum color c, void* key)
{
  rbnode* n = (rbnode*) malloc(sizeof(rbnode));
  n->c = c;
  n->p = t->nil;
  n->left = t->nil;
  n->right = t->nil;
  n->key = key;
  return n;
}

void destroy_rbnode(rbnode* n, void (*destroy)(void*))
{
  if (destroy)
    destroy(n->key);
  free(n);
}

rbnode* make_rbnil()
{
  rbnode* nil = (rbnode*) malloc(sizeof(rbnode));
  nil->c = BLACK;
  nil->p = NULL;
  nil->left = NULL;
  nil->right = NULL;
  nil->key = NULL;
  return nil;
}

void init_rbtree(rbtree* t, comp_t(compar))
{
  assert(t && compar);
  t->nil = make_rbnil();
  t->root = t->nil;
  t->compar = compar;
}

void destroy_rbtree(rbtree* t, void (*destroy)(void*))
{
  assert(t);
  destroy_rbsubtree(t, t->root, destroy);
  free(t->nil);
}

void destroy_rbsubtree(rbtree* t, rbnode* n, void (*destroy)(void*))
{
  assert(t && n);
  if (n == t->nil)
    return;
  else if (n->p != t->nil) {
    if (n == n->p->left)
      n->p->left = t->nil;
    else
      n->p->right = t->nil;
  } else if (n == t->root) {
    t->root = t->nil;
  }

  rbnode* left = n->left;
  if (left != t->nil)
    left->p = t->nil;

  rbnode* right = n->right;
  if (right != t->nil)
    right->p = t->nil;

  destroy_rbnode(n, destroy);
  destroy_rbsubtree(t, left, destroy);
  destroy_rbsubtree(t, right, destroy);
}


void* rbfind_key(rbtree* t, void* key)
{
  assert(t && key);
  rbnode* res = rbsearch(t, key);
  if (res)
    return res->key;
  else
    return NULL;
}

rbnode* rbinsert_key(rbtree* t, void* key)
{
  rbnode* nn = make_rbnode(t, RED, key);
  rbnode* inserted = rbinsert(t, nn);
  if (inserted)
    return inserted;
  else {
    destroy_rbnode(nn, NULL);
    return NULL;
  }
}

void rb_delete_key(rbtree* t, void (*destroy)(void*), void* key)
{
  rbnode* n = rbsearch(t, key);
  if (n) {
    rbdelete(t, n);
    destroy_rbnode(n, destroy);
  }
}

rbnode* rbsearch(rbtree* t, void* key)
{
  assert(t && key);
  rbnode* x = t->root;
  while (x != t->nil) {
    int cmp = t->compar(key, x->key);
    if (cmp == 0)
      return x;
    else if (cmp < 0)
      x = x->left;
    else
      x = x->right;
  }
  return NULL;
}

rbnode* rbinsert(rbtree* t, rbnode* z)
{
  rbnode* y = t->nil;
  rbnode* x = t->root;
  while (x != t->nil) {
    y = x;
    int cmp = t->compar(z->key, x->key);
    if (cmp == 0)
      return NULL;
    else if (cmp < 0)
      x = x->left;
    else
      x = x->right;
  }
  z->p = y;
  if (y == t->nil) {
    assert(t->root == t->nil);
    t->root = z;
  } else if (t->compar(z->key, y->key) < 0)
    y->left = z;
  else
    y->right = z;

  z->left = t->nil;
  z->right = t->nil;
  z->c = RED;
  rb_insert_fixup(t, z);
  return z;
}

void rbdelete(rbtree* t, rbnode* z)
{
  assert(t && z);
  rbnode* x = NULL;
  rbnode* y = z;
  enum color y_orig_c = y->c;
  if (z->left == t->nil) {
    x = z->right;
    rb_transplant(t, z, z->right);
  } else if (z->right == t->nil) {
    x = z->left;
    rb_transplant(t, z, z->left);
  } else {
    y = tree_minimum(t, z->right);
    y_orig_c = y->c;
    x = y->right;
    if (y->p == z)
      x->p = y;
    else {
      rb_transplant(t, y, y->right);
      y->right = z->right;
      y->right->p = y;
    }
    rb_transplant(t, z, y);
    y->left = z->left;
    y->left->p = y;
    y->c = z->c;
  }
  if (y_orig_c == BLACK)
    rb_delete_fixup(t, x);
}

void rb_insert_fixup(rbtree* t, rbnode* z)
{
  assert(t && z);
  while (z->p->c == RED) {
    assert(z->p->p);
    if (z->p == z->p->p->left) {
      rbnode* y = z->p->p->right;
      if (y->c == RED) {
	z->p->c = BLACK;
	y->c = BLACK;
	z->p->p->c = RED;
	z = z->p->p;
      } else {
	if (z == z->p->right) {
	  z = z->p;
	  left_rotate(t, z);
	}
	z->p->c = BLACK;
	z->p->p->c = RED;
	right_rotate(t, z->p->p);
      }
    } else {
      assert(z->p == z->p->p->right);
      rbnode* y = z->p->p->left;
      if (y->c == RED) {
	z->p->c = BLACK;
	y->c = BLACK;
	z->p->p->c = RED;
	z = z->p->p;
      } else {
	if (z == z->p->left) {
	  z = z->p;
	  right_rotate(t, z);
	}
	z->p->c = BLACK;
	z->p->p->c = RED;
	left_rotate(t, z->p->p);
      }
    }
  }
  t->root->c = BLACK;
}

void rb_delete_fixup(rbtree* t, rbnode* x)
{
  assert(t && x);
  while (x != t->root && x->c == BLACK) {
    if (x == x->p->left) {
      rbnode* w = x->p->right;
      if (w->c == RED) {
	w->c = BLACK;
	x->p->c = RED;
	left_rotate(t, x->p);
	w = x->p->right;
      }
      if (w->left->c == BLACK && w->right->c == BLACK) {
	w->c = RED;
	x = x->p;
      } else {
	if (w->right->c == BLACK) {
	  w->left->c = BLACK;
	  w->c = RED;
	  right_rotate(t, w);
	  w = x->p->right;
	}
	w->c = x->p->c;
	x->p->c = BLACK;
	w->right->c = BLACK;
	left_rotate(t, x->p);
	x = t->root;
      }
    } else {
      rbnode* w = x->p->left;
      if (w->c == RED) {
	w->c = BLACK;
	x->p->c = RED;
	right_rotate(t, x->p);
	w = x->p->left;
      }
      if (w->right->c == BLACK && w->left->c == BLACK) {
	w->c = RED;
	x = x->p;
      } else {
	if (w->left->c == BLACK) {
	  w->right->c = BLACK;
	  w->c = RED;
	  left_rotate(t, w);
	  w = x->p->left;
	}
	w->c = x->p->c;
	x->p->c = BLACK;
	w->left->c = BLACK;
	right_rotate(t, x->p);
	x = t->root;
      }
    }
  }
  x->c = BLACK;
}

void rb_transplant(rbtree* t, rbnode* u, rbnode* v)
{
  if (u->p == t->nil)
    t->root = v;
  else if (u == u->p->left)
    u->p->left = v;
  else
    u->p->right = v;
  v->p = u->p;
}

void left_rotate(rbtree* t, rbnode* x)
{
  assert(t && x && x->right);
  rbnode* y = x->right;
  x->right = y->left;
  if (y->left != t->nil)
    y->left->p = x;

  y->p = x->p;
  if (x->p == t->nil) {
    assert(t->root == x);
    t->root = y;
  } else if (x == x->p->left)
    x->p->left = y;
  else {
    assert(x == x->p->right);
    x->p->right = y;
  }

  y->left = x;
  x->p = y;
}

void right_rotate(rbtree* t, rbnode* x)
{
  assert(t && x && x->left);
  rbnode* y = x->left;
  x->left = y->right;
  if (y->right != t->nil)
    y->right->p = x;

  y->p = x->p;
  if (x->p == t->nil) {
    assert(t->root == x);
    t->root = y;
  } else if (x == x->p->left)
    x->p->left = y;
  else {
    assert(x == x->p->right);
    x->p->right = y;
  }

  y->right = x;
  x->p = y;
}

rbnode* tree_minimum(rbtree* t, rbnode* x)
{
  assert(t && x && x != t->nil);
  while (x->left != t->nil)
    x = x->left;

  return x;
}
#include <stdlib.h>
#include <assert.h>


void init_slist(slist* sl, int (*compar)(const void*,const void*))
{
  assert(sl && compar);
  sl->first = sl->last = NULL;
  sl->compar = compar;
}

snode* make_snode(void* data)
{
  snode* new_snode = (snode*) malloc(sizeof(snode));
  new_snode->next = new_snode->prev = NULL;
  new_snode->data = data;
  return new_snode;
}

void destroy_slist_nodes(slist* sl, void (*destroy)(void*))
{
  snode* i = sl->first;
  snode* next;
  while (i) {
    next = i->next;
    if (destroy)
      destroy(i->data);
    free(i);
    i = next;
  }
  sl->first = sl->last = NULL;
}

snode* slist_insert(slist* sl, void* data)
{
  assert(sl && data);
  snode* i = sl->first;
  int cmp;
  while (i && (cmp = sl->compar(i->data, data)) < 0) {
    i = i->next;
  }
  if (i && cmp == 0) {
    // Already present.
    return NULL;
  }
  assert(!i || cmp > 0);
  snode* new_snode = make_snode(data);
  slist_insert_before(sl, i, new_snode);
  return new_snode;
}

void slist_insert_before(slist* sl, snode* next, snode* new_snode)
{
  assert(sl && new_snode);
  if (!sl->first) {
    // List is empty.
    assert(!next && !sl->last);
    new_snode->next = new_snode->prev = NULL;
    sl->first = sl->last = new_snode;
    return;
  }
  assert(sl->first && sl->last);
  if (!next) {
    // End of list.
    new_snode->next = NULL;
    new_snode->prev = sl->last;
    sl->last->next = new_snode;
    sl->last = new_snode;
    return;
  }
  assert(next);
  new_snode->next = next;
  if (next->prev) {
    new_snode->prev = next->prev;
    next->prev->next = new_snode;
  } else {
    // Beginning of the list.
    assert(sl->first == next);
    new_snode->prev = NULL;
    sl->first = new_snode;
  }
  next->prev = new_snode;
}

void slist_delete(slist* sl, snode* sn, void (*destroy)(void*))
{
  if (destroy)
    destroy(sn->data);

  slist_remove(sl, sn);
  free(sn);
}

void slist_remove(slist* sl, snode* sn)
{
  assert(sl && sn);
  if (sn->prev) {
    sn->prev->next = sn->next;
    if (sn->next)
      sn->next->prev = sn->prev;
    else {
      // End of the list.
      assert(sl->last == sn);
      sl->last = sn->prev;
    }
  } else {
    // Beginning of the list.
    assert(sl->first == sn);
    sl->first = sn->next;
    if (sn->next)
      sn->next->prev = NULL;
    else {
      // sn was the only element, sl is now empty.
      assert(sl->last == sn);
      sl->last = NULL;
    }
  }
  sn->next = sn->prev = NULL;
}

snode* slist_find(slist* sl, void* data)
{
  assert(sl && data);
  snode* i = sl->first;
  int cmp;
  while (i && (cmp = sl->compar(i->data, data)) < 0) {
    i = i->next;
  }
  if (i && cmp == 0)
    return i;
  assert(!i || cmp > 0);
  return NULL;
}
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>


int compar_degree(const void* x, const void* y)
{
  return (int) ((degree*) y)->value - (int) ((degree*) x)->value;
}

deg_node* insert_new_indegree_before(deg_list* dl, deg_node* next, unsigned new_indeg)
{
  assert(dl);
  degree* new_degree = make_degree(NULL, new_indeg);
  deg_node* new_dnode = make_snode(new_degree);
  slist_insert_before(dl, next, new_dnode);
  return new_dnode;
}

degree* make_degree(struct entity* first, unsigned value)
{
  degree* new_degree = (degree*) malloc(sizeof(degree));
  new_degree->first = first;
  new_degree->value = value;
  return new_degree;
}

int compar_ent(const void* x, const void* y)
{
  return strcmp(((const entity*) x)->name, ((const entity*) y)->name);
}

relation* make_relation(name_t name)
{
  assert(name);
  relation* r = malloc(sizeof(relation));
  r->name = strdup(name);
  r->ent_first = NULL;
  r->ent_last = NULL;
  INIT_DEG_LIST(r->degree_index);
#ifdef RBTREE
  init_rbtree(&r->ent_index, compar_ent);
#endif
  return r;
}

void destroy_relation(relation* rel)
{
  assert(rel);
  free(rel->name);

  entity* i = rel->ent_first;
  entity* next;
  while (i) {
    next = i->next;
    destroy_entity(i);
    i = next;
  }

  DESTROY_DEG_LIST(rel->degree_index);
#ifdef RBTREE
  destroy_rbtree(&rel->ent_index, NULL);
#endif
  free(rel);
}

bool is_empty_rel(relation* r)
{
  deg_node* first = GET_FIRST_DEGREE(r->degree_index);
  return !first || GET_DEGREE(first)->value == 0;
}

entity* insert_entity(relation* r, name_t name)
{
  assert(r && name);
  entity* new_ent = make_entity(name);

  deg_node* deg_node0 = GET_LAST_DEGREE(r->degree_index);
  if (!deg_node0 || GET_DEGREE(deg_node0)->value != 0) {
    insert_entity_new_indegree(r, NULL, new_ent, 0);
  } else {
    new_ent->indegree = deg_node0;
    int inserted = insert_entity_same_indegree(r, GET_DEGREE(deg_node0)->first, NULL, new_ent);
    if (!inserted) {
      destroy_entity(new_ent);
      return NULL;
    }
 }
#ifdef RBTREE
  rbinsert_key(&r->ent_index, new_ent);
#endif

  return new_ent;
}

void delete_entity(relation* r, name_t name)
{
  assert(r && name);
  entity* ent = find_entity(r, name);
  if (!ent)
    return;

#ifdef RBTREE
  while (!rb_is_empty(&ent->sources)) {
    delete_edge_ent(r, ADJ_NODE_GET(ent->sources.root), ent);
  }
  while (!rb_is_empty(&ent->destinations)) {
    delete_edge_ent(r, ent, ADJ_NODE_GET(ent->destinations.root));
  }
#else
  adj_node* i = ADJ_LIST_FIRST(ent->sources);
  while (i) {
    adj_node* i_next = ADJ_NODE_NEXT(i);
    delete_edge_ent(r, ADJ_NODE_GET(i), ent);
    i = i_next;
  }
  i = ADJ_LIST_FIRST(ent->destinations);
  while (i) {
    adj_node* i_next = ADJ_NODE_NEXT(i);
    delete_edge_ent(r, ent, ADJ_NODE_GET(i));
    i = i_next;
  }
#endif

#ifdef RBTREE
  rb_delete_key(&r->ent_index, NULL, ent);
#endif
  remove_entity(r, ent);
  destroy_entity(ent);
}

void insert_edge(relation* r, name_t src, name_t dst)
{
  assert(r && src && dst);
  entity* dst_ent = find_entity(r, dst);
  if (!dst_ent)
    dst_ent = insert_entity(r, dst);

  entity* src_ent = find_entity(r, src);
  if (!src_ent)
    src_ent = insert_entity(r, src);

  if (ADJ_LIST_FIND(dst_ent->sources, src_ent))
    return;

  ADJ_LIST_INSERT(dst_ent->sources, src_ent);
  ADJ_LIST_INSERT(src_ent->destinations, dst_ent);

  unsigned new_degree = GET_DEGREE(dst_ent->indegree)->value + 1;
  deg_node* succ_dn = GET_HIGHER_DEG_NODE(dst_ent->indegree);
  entity* dst_next = dst_ent->next;
  remove_entity(r, dst_ent);
  entity* lower_first = (dst_ent->indegree) ? GET_DEGREE(dst_ent->indegree)->first : dst_next;

  if (succ_dn && GET_DEGREE(succ_dn)->value == new_degree) {
    dst_ent->indegree = succ_dn;
    insert_entity_same_indegree(r, GET_DEGREE(succ_dn)->first, lower_first, dst_ent);
  } else {
    assert(!succ_dn || GET_DEGREE(succ_dn)->value > new_degree);
    insert_entity_new_indegree(r, lower_first, dst_ent, new_degree);
  }
}

void delete_edge(relation* r, name_t src, name_t dst)
{
  assert(r && src && dst);
  delete_edge_ent(r, find_entity(r, src), find_entity(r, dst));
}

void delete_edge_ent(relation* r, entity* src_ent, entity* dst_ent)
{
  assert(r);
  if (!dst_ent || !src_ent)
    return;

  adj_node* src_node = ADJ_LIST_FIND(dst_ent->sources, src_ent);
  adj_node* dst_node = ADJ_LIST_FIND(src_ent->destinations, dst_ent);
  assert((src_node && dst_node) || (!src_node && !dst_node));
  if (src_node) {
    ADJ_LIST_DELETE(dst_ent->sources, src_node);
    ADJ_LIST_DELETE(src_ent->destinations, dst_node);
  } else
    return;

  unsigned new_degree = GET_DEGREE(dst_ent->indegree)->value - 1;
  deg_node* prev_dn = GET_LOWER_DEG_NODE(dst_ent->indegree);
  remove_entity(r, dst_ent);
  if (prev_dn && GET_DEGREE(prev_dn)->value == new_degree) {
    deg_node* prev_prev_dn = GET_LOWER_DEG_NODE(prev_dn);
    dst_ent->indegree = prev_dn;
    insert_entity_same_indegree(r, GET_DEGREE(prev_dn)->first,
				(prev_prev_dn) ? GET_DEGREE(prev_prev_dn)->first : NULL,
				dst_ent);
  } else {
    assert(!prev_dn || GET_DEGREE(prev_dn)->value < new_degree);
    insert_entity_new_indegree(r, prev_dn ? GET_DEGREE(prev_dn)->first : NULL,
			       dst_ent, new_degree);
  }
}

void insert_entity_new_indegree(relation* r, entity* next_indeg,
				entity* new_ent, unsigned new_indeg)
{
  assert(r && new_ent);
  deg_node* next_deg_node = (next_indeg) ? next_indeg->indegree : NULL;
  new_ent->indegree = insert_new_indegree_before(&r->degree_index, next_deg_node, new_indeg);
  GET_DEGREE(new_ent->indegree)->first = new_ent;
  insert_entity_before(r, next_indeg, new_ent);
}

int insert_entity_same_indegree(relation* r, entity* begin, entity* end, entity* new_ent)
{
  assert(r && begin && new_ent);
  assert(begin->indegree == new_ent->indegree);
  entity* i = begin;
  int cmp;
  while (i != end && (cmp = strcmp(i->name, new_ent->name)) < 0) {
    i = i->next;
  }

  if (i != end && cmp == 0) {
    // Entity already existing.
    return 0;
  }

  assert(i == end || cmp > 0);
  insert_entity_before(r, i, new_ent);

  if (i == begin)
    GET_DEGREE(i->indegree)->first = new_ent;

  return 1;
}

void insert_entity_before(relation* r, entity* next, entity* new_ent)
{
  assert(r && new_ent);
  if (!r->ent_first) {
    // The list is empty.
    assert(!next && !r->ent_last);
    new_ent->next = new_ent->prev = NULL;
    r->ent_first = r->ent_last = new_ent;
    return;
  }
  assert(r->ent_first && r->ent_last);
  if (!next) {
    // End of the list.
    assert(r->ent_last);
    new_ent->next = NULL;
    new_ent->prev = r->ent_last;
    r->ent_last->next = new_ent;
    r->ent_last = new_ent;
    return;
  }
  assert(next);
  new_ent->next = next;
  if (next->prev) {
    new_ent->prev = next->prev;
    next->prev->next = new_ent;
  } else {
    // Beginning of the list.
    assert(r->ent_first == next);
    new_ent->prev = NULL;
    r->ent_first = new_ent;
  }
  next->prev = new_ent;
}

void remove_entity(relation* r, entity* e)
{
  assert(r && e);
  if (GET_DEGREE(e->indegree)->first == e) {
    if (!e->next || e->next->indegree != e->indegree) {
      // This was the only entity with this indegree.
      DELETE_DEG_NODE(r->degree_index, e->indegree);
      e->indegree = NULL;
    } else {
      GET_DEGREE(e->indegree)->first = e->next;
    }
  }

  if (e->prev) {
    e->prev->next = e->next;
    if (e->next) {
      e->next->prev = e->prev;
    } else {
      assert(r->ent_last == e);
      r->ent_last = e->prev;
    }
  } else {
    // Beginning of the list.
    assert(r->ent_first == e);
    r->ent_first = e->next;
    if (e->next)
      e->next->prev = NULL;
    else {
      assert(r->ent_last == e);
      r->ent_last = NULL;
    }
  }
  e->next = e->prev = NULL;
}

entity* find_entity(relation* r, name_t name)
{
  assert(r && name);
#ifdef RBTREE
  entity tmp;
  tmp.name = name;
  return (entity*) rbfind_key(&r->ent_index, &tmp);
#else
  entity* i = r->ent_first;
  int cmp;
  while (i && (cmp = strcmp(i->name, name)) != 0) {
    i = i->next;
  }
  if (i && cmp == 0)
    return i;
  else
    return NULL;
#endif
}

entity* make_entity(name_t name)
{
  entity* new_ent = (entity*) malloc(sizeof(entity));
  new_ent->name = strdup(name);
  INIT_ADJ_LIST(new_ent->sources);
  INIT_ADJ_LIST(new_ent->destinations);
  new_ent->indegree = NULL;
  new_ent->next = new_ent->prev = NULL;
  return new_ent;
}

void destroy_entity(entity* ent)
{
  free(ent->name);
  DESTROY_ADJ_LIST(ent->sources);
  DESTROY_ADJ_LIST(ent->destinations);
  free(ent);
}

void report_relation(relation* r)
{
  entity* i = r->ent_first;
  if (!i)
    return;

  report_name(r->name);
  putchar(' ');

  deg_node* next_deg = GET_LOWER_DEG_NODE(i->indegree);
  entity* end = (next_deg) ? GET_DEGREE(next_deg)->first : NULL;
  while (i != end) {
    report_name(i->name);
    putchar(' ');
    i = i->next;
  }
  printf("%d;", GET_DEGREE(r->ent_first->indegree)->value);
}

void report_name(const char* name)
{
  putchar('\"');
  fputs(name, stdout);
  putchar('\"');
}

#include <string.h>
#include <stdio.h>

relnode* rellist_find(rellist* rl, const char* name)
{
  relnode* rn = RELLIST_FIRST(rl);
  int cmp;
  while (rn && (cmp = strcmp(GET_RELATION(rn)->name, name)) < 0) {
    rn = RELLIST_NEXT(rn);
  }
  if (rn && cmp == 0)
    return rn;
  else
    return NULL;
}

int compar_rel(const void* x, const void* y)
{
  return strcmp(((const relation*) x)->name, ((const relation*) y)->name);
}

void destroy_relation_void(void* r)
{
  destroy_relation((relation*) r);
}

#ifndef RBTREE
void nameset_delete(nameset* ns, char* name)
{
  namenode* nn = NAMESET_FIND(ns, name);
  if (nn) {
    slist_delete(ns, nn, free);
  }
}
#endif

int compar_name(const void* x, const void* y)
{
  return strcmp((const char*) x, (const char*) y);
}

void init_relset(relset* rs)
{
  INIT_RELLIST(rs->rl);
  INIT_NAMESET(&rs->entset);
}

void destroy_relset(relset* rs)
{
  DESTROY_RELLIST(rs->rl);
  DESTROY_NAMESET(&rs->entset);
}

void addent(relset* rs, char* ent_name)
{
  char* name_copy = strdup(ent_name);
  if (!NAMESET_INSERT(&rs->entset, name_copy))
    free(name_copy);
}

void delent(relset* rs, char* ent_name)
{
  relnode* i = RELLIST_FIRST(&rs->rl);
  while (i) {
    delete_entity(GET_RELATION(i), ent_name);
    i = RELLIST_NEXT(i);
  }

  NAMESET_DELETE(&rs->entset, ent_name);
}

void addrel(relset* rs, char* src_name, char* dst_name, char* rel_name)
{
  if (!NAMESET_FIND(&rs->entset, src_name) || !NAMESET_FIND(&rs->entset, dst_name))
    return;

  relnode* rn = RELLIST_FIND(rs->rl, rel_name);
  if (!rn) {
    rn = RELLIST_INSERT(rs->rl, make_relation(rel_name));
  }

  relation* rel = GET_RELATION(rn);
  insert_edge(rel, src_name, dst_name);
}

void delrel(relset* rs, char* src_name, char* dst_name, char* rel_name)
{
  relnode* rn = RELLIST_FIND(rs->rl, rel_name);
  if (rn) {
    delete_edge(GET_RELATION(rn), src_name, dst_name);
  }
}

void report(relset* rs)
{
  relnode* i = RELLIST_FIRST(&rs->rl);
  bool reported = false;
  while (i) {
    relation* r = GET_RELATION(i);
    if (!is_empty_rel(r)) {
      if (reported)
	putchar(' ');

      reported = true;
      report_relation(r);
    }
    i = RELLIST_NEXT(i);
  }

  if (!reported)
    puts("none");
  else
    putchar('\n');
}
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>


#define ADDENT "addent"
#define DELENT "delent"
#define ADDREL "addrel"
#define DELREL "delrel"
#define REPORT "report"
#define END_C  "end"

#define BUFFER_SIZE 256

void p_error(const char* msg)
{
  fputs(msg, stderr);
  fputc('\n', stderr);
  exit(1);
}

void ignore_whitespace()
{
  int c;
  while (isblank(c = getchar()));
  ungetc(c, stdin);
}

bool expect_newline()
{
  ignore_whitespace();
  int c = getchar();
  if (c != '\n') {
    p_error("Newline expected.");
    return false;
  } else
    return true;
}

bool next_token(char* buf)
{
  ignore_whitespace();
  size_t i = 0;
  int c;
  while (i < BUFFER_SIZE - 1 && isgraph(c = getchar())) {
    buf[i] = c;
    ++i;
  }
  buf[i] = '\0';
  if (i == 0)
    return false;
  else if (!isgraph(c))
    ungetc(c, stdin);

  return true;
}

bool expect_name(char* buf)
{
  ignore_whitespace();
  size_t i = 0;
  int c = getchar();
  if (c != '\"') {
    p_error("Name expected.");
    return false;
  }
  while (i < BUFFER_SIZE - 1 && (c = getchar()) != '\"') {
    buf[i] = c;
    ++i;
  }
  buf[i] = '\0';
  if (i == 0) {
    p_error("Name expected.");
    return false;
  }
  else if (c != '\"') {
    p_error("Name too long.");
    return false;
  }

  return true;

}

void process_addent(relset* rs, char* buf)
{
  if (!expect_name(buf)) {
    p_error("addent: relation name expected.");
    return;
  }

  addent(rs, buf);

  expect_newline();
}

void process_delent(relset* rs, char* buf)
{
  if (!expect_name(buf)) {
    p_error("delent: relation name expected.");
    return;
  }

  delent(rs, buf);

  expect_newline();
}

void process_addrel(relset* rs, char* buf)
{
  if (!expect_name(buf)) {
    p_error("addrel: source entity expected.");
    return;
  }
  char* src_name = strdup(buf);

  if (!expect_name(buf)) {
    p_error("addrel: destination entity expected.");
    return;
  }
  char* dst_name = strdup(buf);

  if (!expect_name(buf)) {
    p_error("addrel: relation name expected.");
    return;
  }

  addrel(rs, src_name, dst_name, buf);

  free(src_name);
  free(dst_name);
  expect_newline();
}

void process_delrel(relset* rs, char* buf)
{
  if (!expect_name(buf)) {
    p_error("delrel: source entity expected.");
    return;
  }
  char* src_name = strdup(buf);

  if (!expect_name(buf)) {
    p_error("delrel: destination entity expected.");
    return;
  }
  char* dst_name = strdup(buf);

  if (!expect_name(buf)) {
    p_error("delrel: relation name expected.");
    return;
  }

  delrel(rs, src_name, dst_name, buf);

  free(src_name);
  free(dst_name);
  expect_newline();
}

void process_report(relset* rs, char* buf)
{
/*
  if (!expect_name(buf)) {
    p_error("report: relation name expected.");
    return;
  }
*/

  report(rs);

  expect_newline();
}

bool process_command(relset* rs, char* buf)
{
  if (!next_token(buf)) {
    p_error("Command expected.");
    return false;
  }

  if (strcmp(buf, ADDENT) == 0)
    process_addent(rs, buf);
  else if (strcmp(buf, DELENT) == 0)
    process_delent(rs, buf);
  else if (strcmp(buf, ADDREL) == 0)
    process_addrel(rs, buf);
  else if (strcmp(buf, DELREL) == 0)
    process_delrel(rs, buf);
  else if (strcmp(buf, REPORT) == 0)
    process_report(rs, buf);
  else if (strcmp(buf, END_C) == 0) {
    return false;
  } else {
    strcat(buf, ": unexpected command.");
    p_error(buf);
    return false;
  }
  return true;
}

int main() {

  relset rs;
  init_relset(&rs);

  char* buf = (char*) malloc(BUFFER_SIZE);
  while (process_command(&rs, buf));

  free(buf);
  destroy_relset(&rs);
  return 0;
}
