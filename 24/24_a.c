#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// This algorithm is ready for brute-force, but requires 
// input insights in order to solve in a reasonable time

// Based on input
// i[4] == i[3] -2 
// i[7] == i[6] -4
// i[9] == i[8] -8
// i[10] = i[5] +6
// i[11] = i[2] +7
// i[12] = i[1] +8
// i[13] = i[0] -6

#define MAX_VAR 4
#define MAX_LITERAL_LEN 5
#define MODEL_LEN 14
#define OP_LEN 3

#define MAX_LINE_LEN (OP_LEN + 2*MAX_LITERAL_LEN + 2)

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

void find_monad(int maxcode[], stack_t *monad) {
  char buffer[1+MAX_LITERAL_LEN];
  char input[1+MAX_LINE_LEN];

  int var[MAX_VAR];
  int code[MODEL_LEN] = {0};

  stack_t *m;
  int len;
  char a, b;
  
  for(int i0=9; i0>=7; i0--) {
  for(int i1=1; i1>=1; i1--) {
  for(int i2=2; i2>=1; i2--) {
  for(int i3=9; i3>=3; i3--) {
  for(int i5=3; i5>=1; i5--) {
  for(int i6=9; i6>=5; i6--) {
  for(int i8=9; i8>=9; i8--) {
    
    // Based on specific input
    code[ 0] = i0;
    code[ 1] = i1;
    code[ 2] = i2;
    code[ 3] = i3;
    code[ 4] = i3 - 2;
    code[ 5] = i5;
    code[ 6] = i6;
    code[ 7] = i6 - 4;
    code[ 8] = i8;
    code[ 9] = i8 - 8;
    code[10] = i5 + 6;
    code[11] = i2 + 7;
    code[12] = i1 + 8;
    code[13] = i0 - 6;
    
    
    for(int i=0; i<MAX_VAR; i++)
      var[i] = 0;
    
    m = monad;
    len = 0;
    while(m != NULL) {
      strncpy(input, (char *)m->val, MAX_LINE_LEN);
      m = m->next;
      //printf("Read input=%s\n", input);
      if(!strncmp("inp", input, OP_LEN)) {
        if(sscanf(&input[OP_LEN+1], "%c", &a) <= 0) break;
        var[a-'w'] = code[len];
        len++; 
      } else if(!strncmp("add", input, OP_LEN)) {
        if(sscanf(&input[OP_LEN+1], "%c", &a) <= 0) break;
        if(sscanf(&input[OP_LEN+2], "%s", buffer) <= 0) break;
        b = buffer[0];
        if(b >='w' && b - 'w' < MAX_VAR)
          var[a-'w'] = var[a-'w'] + var[b-'w'];
        else
          var[a-'w'] = var[a-'w'] + atoi(buffer);
      } else if(!strncmp("mul", input, OP_LEN)) {
        if(sscanf(&input[OP_LEN+1], "%c", &a) <= 0) break;
        if(sscanf(&input[OP_LEN+2], "%s", buffer) <= 0) break;
        b = buffer[0];
        if(b >='w' && b - 'w' < MAX_VAR)
          var[a-'w'] = var[a-'w'] * var[b-'w'];
        else
          var[a-'w'] = var[a-'w'] * atoi(buffer);
      } else if(!strncmp("div", input, OP_LEN)) {
        if(sscanf(&input[OP_LEN+1], "%c", &a) <= 0) break;
        if(sscanf(&input[OP_LEN+2], "%s", buffer) <= 0) break;
        b = buffer[0];
        if(b >='w' && b - 'w' < MAX_VAR)
          var[a-'w'] = var[a-'w'] / var[b-'w'];
        else
          var[a-'w'] = var[a-'w'] / atoi(buffer);
      } else if(!strncmp("mod", input, OP_LEN)) {
        if(sscanf(&input[OP_LEN+1], "%c", &a) <= 0) break;
        if(sscanf(&input[OP_LEN+2], "%s", buffer) <= 0) break;
        b = buffer[0];
        if(b >='w' && b - 'w' < MAX_VAR)
          var[a-'w'] = var[a-'w'] % var[b-'w'];
        else
          var[a-'w'] = var[a-'w'] % atoi(buffer);
      } else if(!strncmp("eql", input, OP_LEN)) {
        if(sscanf(&input[OP_LEN+1], "%c", &a) <= 0) break;
        if(sscanf(&input[OP_LEN+2], "%s", buffer) <= 0) break;
        b = buffer[0];
        if(b >='w' && b - 'w' < MAX_VAR)
          var[a-'w'] = (var[a-'w'] == var[b-'w']);
        else
          var[a-'w'] = (var[a-'w'] == atoi(buffer));
      }
    }
    
    if(var['z'-'w'] == 0) {
      for(int i=0; i<MODEL_LEN; i++)
        maxcode[i] = code[i];
      return;
    }
    
    
  }}}}}}}
}

int main() {
  char input[1+MAX_LINE_LEN];

  int maxcode[MODEL_LEN] = {0};

  stack_t *monad = NULL;
  while(fgets(input, MAX_LINE_LEN, stdin) != NULL) {
    char *line = (char *)malloc((1+MAX_LINE_LEN) * sizeof(*line));
    strncpy(line, input, MAX_LINE_LEN);
    line[MAX_LINE_LEN] = '\0';
    stack_append(&monad, (void *)line);
  }
  stack_reverse(&monad);

  
  find_monad(maxcode, monad);

  // 91297395919993
  printf("Highest accepted model number: ");
  for(int i=0; i<MODEL_LEN; i++)
    printf("%d", maxcode[i]);
  printf("\n");

  stack_free(&monad, NULL);
  
  return 0;
}