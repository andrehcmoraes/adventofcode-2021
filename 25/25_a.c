#include<stdio.h>
#include<stdlib.h>

#define MAX_ROWS 140
#define MAX_COLS 140

unsigned long long cucumbers_east(char **grid, int nrows, int ncols) {
  unsigned long long moved = 0;

  for(int i=0; i<nrows; i++) {
    // Process entire row with placeholders
    for(int j=0; j<ncols; j++) {
      if(grid[i][j] != '>') continue;
      int n=j+1;
      if(n == ncols) n = 0;
      if(grid[i][n] != '.') continue;
      
      grid[i][j] = '?';
      grid[i][n] = '!';
      moved++;
    }
    // Apply placeholders
    for(int j=0; j<ncols; j++) {
      if(grid[i][j] == '?') grid[i][j] = '.';
      else if(grid[i][j] == '!') grid[i][j] = '>';
    }
  }

  return moved;
}

unsigned long long cucumbers_south(char **grid, int nrows, int ncols) {
  unsigned long long moved = 0;

  for(int j=0; j<ncols; j++) {
    // Process entire column with placeholders
    for(int i=0; i<nrows; i++) {
      if(grid[i][j] != 'v') continue;
      int n=i+1;
      if(n == nrows) n = 0;
      if(grid[n][j] != '.') continue;
      
      grid[i][j] = '?';
      grid[n][j] = '!';
      moved++;
    }
    // Apply placeholders
    for(int i=0; i<nrows; i++) {
      if(grid[i][j] == '?') grid[i][j] = '.';
      else if(grid[i][j] == '!') grid[i][j] = 'v';
    }
  }

  return moved;
}


unsigned long long cucumbers_move(char **grid, int nrows, int ncols) {
  unsigned long long steps, moved;
  steps = 0;

  do{
    moved = cucumbers_east(grid, nrows, ncols);
    moved += cucumbers_south(grid, nrows, ncols);
    steps++;
    //break;
  } while(moved > 0);
  
  return steps;
}

int main() {
  
  int nrows, ncols, idx;
  char c;
  char **grid;

  grid = (char **)malloc(MAX_ROWS * sizeof(*grid));
  for(int i=0; i<MAX_ROWS; i++)
    grid[i] = (char *)malloc(MAX_COLS * sizeof(*(grid[i])));

  // Read the initial grid
  nrows = ncols = idx = 0;
  while(scanf("%c", &c) > 0) {
    if(c == '\n') {
      nrows++;
      ncols = idx;
      idx = 0;
      continue;
    }
    grid[nrows][idx] = c;
    idx++;
  }

  unsigned long long res = cucumbers_move(grid, nrows, ncols);
  printf("Steps until no cucumbers move: %llu\n", res); 
  

  for(int i=0; i<MAX_ROWS; i++)
    free(grid[i]);
  free(grid);

  return 0;
}