#include<stdio.h>
#include<stdlib.h>

// Should be ideally at least 2*MAX_POINTS
#define HASHSET_SIZE 100000

// Struct for the points
typedef struct point {
  unsigned long long x,y;
} point_t;


// Scuffed hash - hash64shift
unsigned long long hash64shift(unsigned long long key) {
  key = (~key) + (key << 21); // key = (key << 21) - key - 1;
  key = key ^ (key >> 24);
  key = (key + (key << 3)) + (key << 8); // key * 265
  key = key ^ (key >> 14);
  key = (key + (key << 2)) + (key << 4); // key * 21
  key = key ^ (key >> 28);
  key = key + (key << 31);
  return key;
}

// Make sure (0,1) does not have the same hash as (1,0)
unsigned long long point_hash(point_t *p) {
  return hash64shift(p->x) ^ p->y;
}

// Since the keys are already a hash, just return them
unsigned long long point_h(void *k){
  return *((unsigned long long *)k);
}

// Function to see if two points are the same
// Since hashes are used as keys, just compare them
int point_eq(void *a, void *b) {
  unsigned long long *hash_a = (unsigned long long *) a;
  unsigned long long *hash_b = (unsigned long long *) b;
  return *hash_a == *hash_b;
}

// Make a new point and calculate its key, which is its hash
point_t *point_new(unsigned long long x, unsigned long long y, unsigned long long **key) {
  point_t *p = (point_t *)malloc(sizeof(*p));
  *key = (unsigned long long *)malloc(sizeof(**key));
  p->x = x;
  p->y = y;
  **key = point_hash(p);
  return p;
}


// Double linked stack
typedef struct stack {
  void *val;
  struct stack *next, *prev;
} stack_t;

void stack_append(stack_t **s, void *v) {
  stack_t *t = (stack_t *)malloc(sizeof(*t));
  t->next = *s;
  t->prev = NULL;
  t->val = v;
  if(*s != NULL) (*s)->prev = t;
  *s = t;
}

void stack_pop(stack_t **h, stack_t *s) {
  if(s->next != NULL) s->next->prev = s->prev;
  if(s->prev != NULL) s->prev->next = s->next;
  if(s->val  != NULL) free(s->val);
  if(*h == s) *h = s->next;
  free(s);
}

void stack_free(stack_t **s) {
  while(*s != NULL) {
    stack_t *t = *s;
    *s = (*s)->next;
    if(t->val != NULL) free(t->val);
    free(t);
  }
}

// Dictionary with stack to handle collision
typedef struct node_dict {
  void *key, *value;
  stack_t *key_pos, *self;
} node_dict_t;

typedef struct dictionary {
  stack_t **pairs;
  stack_t *keys;
  unsigned long long (*hash)(void*);
  int (*key_eq)(void*, void*);
  unsigned long long size, len;
} dict_t;


void dict_init(dict_t **d, unsigned int size, unsigned long long (*h)(void*), int (*e)(void*,void*)) {
  dict_t *t;
  t = (dict_t *)malloc(sizeof(*t));
  t->size = size;
  t->key_eq = e;
  t->hash = h;
  t->len = 0;
  t->keys = NULL;
  t->pairs = (stack_t **)malloc(size * sizeof(*(t->pairs)));
  for(int i=0; i<size; i++)
    t->pairs[i] = NULL;
  *d = t;
}

node_dict_t *dict_get(dict_t *d, void *key) {
  unsigned int h = d->hash(key);
  h = h % d->size;
  
  stack_t *t = d->pairs[h];
  while(t != NULL) {
    node_dict_t *e = (node_dict_t *) t->val;
    if(d->key_eq(e->key, key)) return e;
    t = t->next;
  }
  return NULL;
}

void dict_set(dict_t *d, void *key, void *val) {
  node_dict_t *e = (node_dict_t *) dict_get(d, key);
  if(e != NULL) {
    if(e->value) free(e->value);
    if(e->key) free(e->key);
    e->value = val;
    e->key = key;
    if(e->key_pos) {
      stack_t *t =  e->key_pos;
      if(t->val) free(t->val);
      t->val = key;
    }
    e->key_pos = key;
    
    return;
  }
  
  unsigned int h = d->hash(key);
  h = h % d->size;
  
  stack_append(&(d->keys), key);
  
  node_dict_t *new_e = (node_dict_t *) malloc(sizeof(*new_e));
  new_e->key = key;
  new_e->value = val;
  new_e->key_pos = d->keys;
  
  stack_append(&(d->pairs[h]), new_e);
  new_e->self = d->pairs[h];
  d->len++;
}

void dict_del(dict_t *d, void *key) {
  node_dict_t *e = (node_dict_t *) dict_get(d, key);
  if(e == NULL)
    return;
  
  unsigned int h = d->hash(key);
  h = h % d->size;

  if(e->value) free(e->value);
  stack_pop(&(d->keys), e->key_pos);
  stack_pop(&(d->pairs[h]), e->self);
  d->len--;
}

void dict_free(dict_t **d) {
  dict_t *t = *d;
  for(int i=0; i<t->size; i++) {
    stack_t *s = t->pairs[i];
    while(s != NULL) {
      node_dict_t *e = (node_dict_t *) s->val;
      if(e->value != NULL) free(e->value);
      free(e);
      stack_t *x = s;
      s = s->next;
      free(x);
    }
  }
  free(t->pairs);
  stack_free(&(t->keys));
  free(t);
  *d = NULL;
}

void points_fold(dict_t *points_set, unsigned long long *len_x, 
    unsigned long long *len_y, unsigned long long n, int is_ver) {
  
  // For each point, see if they lie beyond the soon-to-be excluded zone
  point_t p1;
  stack_t *t = points_set->keys;
  while(t != NULL) {
    unsigned long long *key = (unsigned long long *)(t->val);
    node_dict_t *e0 = dict_get(points_set, (void *)key);
    point_t *p0 = (point_t *) e0->value;
    stack_t *next = t->next;
    int apply = 0;
    if(is_ver && p0->y > n) {
      // Vertical fold
      // Set point to look for
      p1.x = p0->x;
      p1.y = 2*n - p0->y;
      apply = 1;
    } else if(!is_ver && p0->x > n) {
      // Horizontal fold
      p1.x = 2*n - p0->x;
      p1.y = p0->y;
      apply = 1;
    }

    if(apply) {
      // See if point p1 already exists, add it if it doesn't
      unsigned long long k = point_hash(&p1);
      node_dict_t *e1 = dict_get(points_set, (void *)(&k));
      if(e1 == NULL) {
        unsigned long long *key;
        point_t *p = point_new(p1.x, p1.y, &key);
        dict_set(points_set, key, p);
      }
        // Delete current point
      dict_del(points_set, e0->key);
    }
    t = next;
  }

  // Update new lengths
  if(is_ver) *len_y = n;
  else *len_x = n;
}

int main() {
  unsigned long long x,y,n;
  unsigned long long len_x, len_y;
  char c;
  // Set with points
  dict_t *points_set = NULL;
  dict_init(&points_set, HASHSET_SIZE, point_h, point_eq);

  len_x = len_y = 0;
  while(scanf(" %llu,%llu", &x, &y) > 0) {
    unsigned long long *k = NULL;
    point_t *p = point_new(x, y, &k);
    dict_set(points_set, k, p);
  }
  
  // Do folds
  while(scanf(" fold along %c=%llu", &c, &n) > 0) {
    if(c != 'x' && c !='y') {
      printf("[!] Invalid fold %c\n", c);
      exit(-1);
    }
    points_fold(points_set, &len_x, &len_y, n, c == 'y');
  }

  // Build grid
  short **grid;
  grid = (short **)malloc(len_y * sizeof(*grid));
  for(unsigned long long i=0; i<len_y; i++) {
    grid[i] = (short *)malloc(len_x * sizeof(*(grid[i])));
    for(unsigned long long j=0; j<len_x; j++)
      grid[i][j] = 0;
  }
  
  // Apply points
  stack_t *t = points_set->keys;
  while(t != NULL) {
    unsigned long long *k = (unsigned long long *)(t->val);
    node_dict_t *e = dict_get(points_set, (void *)k);
    point_t *p = (point_t *) e->value;
    grid[p->y][p->x] = 1;
    
    t = t->next;
  }
  
  for(unsigned long long y=0; y<len_y; y++) {
    for(unsigned long long x=0; x<len_x; x++) {
      printf("%c", grid[y][x] ? '#' : ' ');
    }
    printf("\n");
  }
  
  for(unsigned long long i=0; i<len_y; i++)
    free(grid[i]);
  free(grid);
  dict_free(&points_set);
  
  return 0;
}