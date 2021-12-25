#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int check(int t, int vy, int x1, int x2, int y1, int y2) {
  // Could be wrong due to nondiscreet shaenigans
  int py = ((t+1)*(2*vy-t))/2;
  int maxy = vy > 0 ? (vy*(vy+1))/2 : 0;

  // Check if in bounds
  if(y1 > py || py > y2) return -1; 
  
  return maxy;
}

int find_t(int t, int x1, int x2, int y1, int y2) {
  double tt = (double)t;
  double dvy = (y2 + (tt*tt/2.0f))/tt;
  int vy = (int)ceil(dvy);
  
  // I have no idea
  tt = (2*vy + sqrt(4*vy*vy-8*y2))/2.0f;
  
  t = (int) ceil (tt);

  // Because of ceilings, just check everything
  int max = 0;
  for(int i=-1; i<2; i++) {
    for(int j=-1; j<2; j++) {
      int x = check(t+i, vy+j, x1, x2, y1, y2);
      max = max < x ? x : max;
    }
  }
  
  return max;
}

int main() {
  int x1, x2, y1, y2;

  scanf("target area: x=%d..%d, y=%d..%d", &x1, &x2, &y1, &y2);
  
  // Scuff bhaskara
  double tt = (-1 + sqrt(1+8*x1))/2.0f;
  int tmin = (int) ceil (tt);

  int max = 0;
  int had_invalid = 0;
  // Fake heuristic: Max height happens at highest possible time
  // And will fail at most x2 times
  // *** The reader is welcome to prove/disprove these! ***
  for(int t=tmin; had_invalid < x2; t++) {
    int x = find_t(t, x1, x2, y1, y2);
    if(!x) had_invalid++;
    else had_invalid = 0;
    max = max < x ? x : max;
  }
  // 5671
  printf("Max height: %d\n", max);
  
  return 0;
}