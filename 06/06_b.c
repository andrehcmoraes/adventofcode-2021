#include<stdio.h>
#include<stdlib.h>


#define NUM_DAYS 256
#define TIMER_DELAY 2
#define TIMER_PERIOD 6

int main() {
  
  unsigned long long sum, aux;
  
  unsigned long long timers[NUM_DAYS];
  
  for(int i=0; i<NUM_DAYS; i++)
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
    if(timers[i] == 0) continue;
    
    unsigned long long births = timers[i];
    n = i + TIMER_PERIOD+1;
    if(n < NUM_DAYS) timers[n] += timers[i];
    n = i + TIMER_DELAY + TIMER_PERIOD+1;
    if(n < NUM_DAYS) timers[n] += timers[i];
    
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
