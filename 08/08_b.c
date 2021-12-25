#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_LEN 7
#define NUM_DIGITS 10
#define NUM_DISPLAY 4

int digit_one(char *d, int dlen, char *s) {
  int c = 0;
  for(int i=0; i<dlen; i++)
    if(d[i] == s[0] || d[i] == s[1]) c++;
  return c;
}

int digit_four(char *d, int dlen, char *s) {
  int c = 0;
  for(int i=0; i<dlen; i++) {
    for(int j=0; j<4; j++)
      if(d[i] == s[j]) c++;
  }
  return c;
}


int find_235(char *d, char *s1, char *s4) {
  if(digit_one(d, 5, s1) == 2) return 3;
  if(digit_four(d, 5, s4) == 3) return 5;
  return 2;
}


int find_069(char *d, char *s1, char *s4) {
  if(digit_one(d, 6, s1) != 2) return 6;
  if(digit_four(d, 6, s4) == 4) return 9;
  return 0;
}

int main() {
  
  char digit[MAX_LEN+1];
  char signal_one[MAX_LEN+1];
  char signal_four[MAX_LEN+1];

  unsigned long long sum = 0;
  // Read signals
  while(scanf(" %s", digit) != EOF) {
    if(digit[0] != '|') {
      long int len = strlen(digit);
      if(len == 2) strcpy(signal_one, digit);
      else if(len == 4) strcpy(signal_four, digit);
      continue;
    }
    
    // Read display digit
    unsigned long long display = 0;
    for(int i=0; i<NUM_DISPLAY; i++) {
      scanf(" %s", digit);
      if(digit[0] == '\n') break;

      int d = 0;
      switch(strlen(digit)) {
        case 2:
          d = 1;
          break;
        case 3:
          d = 7;
          break;
        case 4:
          d = 4;
          break;
        case 5:
          d = find_235(digit, signal_one, signal_four); 
          break;
        case 6:
          d = find_069(digit, signal_one, signal_four); 
          break;
        case 7:
          d = 8;
          break;
        default:
          // Despair
          printf("Nao, cara... nao\n");
          break;
      }
      // Add new digit
      display *= 10;
      display += d;
    }
    sum += display;
  }
  // 1028926
  printf("Display sum: %llu\n", sum);
  
  return 0;
}
