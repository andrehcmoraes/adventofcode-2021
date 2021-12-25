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

int match_chunk(char a, char b) {
  switch(b) {
    case ')': RETIFCND((a != '('), 1);
    case ']': RETIFCND((a != '['), 1);
    case '}': RETIFCND((a != '{'), 1);
    case '>': RETIFCND((a != '<'), 1);
  }
  return 0;
}

unsigned long long complete_chunk(char *s, int s_len) {
  unsigned long long val = 0;
  while(s_len > 0) {
    char c = s[--s_len];
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
  char chunks[MAX_LINE];
  unsigned long long misses[MAX_LINE];
  
  for(int i=0; i<MAX_LINE; i++)
    misses[i] = 0;

  int miss_len = 0;
  while(scanf(" %s", buff) != EOF) {
    long int len = strlen(buff);
    int corrupt = 0;
    
    int s_len = 0;
    for(int i=0; i<len; i++) {
      char c = buff[i];
      // Stack left side characters
      if( ISLEFT(c) ) {
        chunks[s_len++] = c;
        continue;
      }
      // Pop last and compare
      char d = chunks[--s_len];
      corrupt = match_chunk(d,c);
      if(corrupt) break;
    }
    // Skip corrupted lines
    if(corrupt) continue;

    // Complete missing
    misses[miss_len++] = complete_chunk(chunks, s_len);
  }
  
    // Sort the lines with missing chunks
  qsort(misses, miss_len, sizeof(*misses), miss_cmb);

  // 3042730309
  printf("Middle score of missing lines: %llu\n", misses[miss_len/2]);
  
  return 0;
}
