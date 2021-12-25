#include<stdio.h>


int main() {
  int prev = -1;
  int depth = 0;
  int res = 0;
  
  while(scanf(" %d", &depth) > 0) {
    prev = prev < 0 ? depth : prev;
    if(prev < depth) res++;
    prev = depth;
  }
  // 1451
  printf("Total increased: %d\n", res);
  
  return 0;
}
