#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define NUM_BITS 12

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

void stack_del(stack_t **h, stack_t *s) {
  if(s->next != NULL) s->next->prev = s->prev;
  if(s->prev != NULL) s->prev->next = s->next;
  if(s->val  != NULL) free(s->val);
  if(*h == s) *h = s->next;
  free(s);
}

// Supporting struct for the stack
typedef struct numbers {
  stack_t *s;
  int len;
  int ones_sum[NUM_BITS];
} numbers_t;

void numbers_add(numbers_t *n, char *bin) {
  char *b = (char *)malloc((NUM_BITS+1)*sizeof(*b));
  for(int i=0; i<NUM_BITS; i++) 
    b[i] = '0';
  strcpy(b, bin);
  stack_append(&(n->s), (void *)b);
}

void numbers_filter(numbers_t **n, int i, int reverse) {
  stack_t *t = (*n)->s;
  char c = 2*(*n)->ones_sum[i] >= (*n)->len ? '1' : '0';
  if(reverse) {
    c = c == '1' ? '0' : '1';
  }
  while(t != NULL) {
    stack_t *a = t->next;
    char *bin = (char *)t->val;
    // If current bit did not match filter, remove it
    if(bin[i] != c) {
      for(int j=i; j<NUM_BITS; j++)
        if(bin[j] == '1') (*n)->ones_sum[j]--;
      
      (*n)->len--;
      stack_del(&((*n)->s), t);
    }
    t = a;
  }
}

void numbers_init(numbers_t **n) {
  numbers_t *t = (numbers_t *)malloc(sizeof(*t));
  t->s = NULL;
  t->len = 0;
  for(int i=0; i<NUM_BITS; i++)
    t->ones_sum[i] = 0;
  *n = t;
}

void numbers_free(numbers_t **n) {
  if(*n == NULL) return;
  stack_free(&((*n)->s), NULL);
  free(*n);
  *n = NULL;
}

static char *FORMAT_READ=" %%%ds";

int main() {
  char buff[NUM_BITS+1];
  char format[strlen(FORMAT_READ) + ((NUM_BITS+1)/10)];
  
  numbers_t *oxy_nums = NULL;
  numbers_t *co2_nums = NULL;
  
  unsigned long oxygen, carbon, mask;
  char *endp = NULL;
  
  for(int i=0; i<NUM_BITS; i++) 
    buff[i] = '0';
  
  numbers_init(&oxy_nums);
  numbers_init(&co2_nums);
  
  sprintf(format, FORMAT_READ, NUM_BITS);
  while(fscanf(stdin, format, buff) > 0) {
    oxy_nums->len++;
    co2_nums->len++;
    for(int i=0; i<NUM_BITS; i++) {
      if(buff[i] == '1') {
        oxy_nums->ones_sum[i]++;
        co2_nums->ones_sum[i]++;
      }
    }
    numbers_add(oxy_nums, buff);
    numbers_add(co2_nums, buff);
  }
  
  for(int i=0; i<NUM_BITS; i++) {
    // Not supposed to happen
    if(oxy_nums->s == NULL || co2_nums->s == NULL) {
      printf("[!] Error: No numbers left\n");
      return -1;
    }
    
    // Stop if both only have one number left
    if(oxy_nums->s->next == NULL && co2_nums->s->next == NULL) break;
    
    // Filter currently lists if they have more than one number
    if(oxy_nums->s->next != NULL) numbers_filter(&oxy_nums, i, 0);
    if(co2_nums->s->next != NULL) numbers_filter(&co2_nums, i, 1);
  }
  
  // atoi doesnt support binary
  char *bin = (char *)oxy_nums->s->val;
  oxygen = strtoul(bin, &endp, 2);
  if(endp == NULL || *endp != '\0') {
    printf("[!] Error, invalid binary\n");
    return -1;
  }
  bin = (char *)co2_nums->s->val;
  carbon = strtoul(bin, &endp, 2);
  if(endp == NULL || *endp != '\0') {
    printf("[!] Error, invalid binary\n");
    return -1;
  }
  
  // Bitwise shaenigans
  mask = (1 << NUM_BITS) - 1;
  oxygen &= mask;
  carbon &= mask;
  printf("oxygen = %lu, carbon =%lu, total=%lu\n", oxygen, carbon, oxygen*carbon);
  
  
  numbers_free(&oxy_nums);
  numbers_free(&co2_nums);
  return 0;
}
