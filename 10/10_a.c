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

void stack_free(stack_t **s, void (*f)(void*)) {
  if(f == NULL) f = free;
  while(*s != NULL) {
    stack_t *t = *s;
    *s = (*s)->next;
    if(t->val != NULL) f(t->val);
    free(t);
  }
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

void char_add(stack_t **h, char c) {
  char *s = (char *)malloc(sizeof(*s));
  *s = c;
  stack_append(h, (void *)s);
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
    
    stack_free(&chunks, NULL);
    for(int i=0; i<len; i++) {
      char c = buff[i];
      // Stack left side characters
      if( ISLEFT(c) ) {
        char_add(&chunks, c);
        continue;
      }
      // Pop last and compare
      char *d = (char *)stack_pop(&chunks);
      val = match_chunk(*d,c);
      free(d);
      if(val) break;
    }
    sum += val;
  }
  
  printf("Sum of syntax error points: %llu\n", sum);
  
  stack_free(&chunks, NULL);
  return 0;
}
