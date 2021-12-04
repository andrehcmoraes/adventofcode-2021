#include<stdio.h>
#include<stdlib.h>

#define MAX_X 1000
#define MAX_Y 1000


#define SWAP(a,b,c) c = a; a = b; b = c;
#define MIN(a, b, c) if(a > b)  { SWAP(a,b,c) }

int main() {
  int x1,x2;
  int y1,y2;
  int ax;
  int n = 0;

  int **map = NULL;
  map = (int **)malloc(MAX_Y * sizeof(*map));
  for(int i=0; i<MAX_Y; i++) {
    map[i] = (int *)malloc(MAX_X * sizeof(*(map[i])));
    for(int j=0; j<MAX_X; j++)
      map[i][j] = 0;
  }
  
  
  while(fscanf(stdin, " %d,%d -> %d,%d", &x1, &y1, &x2, &y2) > 0) {
    if(x1 == x2) {
      MIN(y1,y2,ax);
      for(int i=y1; i<=y2; i++) {
        map[i][x1]++;
        // Only add once
        if(map[i][x1] == 2) n++;
      }
    } else if(y1 == y2) {
      MIN(x1,x2,ax);
      for(int j=x1; j<=x2; j++) {
        map[y1][j]++;
        // Only add once
        if(map[y1][j] == 2) n++;
      }
    }
  }
  printf("At least two =%d\n", n);

  for(int i=0; i<MAX_Y; i++) {
    free(map[i]);
  }
  free(map);
  return 0;
}
