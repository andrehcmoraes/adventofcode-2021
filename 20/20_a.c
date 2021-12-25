#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_STEPS 50
#define ALGORITHM_LEN 512

#define GRID_ROWS 100
#define GRID_COLS 100
#define MAX_ROWS (GRID_ROWS+(2*MAX_STEPS))
#define MAX_COLS (GRID_COLS+(2*MAX_STEPS))

#define STEPS 2

static char *FORMAT_READ="%%%ds\n";
int CUR_STEPS = 0;

// Expand grid with extra rows and cols
// Assign the current state of the inifinite numbers
void grid_expand(char z, char **grid, int *nrows, int *ncols) {
  for(int i=*nrows; i>=1; i--) {
    for(int j=*ncols; j>=1; j--) {
      grid[i][j] = grid[i-1][j-1];
    }
  }
  for(int i=0; i<=*nrows+1; i++) {
    grid[i][0] = z;
    grid[i][1+*ncols] = z;
  }
  for(int j=0; j<=*ncols+1; j++) {
    grid[0][j] = z;
    grid[1+*nrows][j] = z;
  }
  *nrows += 2;
  *ncols += 2;
}

// Look up the 3x3 area around a pixel
int grid_lookup(char z, char alg[], char **grid, int ii, int jj, int nrows, int ncols) {
  unsigned long long v = 0;
  for(int i=ii-1; i<=ii+1; i++) {
    for(int j=jj-1; j<=jj+1; j++) {
      v <<= 1;
      if(i >= 0 && i < nrows && j >= 0 && j < ncols)
        v += grid[i][j];
      else
        v += z;
    }
  }
  //if(v>=512) printf("v=%llu\n",v);
  return alg[v] == '#';
}

// Enhance the grid
void grid_enhance(char z, char alg[], char **grid, int nrows, int ncols) {
  char buffer[nrows][ncols];
  for(int i=0; i<nrows; i++) {
    for(int j=0; j<ncols; j++) {
      buffer[i][j] = grid_lookup(z, alg, grid, i, j, nrows, ncols);
    }
  }
  
  for(int i=0; i<nrows; i++) {
    for(int j=0; j<ncols; j++) {
      grid[i][j] = buffer[i][j];
    }
  }
}

// Expand grid and enhance on every step
// Keep track of the infinite numbers state
void grid_steps(int steps, char alg[], char **grid, int *nrows, int *ncols) {
  CUR_STEPS += steps;
  if(CUR_STEPS > MAX_STEPS) {
    printf("[!] Error: Maximum number of steps exceeded\n");
    exit(-1);
  }
  char z = 0;
  for(int i=0; i<steps; i++) {
    grid_expand(z, grid, nrows, ncols);
    grid_enhance(z, alg, grid, *nrows, *ncols);
    // Infinite numbers will alternate between alg[0] and alg[ALGORITHM_LEN]
    z = z == 0 ? alg[0] == '#' : alg[ALGORITHM_LEN] == '#';
  }

}

// Count numbers of lit pixels
unsigned long long grid_count(char **grid, int nrows, int ncols) {
  unsigned long long sum = 0;
  for(int i=0; i<nrows; i++) {
    for(int j=0; j<ncols; j++) {
      if(grid[i][j]) sum++;
    }
  }
  return sum;
}

int main() {
  
  int nrows, ncols, idx;
  char c;
  char **grid;
  char alg[ALGORITHM_LEN+1];
  char format[strlen(FORMAT_READ) + 1];

  grid = (char **)malloc(MAX_ROWS * sizeof(*grid));
  for(int i=0; i<MAX_ROWS; i++)
    grid[i] = (char *)malloc(MAX_COLS * sizeof(*(grid[i])));

  // Read the initial grid
  sprintf(format, FORMAT_READ, ALGORITHM_LEN);  
  fscanf(stdin, format, alg);
  
  nrows = ncols = idx = 0;
  while(scanf("%c", &c) > 0) {
    //printf("Read #%c#\n", c);
    if(c == '\n') {
      nrows++;
      ncols = idx;
      idx = 0;
      continue;
    }
    grid[nrows][idx] = c == '#' ? 1 : 0;
    idx++;
  }
  
  // Perform the steps
  int steps = STEPS;
  unsigned long long sum = 0;
  grid_steps(steps, alg, grid, &nrows, &ncols);

  sum = grid_count(grid, nrows, ncols);
  // 5225
  printf("Lit pixels after %d steps: %llu\n", steps, sum);


  for(int i=0; i<MAX_ROWS; i++)
    free(grid[i]);
  free(grid);

  
  return 0;
}