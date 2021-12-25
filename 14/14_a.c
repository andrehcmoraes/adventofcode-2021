#include<stdio.h>
#include<stdlib.h>

#define NUM_STEPS 10
#define NUM_ELEMS ('Z'-'A'+1)

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

void stack_reverse(stack_t **s) {
  stack_t *t = *s;
  stack_t *tail = t;
  while(t != NULL) {
    stack_t *x = t->next;
    t->next = t->prev;
    t->prev = x;
    tail = t;
    t = x;
  }
  *s = tail;
}

void stack_free(stack_t **s, void (*f)(void*)) {
  if(f == NULL) f = free;
  while(*s != NULL) {
    stack_t *t = *s;
    *s = (*s)->next;
    if(t->val != NULL) f(t->val);
    free(t);
  }
}

void rule_step(stack_t **pol, char **rul) {
  stack_t *t = *pol;
  while(t != NULL) {
    // Break after last pair
    if(t->next == NULL) break;

    stack_t *ta = t;
    stack_t *tb = t->next;
    t = t->next;

    // Check current pair
    char a = *((char *)ta->val);
    char b = *((char *)tb->val);
    
    // Skip if no rule for current pair
    char c = rul[a-'A'][b-'A'];
    if(c == '\0') continue;
    
    // Stack new elem before 'tb'
    char *v = (char *)malloc(sizeof(*v));
    *v = c;
    stack_append(&tb, (void *)v);
  }
}



int main() {
  stack_t *poly;
  unsigned long long *elems;
  char **rules;
  
  rules = (char **)malloc(NUM_ELEMS * sizeof(*rules));
  elems = (unsigned long long *)malloc(NUM_ELEMS * sizeof(*elems));
  for(int i=0; i<NUM_ELEMS; i++) {
    elems[i] = 0;
    rules[i] = (char *)malloc(NUM_ELEMS * sizeof(*(rules[i])));
    for(int j=0; j<NUM_ELEMS; j++)
      rules[i][j] = '\0';
  }
  
  // Read template
  char a,b,c;
  poly = NULL;
  while(scanf("%c", &c) > 0) {
    if(c==' ' || c=='\t') continue;
    if(c=='\n') break;
    
    char *v = (char *)malloc(sizeof(*v));
    *v = c;
    stack_append(&poly, (void *)v);
  }
  // Reverse stack to get proper order
  stack_reverse(&poly);
  
  // Read rules
  while(scanf(" %c%c -> %c", &a, &b, &c) > 0) {
    rules[a-'A'][b-'A'] = c;
  }

  // Apply steps
  for(int i=0; i<NUM_STEPS; i++)
    rule_step(&poly, rules);
  
  // Count elems
  stack_t *t = poly;
  while(t != NULL) {
    char v = *((char *)t->val);
    int i = v - 'A';
    elems[i]++;
    t = t->next;
  }

  // Count lowest and highest
  unsigned long long min = 0 - 1; // Underflow
  unsigned long long max = 0;
  for(int i=0; i<NUM_ELEMS; i++) {
    if(!elems[i]) continue;
    min = min > elems[i] ? elems[i] : min;
    max = max < elems[i] ? elems[i] : max;
  }
  // 2223
  printf("Most common minus least common: %llu\n", max-min);
  
  stack_free(&poly, NULL);  
  free(elems);
  for(int i=0; i<NUM_ELEMS; i++)
    free(rules[i]);
  free(rules);
  
  return 0;
}