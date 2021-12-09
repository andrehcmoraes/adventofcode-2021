#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_LEN 7
#define NUM_DIGITS 10
#define NUM_DISPLAY 4

int main() {
  
  char digit[MAX_LEN];
  int disp_sum[MAX_LEN];

  for(int i=0; i<MAX_LEN; i++)
    disp_sum[i] = 0;


  while(scanf(" %s", digit) != EOF) {
    // Skip signals
    if(digit[0] != '|')
      continue;

    // Read display digit
    for(int i=0; i<NUM_DISPLAY; i++) {
      scanf(" %s", digit);
      if(digit[0] == '\n') break;
      long int len = strlen(digit);
      disp_sum[len-1]++;
    }
  }


  // Number of unique digits
  long int sum = disp_sum[1] + disp_sum[2] + disp_sum[3] + disp_sum[6];
  printf("Unique digits: %ld\n", sum);
  
  return 0;
}
