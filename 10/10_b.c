#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_LINE 110

#define ISLEFT(c)  (c == '(' || c == '[' || c == '{' || c == '<' )
#define RETIFCND(c, r) { if(c) return r; else break; }
#define SETANDBR(a,b) { a = b; break; }

#define PARENTHESISVAL 1
#define SQREBRACKETVAL 2
#define CURLBRACKETVAL 3
#define GREATERTHANVAL 4

#define MISSINGPENALTY 5

typedef struct stack {
  char c;
  struct stack *next;
} stack_t;

void stack_append(stack_t **s, char c) {
  stack_t *t = (stack_t *)malloc(sizeof(*t));
  t->next = *s;
  t->c = c;
  *s = t;
  return;
}

char stack_pop(stack_t **s) {
  stack_t *t = *s;
  *s = (*s)->next;
  char c = t->c;
  free(t);
  return c;
}

void stack_free(stack_t **s) {
  while(*s != NULL) {
    stack_t *t = *s;
    *s = (*s)->next;
    free(t);
  }
}

int match_chunk(char a, char b) {
  switch(b) {
    case ')': RETIFCND((a != '('), 1);
    case ']': RETIFCND((a != '['), 1);
    case '}': RETIFCND((a != '{'), 1);
    case '>': RETIFCND((a != '<'), 1);
  }
  return 0;
}

unsigned long long complete_chunk(stack_t **s) {
  unsigned long long val = 0;
  while(*s != NULL) {
    char c = stack_pop(s);
    unsigned long long delta = 0;
    switch(c) {
      case '(': SETANDBR(delta, PARENTHESISVAL);
      case '[': SETANDBR(delta, SQREBRACKETVAL);
      case '{': SETANDBR(delta, CURLBRACKETVAL);
      case '<': SETANDBR(delta, GREATERTHANVAL);
      }
    
    val *= MISSINGPENALTY;
    val += delta;
  }
  return val;
}

int miss_cmb(const void *a, const void *b) {
  unsigned long long va = (*(unsigned long long *)(a));
  unsigned long long vb = (*(unsigned long long *)(b));
  if(va < vb) return -1;
  if(va > vb) return 1;
  return 0;
}

int main() {
  char buff[MAX_LINE];
  unsigned long long misses[MAX_LINE];
  
  stack_t *chunks = NULL;
  for(int i=0; i<MAX_LINE; i++)
    misses[i] = 0;

  int miss_len = 0;
  while(scanf(" %s", buff) != EOF) {
    long int len = strlen(buff);
    int corrupt = 0;
    
    stack_free(&chunks);
    for(int i=0; i<len; i++) {
      char c = buff[i];
      // Stack left side characters
      if( ISLEFT(c) ) {
        stack_append(&chunks, c);
        continue;
      }
      // Pop last and compare
      char d = stack_pop(&chunks);
      corrupt = match_chunk(d,c);
      if(corrupt) break;
    }
    // Skip corrupted lines
    if(corrupt) continue;

    // Complete missing
    misses[miss_len++] = complete_chunk(&chunks);
  }
  
    // Sort the lines with missing chunks
  qsort(misses, miss_len, sizeof(*misses), miss_cmb);

  printf("Middle score of missing lines: %llu\n", misses[miss_len/2]);
  
  stack_free(&chunks);
  return 0;
}
