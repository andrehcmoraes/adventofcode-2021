#include<stdio.h>
#include<stdlib.h>

#define MAX_LITERAL 9
#define EXPLODE_LEVEL 4

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
  if(*s != NULL) {
    if((*s)->prev != NULL) {
      (*s)->prev->next = t;
      t->prev = (*s)->prev;
    }
    (*s)->prev = t;
  }
  *s = t;
}

void *stack_get(stack_t *s, int n) {
  stack_t *t = s;
  for(int i=0; i<n; i++) {
    t = t->next;
    if(t == NULL) return NULL;
  }
  return t->val;
}

void *stack_pop(stack_t **h) {
  stack_t *s = *h;
  if(s == NULL) return NULL;
  void *v = s->val;
  (*h) = (*h)->next;
  if(*h != NULL) (*h)->prev = NULL;
  free(s);
  return v;
}

typedef struct pair {
  struct pair *left, *right, *parent;
  unsigned long long level;
  int val;
} pair_t;

pair_t *pairs_new(unsigned long long level, int val, pair_t *parent) {
  pair_t *p = (pair_t *)malloc(sizeof(*p));
  p->level = level;
  p->val = val;
  p->parent = parent;
  p->left = NULL;
  p->right = NULL;
  return p;
}

void pairs_update(pair_t *p) {
  if(p == NULL) return;
  pairs_update(p->left);
  pairs_update(p->right);
  p->level++;
}

pair_t *pairs_add(pair_t *a, pair_t *b) {
  pair_t *p = pairs_new(0, -1, NULL);
  p->left = a;    
  p->right = b;

  a->parent = p;
  b->parent = p;

  pairs_update(a);
  pairs_update(b);
  
  return p;
}

unsigned long long pairs_magnitude(pair_t *p) {
  if(p == NULL) return 0;
  if(p->val >= 0) return p->val;
  
  return 3*pairs_magnitude((pair_t *)p->left) + 2*pairs_magnitude((pair_t *)p->right);
}

void pairs_append(stack_t **s, int l, int v) {
  pair_t *p = pairs_new(l, v, NULL);
  stack_append(s, (void *)p);
}


pair_t *pairs_copy(pair_t *p) {
  if(p == NULL) return NULL;
  
  pair_t *v = pairs_new(p->level, p->val, NULL);
  v->left = pairs_copy(p->left);
  v->right = pairs_copy(p->right);

  if(v->left != NULL) v->left->parent = v;
  if(v->right != NULL) v->right->parent = v;

  return v;
}

void pairs_free(pair_t **p) {
  if(*p == NULL) return;
  pairs_free((pair_t **)(&(*p)->left));
  pairs_free((pair_t **)(&(*p)->right));
  free(*p);
  *p = NULL;
}

void pairs_split(pair_t *p) {
  int n = p->val;
  p->left = pairs_new(p->level + 1, n/2, p);
  p->right = pairs_new(p->level + 1, n - n/2, p);
  p->val = -1;
}

void pairs_explode(pair_t *p) {
  int left_val, right_val, was_left;
  pair_t *left_bro, *right_bro;
  pair_t *parent, *prev;


  parent = p->parent;
  prev = p;
  left_val = ((pair_t *)p->left)->val;
  right_val = ((pair_t *)p->right)->val;
  left_bro = right_bro = NULL;

  // Find the left and right of the exploded element
  while(parent != NULL) {
    if(left_bro != NULL && right_bro != NULL) break;
    
    was_left = (pair_t *)parent->left == prev;
    if(was_left && right_bro == NULL)
      right_bro = (pair_t *)parent->right;
    if(!was_left && left_bro == NULL)
      left_bro = (pair_t *)parent->left;
    
    prev = parent;
    parent = (pair_t *)parent->parent;
  }

  // Find the first regular number to the left
  if(left_bro != NULL) {
    while(left_bro->val < 0)
      left_bro = (pair_t *)left_bro->right;
    
    left_bro->val += left_val;
  }
  if(right_bro != NULL){
    while(right_bro->val < 0)
      right_bro = (pair_t *)right_bro->left;
    
    right_bro->val += right_val;
  }
  
  // Free the former left and right nodes
  pairs_free((pair_t **)(&(p->left)));
  pairs_free((pair_t **)(&(p->right)));
  
  p->val = 0;
}

int pairs_reduce_explode(pair_t *p) {
  if(p == NULL) return 0;

  // Skip leaf node
  if(p->val >= 0) return 0;

  
  // Leftmost first
  if(pairs_reduce_explode(p->left)) return 1;

  // Explode
  if(p->level >= EXPLODE_LEVEL) {
    pairs_explode(p);
    return 1;
  }

  // Rightmost last
  if(pairs_reduce_explode(p->right)) return 1;
  
  // Nothing happens, feijoada
  return 0;
}

int pairs_reduce_split(pair_t *p) {
  if(p == NULL) return 0;

  // Leftmost first
  if(p->val < 0)
      if(pairs_reduce_split(p->left)) return 1;

  // Split
  if(p->val > MAX_LITERAL) {
    pairs_split(p);
    return 1;
  }

  // Rightmost last
  if(p->val < 0)
    if(pairs_reduce_split(p->right)) return 1;
  
  // Nothing happens, feijoada
  return 0;

}

int pairs_read(pair_t **pp) {
  int level;
  int line, eof;
  char c;
  pair_t *p, *v, *prev;
  stack_t *values, *pairs;

  pairs = values = NULL;
  prev = *pp;
  eof = 1;
  line = 0;
  level = 0;
  while(scanf("%c", &c) > 0) {
    switch(c){
      case '\n': line=1; break;
      case '[':
        // Append new pair
        pairs_append(&pairs, level, -1);
        level++;
        eof = 0;
        break;
      case ',':
        // Finished left side of topmost pair
        v = (pair_t *)stack_pop(&values);
        p = (pair_t *)pairs->val;
        p->left = v;
        break;
      case ']':
         // Finished right side of topmost pair
        p = (pair_t *)stack_pop(&pairs);
        v = (pair_t *)stack_pop(&values);
        p->right = v;
        p->parent = NULL;
        if(pairs != NULL) p->parent = (pair_t *)pairs->val;
        stack_append(&values, (void *)p);
        level--;
        break;
      default:
        // Read a literal value
        pairs_append(&values, level, c - '0');
        // Parent is topmost pair
        if(pairs != NULL)
          ((pair_t *)(values->val))->parent = (pair_t *)pairs->val;
        break;
    }
    if(line) break;
  }

  // If eof was reached
    if(eof)
      return 0;
  
  v = (pair_t *)stack_pop(&values);
  *pp = v;
  
  if(prev != NULL) {
    // If previous pair exists, add it with current pair
    *pp = pairs_add(prev, v);
  }
  
  return 1;
}

int main() {
  stack_t *numbers = NULL;
  pair_t *p = NULL;
  int x, y, n;
  x = 1;
  n = 0;
  while(x) {
    x = pairs_read(&p);
    if(p == NULL) continue;
    
    stack_append(&numbers, (void *)p);
    n++;
    p = NULL;
  }
  
  unsigned long long max = 0;
  // Slowwww
  for(int i=0; i<n; i++) {
    for(int j=0; j<n; j++) {
      if(i == j) continue;
      pair_t *a = (pair_t *)stack_get(numbers, i);
      pair_t *b = (pair_t *)stack_get(numbers, j);
      
      pair_t *aa = pairs_copy(a);
      pair_t *bb = pairs_copy(b);
      
      p = pairs_add(aa, bb);
      do {
        y = pairs_reduce_explode(p);
        if(y) continue;
        y = pairs_reduce_split(p);
      } while(y);
      
      unsigned long long ans = pairs_magnitude(p);
      max = ans > max ? ans : max;
      pairs_free(&p);
    }
  }
  
  // 4687
  printf("Max Magnitude: %llu\n", max);
  
  while(numbers!= NULL) {
    stack_t *t = numbers;
    numbers = numbers->next;
    if(t->val != NULL) pairs_free((pair_t **)&(t->val));
    free(t);
  }
  
  return 0;
}