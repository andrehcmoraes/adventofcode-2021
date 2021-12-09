#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_COLS 100
#define MAX_ROWS 100

#define CHECK_NEAR(r,v,c,m,i,j) { if(c) r = m[i][j] <= v ? 0 : r; }

int is_risk(int **map, int i, int j, int r, int c) {
  int n = 1;
  int v = map[i][j];
  CHECK_NEAR(n, v, (i>0), map, i-1, j);
  CHECK_NEAR(n, v, (j>0), map, i, j-1);
  CHECK_NEAR(n, v, (i<r), map, i+1, j);
  CHECK_NEAR(n, v, (j<c), map, i, j+1);
  
  n = n > 0 ? v+1 : 0;
  return n;
}

int main() {
  char *buff;
  int **map;

  buff = (char *)malloc((MAX_COLS+1) * sizeof(*buff));
  map = (int **)malloc(MAX_ROWS * sizeof(*map));
  for(int i=0; i<MAX_ROWS; i++)
    map[i] = (int *)malloc(MAX_COLS * sizeof(*(map[i])));


  int rows, cols;
  rows = cols = 0;
  while(scanf(" %s", buff) != EOF) {
    if(!cols) cols = strlen(buff);
    for(int j=0; j<cols; j++)
      map[rows][j] = (int)(buff[j] - '0');
    rows++;
  }
  
  unsigned long long sum = 0;
  for(int i=0; i<rows; i++) {
    for(int j=0; j<cols; j++)
      sum += is_risk(map, i, j, rows-1, cols-1);
  }
    
  
  printf("Sum of risk points: %llu\n", sum);
  
  for(int i=0; i<MAX_ROWS; i++)
    free(map[i]);
  free(map);
  free(buff);

  
  return 0;
}
