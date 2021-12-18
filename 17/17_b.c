#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int check(int tmin, int tmax, int vx, int vy, int y1, int y2) {
  int maxy, py;

  maxy = vy > 0 ? (vy*(vy+1))/2 : 0;
  int t = 0;
  int ly = vy;
  py = 0;
  
  // Find the min and max times manually
  while(t < tmax && (py > y2 || py + ly >= y1)) {
    if(t >= tmin && y2 >= py) break;
    py += ly;
    t++;
    ly--;
  }


  // Check if in bounds
  if(t < tmin || t >= tmax || y1 > py || py > y2) return -1;

  return maxy;
}

int find_t(int vx, int x1, int x2, int y1, int y2, int *sum) {
  int tmin = 0;
  int tmax = 0;
  int vmin = y1;
  int vmax = -y1;

  // Find the min and max times manually
  int lx = vx;
  int px = 0;
  while(px <= x2 && lx > 0) {
    if(px < x1) tmin++;
    px += lx;
    tmax++;
    lx--;
  }
  if(lx <= 0) tmax = x2;

  // Lmao bruteforce
  int max = 0;
  int count = 0;
  for(int v=vmin; v<vmax; v++) {
    int x = check(tmin, tmax, vx, v, y1, y2);
    if(x >= 0) {
      count++;
      max = max < x ? x : max;
    }
  }

  *sum += count;
  return max;
}

int main() {
  int x1, x2, y1, y2;

  scanf("target area: x=%d..%d, y=%d..%d", &x1, &x2, &y1, &y2);
  
  int vxmin = (int)floor(sqrt(2*x1));
  
  int max = 0;
  int sum = 0;
  for(int vx=vxmin; vx <= x2; vx++) {
    int x = find_t(vx, x1, x2, y1, y2, &sum);
    max = max < x ? x : max;
  }
  
  printf("Max height: %d\n", max);
  printf("Max values: %d\n", sum);
  
  return 0;
}