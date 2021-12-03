#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define NUM_BITS 12

static char *FORMAT_READ=" %%%ds";

int main() {
  char buff[NUM_BITS+1];
  char format[strlen(FORMAT_READ) + ((NUM_BITS+1)/10)];

  int ones_sum[NUM_BITS];
  char *endp = NULL;
  
  int n = 0;
  unsigned long res, gamma, epsilon, mask;
  
  for(int i=0; i<NUM_BITS; i++) {
    ones_sum[i] = 0;
    buff[i] = '0';
  }
  
  sprintf(format, FORMAT_READ, NUM_BITS);
  while(fscanf(stdin, format, buff) > 0) {
    n++;
    for(int i=0; i<NUM_BITS; i++)
      if(buff[i] == '1') ones_sum[i]++;
  }
  
  for(int i=0; i<NUM_BITS; i++)
    buff[i] = 2*ones_sum[i] >= n ? '1' : '0';
  buff[NUM_BITS] = '\0';

  // atoi doesnt support binary
  res = strtoul(buff, &endp, 2);
  if(endp == NULL || *endp != '\0') {
    printf("[!] Error, invalid binary\n");
    return -1;
  }
  
  // Bitwise shaenigans
  mask = (1 << NUM_BITS) - 1;
  gamma = res & mask;
  epsilon = (mask ^ gamma) & mask;

  res =  gamma*epsilon;
  printf("gamma * epsilon = %lu\n", res);
  
  return 0;
}
