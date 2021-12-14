#include<stdio.h>
#include<stdlib.h>

#define NUM_STEPS 40
#define NUM_ELEMS ('Z'-'A'+1)

// Each rule increases the sum of one element
// And the number of generators for two other rules
typedef struct rule {
  struct rule *out_a, *out_b;
  unsigned long long count, index;
  short in_a, in_b, gen;
} rule_t;

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

void stack_free(stack_t **s) {
  while(*s != NULL) {
    stack_t *t = *s;
    *s = (*s)->next;
    if(t->val != NULL) free(t->val);
    free(t);
  }
}

void rule_link(stack_t *rul, rule_t **grd) {
  stack_t *t = rul;
  while(t != NULL) {
    rule_t *r = *((rule_t **)t->val);
    t = t->next;

    short a = r->in_a;
    short b = r->in_b;
    short c = r->gen;

    r->out_a = &(grd[a][c]);
    r->out_b = &(grd[c][b]);
  }
  
}

void rule_step(stack_t *rul, unsigned long long rlen, unsigned long long *elm, int steps) {
  unsigned long long *delta;
  delta = (unsigned long long *)malloc(rlen * sizeof(*delta));
  for(int i=0; i<rlen; i++)
      delta[i] = 0;

  for(int i=0; i<steps; i++) {
    // Reset previous delta
    stack_t *t = rul;
    while(t != NULL) {
      rule_t *r = *((rule_t **)t->val);
      t = t->next;
      
      // Skip if no generators for this rule
      if(r->count == 0 || r->gen < 0) continue;
      
      // Increase element count by the number of current generators for this rule
      elm[r->gen] += r->count;
      
      // Set to increase the generators next step, if such a rule exists
      rule_t *oa = r->out_a;
      if(oa != NULL)
        delta[oa->index] += r->count;
      
      rule_t *ob = r->out_b;
      if(ob != NULL) 
        delta[ob->index] += r->count;
      
    }
    
    // Iterate again, apply delta
    t = rul;
    while(t != NULL) {
      rule_t *r = *((rule_t **)t->val);
      t = t->next;
      r->count = delta[r->index];
      delta[r->index] = 0;
    }
  }
  free(delta);
}



int main() {
  stack_t *rules;
  unsigned long long rlen;
  unsigned long long *elems;
  rule_t **rules_grid;
  
  rules_grid = (rule_t **)malloc(NUM_ELEMS * sizeof(*rules));
  elems = (unsigned long long *)malloc(NUM_ELEMS * sizeof(*elems));
  for(int i=0; i<NUM_ELEMS; i++) {
    elems[i] = 0;
    rules_grid[i] = (rule_t *)malloc(NUM_ELEMS * sizeof(*(rules_grid[i])));
    for(int j=0; j<NUM_ELEMS; j++) {
      rules_grid[i][j].count = 0;
      rules_grid[i][j].gen = -1;
    }
  }
  
  // Read template
  char a,b,c;
  a = '\0';
  while(scanf("%c", &b) > 0) {
    if(b == ' ' || b == '\t') continue;
    if(a == '\0') {
      a = b;
      continue;
    }
    
    // Count current element
    short ai = a - 'A';
    short bi = b - 'A';
    elems[ai] += 1;
    
    // No pair available
    if(b == '\n' || b == ' ') break;
    
    // Increase amount of generators
    rules_grid[ai][bi].count += 1;
    a = b;
  }
  
  // Read rules
  rules = NULL;
  rlen = 0;
  while(scanf(" %c%c -> %c", &a, &b, &c) > 0) {
    short ai = a - 'A';
    short bi = b - 'A';
    rules_grid[ai][bi].in_a =  ai;
    rules_grid[ai][bi].in_b =  bi;
    rules_grid[ai][bi].gen = c-'A';
    rules_grid[ai][bi].index =  rlen;
    rlen++;

    rule_t **r = (rule_t **)malloc(sizeof(**r));
    *r = &(rules_grid[ai][bi]);
    stack_append(&rules, (void *)(r));
  }
  
  // Link rules to each other
  rule_link(rules, rules_grid);
  
  // Apply steps
  rule_step(rules, rlen, elems, NUM_STEPS);

  // Count lowest and highest
  unsigned long long min = 0 - 1; // Underflow
  unsigned long long max = 0;
  for(int i=0; i<NUM_ELEMS; i++) {
    if(!elems[i]) continue;
    min = min > elems[i] ? elems[i] : min;
    max = max < elems[i] ? elems[i] : max;
  }
  
  printf("Most common minus least common: %llu\n", max-min);
  

  free(elems);
  for(int i=0; i<NUM_ELEMS; i++) {
    free(rules_grid[i]);
  }
  free(rules_grid);
  stack_free(&rules);

  return 0;
}