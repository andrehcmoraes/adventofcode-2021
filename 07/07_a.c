#include<stdio.h>
#include<stdlib.h>

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
    sum += delta;
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
  
  
  // While progressing through the sorted vector, 
  // solution will gradually improve as we come closer to the ideal solution
  // Then, it will gradually worsen the further away we go from it
  
  // Find the interval between the crabs where the turning point is,
  // The first time where the solution worsens
  
  int x = 0;
  unsigned long fuel = crab_fuel(crabs[0], crabs, crab_len);
  for(x=0; x<crab_len; x++) {
    // Skip crabs at same pos
    if(x<crab_len-1) if(crabs[x] == crabs[x+1]) continue;
    
    unsigned long f = crab_fuel(crabs[x], crabs, crab_len);
    if(f > fuel) break;
    fuel = f;
  }
  
  // Solution will be between the two crabs where the turning point happened
  int x1, x2;
  x1 = x > 1 ? x - 1 : x;
  x2 = x;
  
  // Now iterate position by position 
  // Binary search could improve this
  fuel = crab_fuel(crabs[x1], crabs, crab_len);
  for(int p=crabs[x1]; p<crabs[x2]; p++) {
    unsigned long f = crab_fuel(p, crabs, crab_len);
    
    if(f > fuel) break;
    fuel = f;
  }
  // 343468
  printf("Best fuel: %lu\n", fuel);
  free(crabs);
  return 0;
}
