#include<stdio.h>

#define MEASURE 3

int main() {
  int sums[MEASURE];
  int prv_sum = 0;
  int cur_sum = 0;
  int depth = 0;
  int idx = 0;
  int res = 0;
  int rnd = 0;

  for(int i=0; i<MEASURE; i++)
    sums[i] = 0;
  
  while(scanf(" %d", &depth) > 0) {
    rnd++;
    int limit = rnd < MEASURE ? rnd : MEASURE;
    for(int i=0; i<limit; i++)
      sums[i] += depth;
    if(limit < MEASURE) continue;
    
    cur_sum = sums[idx];
    if(prv_sum == 0) prv_sum = cur_sum;
    
    if(cur_sum > prv_sum) res++;
    prv_sum = cur_sum;
    sums[idx] = 0;
    idx = (idx + 1) % MEASURE;
  }
  // 1395
  printf("Total increased: %d\n", res);
  
  return 0;
}
