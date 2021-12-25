#include<stdio.h>
#include<stdlib.h>

#define BREAK_OR_ASSIGN(a,b,r) { if(r) break; a = b; }

typedef struct stack {
  int pos;
  struct stack *next;
} stack_t;

void stack_append(stack_t **s, int n) {
  stack_t *t = (stack_t *)malloc(sizeof(*t));
  t->next = *s;
  t->pos = n;
  *s = t;
  return;
}

void stack_dump(stack_t **s, int *c) {
  int i = 0;
  while(*s != NULL) {
    stack_t *t = *s;
    *s = (*s)->next;
    c[i++] = t->pos;
    free(t);
  }
}

int crab_cmb (const void *a, const void *b) {
  return ( *((int *)(a)) - *((int *)(b)) );
}

unsigned long crab_fuel(int p, int *c, int l) {
  unsigned long sum = 0;
  
  for(int i=0; i<l; i++) {
    unsigned long delta = c[i] > p ? c[i] - p : p - c[i];
  // Arithmethic Sum
    sum += (delta*(delta+1))/2;
  }
  return sum;
}


int main() {
  
  stack_t *crb = NULL;

  int n;
  char c;
  int crab_len = 0;
  while(fscanf(stdin, " %d%c", &n, &c) > 0) {
    crab_len++;
    stack_append(&crb, n);
  }
  int *crabs = (int *)malloc(crab_len * sizeof(*crabs));
  stack_dump(&crb, crabs);

  // Sort the crabs
  qsort(crabs, crab_len, sizeof(*crabs), crab_cmb);
  
  // Find the global minimum
  int x = 0;
  unsigned long fuel = crab_fuel(crabs[0], crabs, crab_len);
  for(x=0; x<crab_len; x++) {
    // Skip crabs at same pos
    if(x<crab_len-1) if(crabs[x] == crabs[x+1]) continue;
    
    unsigned long f = crab_fuel(crabs[x], crabs, crab_len);
    BREAK_OR_ASSIGN(fuel, f, f>fuel);
  }
  
  int x1, x2;
  x1 = x > 1 ? x - 1 : x;
  x2 = x;
  
  fuel = crab_fuel(crabs[x1], crabs, crab_len);
  for(int p=crabs[x1]; p<crabs[x2]; p++) {
    unsigned long f = crab_fuel(p, crabs, crab_len);
    BREAK_OR_ASSIGN(fuel, f, f>fuel);
  }

  // Binary search... ish
  unsigned long f1, f2;
  int new_f1, new_f2;
  x1 = crabs[x1];
  x2 = crabs[x2];
  f1 = crab_fuel(x1, crabs, crab_len);
  f2 = crab_fuel(x2, crabs, crab_len);
  new_f1 = new_f2 = 0;
  while( x1 != x2 ) {
    if(new_f1) f1 = crab_fuel(x1, crabs, crab_len);
    if(new_f2) f2 = crab_fuel(x2, crabs, crab_len);
    
    int x3 = (x1+x2)/2;
    if(f1 > f2) {
      new_f1 = 1;
      new_f2 = 0;
      fuel = f2;
      
      BREAK_OR_ASSIGN(x1, x3, x1==x3);
    } else {
      new_f1 = 0;
      new_f2 = 1;
      fuel = f1;
      
      BREAK_OR_ASSIGN(x2, x3, x2==x3);
    }
  }
  // 96086265
  printf("Best fuel: %lu\n", fuel);
  free(crabs);
  return 0;
}
