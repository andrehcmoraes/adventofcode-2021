#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_COLS 10
#define MAX_ROWS 10

#define FLASH_LIMIT 9

#define NUM_DUMBOS (MAX_COLS * MAX_ROWS)
#define NUM_STEPS 100

#define PROCESS_NEIGHBOUR(c1, c2, n, g, i, j, s, sl) { \
  if(c1 && c2) { \
    g[i][j]++; \
    if(g[i][j] == n) stack_append(s, sl, i, j); \
  } \
}

// Changed dynamic stack to stack array to reduce mallocs
typedef struct stack {
  int r, c;
} stack_t;

void stack_append(stack_t *s, unsigned long long *s_len, int r, int c) {
  unsigned long long l = *s_len;
  s[l].r = r;
  s[l].c = c;
  *s_len = l + 1;
}

void stack_pop(stack_t *s, unsigned long long *s_len, int *r, int *c) {
  unsigned long long l = (*s_len) - 1;
  *r = s[l].r;
  *c = s[l].c;
  *s_len = l;
}

void grid_update(int **g, int r, int c, stack_t *s, unsigned long long *s_len) {
  for(int i=0; i<r; i++) {
    for(int j=0; j<c; j++) {
      g[i][j]++;
      if(g[i][j] == FLASH_LIMIT + 1) stack_append(s, s_len, i, j);
    }
  }
}

unsigned long long grid_flash(int **g, int r, int c, stack_t *s, 
    unsigned long long *s_len, stack_t *z) {
  int i, j;

  unsigned long long z_len = 0;
  while(*s_len > 0) {
    stack_pop(s, s_len, &i, &j);
    stack_append(z, &z_len, i, j);
    
    int has_left = i>0;
    int has_right = i<c-1;
    int has_up = j>0;
    int has_down = j<r-1;

    int i_l = i-1;
    int i_r = i+1;
    int j_u = j-1;
    int j_d = j+1;

    int limit = FLASH_LIMIT + 1;

    PROCESS_NEIGHBOUR(has_left,  1, limit, g, i_l, j, s, s_len);
    PROCESS_NEIGHBOUR(has_up,    1, limit, g, i, j_u, s, s_len);
    PROCESS_NEIGHBOUR(has_right, 1, limit, g, i_r, j, s, s_len);
    PROCESS_NEIGHBOUR(has_down,  1, limit, g, i, j_d, s, s_len);
    
    PROCESS_NEIGHBOUR(has_left, has_up,    limit, g, i_l, j_u, s, s_len);
    PROCESS_NEIGHBOUR(has_left, has_down,  limit, g, i_l, j_d, s, s_len);
    PROCESS_NEIGHBOUR(has_right, has_up,   limit, g, i_r, j_u, s, s_len);
    PROCESS_NEIGHBOUR(has_right, has_down, limit, g, i_r, j_d, s, s_len);
  }
  
  // Set flashed points to zero
  unsigned long long flashes = 0;
  while(z_len > 0) {
    stack_pop(z, &z_len, &i, &j);
    g[i][j] = 0;
    flashes++;
  }
  
  return flashes;
}

int main() {
  int **grid;
  char *buff;
  
  buff = (char *)malloc((MAX_COLS + 1) * sizeof(*buff));
  grid = (int **)malloc(MAX_ROWS * sizeof(*grid));
  for(int i=0; i<MAX_ROWS; i++)
    grid[i] = (int *)malloc(MAX_COLS * sizeof(*(grid[i])));

  int rows, cols;
  rows = cols = 0;
  while(scanf(" %s", buff) != EOF) {
    if(!cols) cols = strlen(buff);
    for(int j=0; j<cols; j++)
      grid[rows][j] = (int)(buff[j] - '0');
    rows++;
  }
  
  unsigned long long sum = 0;
  unsigned long long f, n, total;

  f = n = 0;
  total = cols * rows;

  stack_t *f_stack = (stack_t *)malloc(total * sizeof(*f_stack));
  stack_t *z_stack = (stack_t *)malloc(total * sizeof(*z_stack));
  unsigned long long f_len = 0;
  while(f != total) {
    // Update energy level step
    grid_update(grid, rows, cols, f_stack, &f_len);

    // Flash step
    f = grid_flash(grid, rows, cols, f_stack, &f_len, z_stack);
    sum += f;
    n++;
  }
  
  printf("First step where all dumbos flash: %llu\n", n);
  
  for(int i=0; i<MAX_ROWS; i++)
    free(grid[i]);
  free(grid);
  free(buff);
  free(f_stack);
  free(z_stack);
  

  return 0;
}