#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_COLS 100
#define MAX_ROWS 100
#define MAX_HEIGHT 9
#define NUM_BASINS 3

#define UNMARKED(x) (x < MAX_HEIGHT)
#define MARK(x) x = MAX_HEIGHT;

// Should use a Max Heap - insertion sort for now
void update_basins(unsigned long long *basins, int b) {
  int i;
  for(i=0; i<NUM_BASINS; i++)
    if(b < basins[i]) break;
  
  if(i==0) return;
  i--;
  for(int j=0; j<i; j++)
    basins[j] = basins[j+1];
  
  basins[i] = b;
}

// Should not exceed stack limit
unsigned long long find_basin(int **map, int i, int j, int r, int c) {
  
  if(!(UNMARKED(map[i][j]))) return 0;
  
  // This will destroy original map
  MARK(map[i][j]);
  unsigned long long n = 1;
  
  if(i>0) if(UNMARKED(map[i-1][j])) n += find_basin(map, i-1, j, r, c);
  if(j>0) if(UNMARKED(map[i][j-1])) n += find_basin(map, i, j-1, r, c);
  if(i<r) if(UNMARKED(map[i+1][j])) n += find_basin(map, i+1, j, r, c);
  if(j<c) if(UNMARKED(map[i][j+1])) n += find_basin(map, i, j+1, r, c);
  
  return n;
}

int main() {
  char *buff;
  int **map;
  unsigned long long basins[NUM_BASINS];

  buff = (char *)malloc((MAX_COLS+1) * sizeof(*buff));
  map = (int **)malloc(MAX_ROWS * sizeof(*map));
  for(int i=0; i<MAX_ROWS; i++)
    map[i] = (int *)malloc(MAX_COLS * sizeof(*(map[i])));
  for(int i=0; i<NUM_BASINS; i++)
    basins[i] = 0;

  int rows, cols;
  rows = cols = 0;
  while(scanf(" %s", buff) != EOF) {
    if(!cols) cols = strlen(buff);
    for(int j=0; j<cols; j++)
      map[rows][j] = (int)(buff[j] - '0');
    rows++;
  }


  for(int i=0; i<rows; i++) {
    for(int j=0; j<cols; j++) {
      unsigned long long b = find_basin(map, i, j, rows-1, cols-1);
      if(!b) continue;
      update_basins(basins, b);
    }
  }
    
  unsigned long long sum = 1;
  for(int i=0; i<NUM_BASINS; i++)
    sum = sum * basins[i];
  printf("Sum of top %d basins: %llu\n", NUM_BASINS, sum);
  
  for(int i=0; i<MAX_ROWS; i++)
    free(map[i]);
  free(map);
  free(buff);
  
  return 0;
}
