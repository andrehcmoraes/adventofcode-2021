#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_LINE 110

#define ISLEFT(c)  (c == '(' || c == '[' || c == '{' || c == '<' )
#define RETIFCND(c, r) { if(c) return r; else break; }

#define PARENTHESISERR 3
#define SQREBRACKETERR 57
#define CURLBRACKETERR 1197
#define GREATERTHANERR 25137

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
    case ')': RETIFCND(a != '(', PARENTHESISERR);
    case ']': RETIFCND(a != '[', SQREBRACKETERR);
    case '}': RETIFCND(a != '{', CURLBRACKETERR);
    case '>': RETIFCND(a != '<', GREATERTHANERR);
  }
  return 0;
}

int main() {
  char buff[MAX_LINE];
  stack_t *chunks = NULL;
  unsigned long long sum = 0;
  
  while(scanf(" %s", buff) != EOF) {
    long int len = strlen(buff);
    int val = 0;
    
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
      val = match_chunk(d,c);
      if(val) break;
    }
    sum += val;
  }
  
  printf("Sum of syntax error points: %llu\n", sum);
  
  stack_free(&chunks);
  return 0;
}
