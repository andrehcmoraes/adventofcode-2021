#include<stdio.h>
#include<stdlib.h>


#define NUM_DAYS 80
#define TIMER_DELAY 2
#define TIMER_PERIOD 6
#define TIMER_TOTAL TIMER_PERIOD+TIMER_DELAY+1

int main() {
  
  unsigned long long sum, aux;
  
  unsigned long long timers[TIMER_TOTAL];
  for(int i=0; i<TIMER_TOTAL; i++)
    timers[i] = 0;
  
  // Read until tail non-comma
  int n;
  char c;
  sum = 0;
  while(fscanf(stdin, " %d%c", &n, &c) > 0) {
    sum++;
    timers[n]++;
    if(c != ',') break;
  }

  
  for(int i=0; i<NUM_DAYS; i++) {
    unsigned long long births = timers[0];
    
    for(int j=0; j<TIMER_TOTAL-1; j++) 
      timers[j] = timers[j+1];
    
    timers[TIMER_PERIOD+TIMER_DELAY] = births;
    timers[TIMER_PERIOD] += births;
    aux = sum;
    sum += births;
    
    // Hopefully won't ever happen...
    if(sum < aux) {
      printf("[!] OVERFLOW DETECTED!!!\nTime for BIGNUM :( \n");
      return -1;
    }
  }
  
  printf("Fish at %d days: %llu\n", NUM_DAYS, sum);
  
  return 0;
}
