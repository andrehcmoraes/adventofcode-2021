#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_COLS 1000
#define MAX_ROWS 1000

#define FLASH_LIMIT 9
#define NUM_STEPS 100


#define PROCESS_NEIGHBOUR(c1, c2, n, g, i, j, s) { \
  if(c1 && c2) { \
    g[i][j]++; \
    if(g[i][j] == n) stack_append(s, i, j); \
  } \
}

// Using dynamic stack
typedef struct stack {
  int r, c;
  struct stack *next;
} stack_t;

void stack_append(stack_t **s, int r, int c) {
  stack_t *t = (stack_t *)malloc(sizeof(*t));
  t->next = *s;
  t->r = r;
  t->c = c;
  *s = t;
  return;
}

void stack_pop(stack_t **s, int *r, int *c) {
  stack_t *t = *s;
  *s = (*s)->next;
  *r = t->r;
  *c = t->c;
  free(t);
}

void stack_free(stack_t **s) {
  while(*s != NULL) {
    stack_t *t = *s;
    *s = (*s)->next;
    free(t);
  }
}

void grid_update(int **g, int r, int c, stack_t **s) {
  for(int i=0; i<r; i++) {
    for(int j=0; j<c; j++) {
      g[i][j]++;
      if(g[i][j] == FLASH_LIMIT + 1) stack_append(s, i, j);
    }
  }
}

unsigned long long grid_flash(int **g, int r, int c, stack_t **s) {
  int i, j;

  // Stack with points to set energy level to zero after flash step
  stack_t *zeros = NULL;
  while(*s != NULL) {
    stack_pop(s, &i, &j);
    stack_append(&zeros, i, j);
    
    int has_left = i>0;
    int has_right = i<c-1;
    int has_up = j>0;
    int has_down = j<r-1;

    int i_l = i-1;
    int i_r = i+1;
    int j_u = j-1;
    int j_d = j+1;

    int limit = FLASH_LIMIT + 1;

    PROCESS_NEIGHBOUR(has_left,  1, limit, g, i_l, j, s);
    PROCESS_NEIGHBOUR(has_up,    1, limit, g, i, j_u, s);
    PROCESS_NEIGHBOUR(has_right, 1, limit, g, i_r, j, s);
    PROCESS_NEIGHBOUR(has_down,  1, limit, g, i, j_d, s);
    
    PROCESS_NEIGHBOUR(has_left, has_up,    limit, g, i_l, j_u, s);
    PROCESS_NEIGHBOUR(has_left, has_down,  limit, g, i_l, j_d, s);
    PROCESS_NEIGHBOUR(has_right, has_up,   limit, g, i_r, j_u, s);
    PROCESS_NEIGHBOUR(has_right, has_down, limit, g, i_r, j_d, s);
  }
  
  // Set flashed points to zero
  unsigned long long flashes = 0;
  while(zeros != NULL) {
    stack_pop(&zeros, &i, &j);
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
  
  // Stack to save all points that will flash
  stack_t *f_stack = NULL;
  unsigned long long sum = 0;
  for(int n=0; n<NUM_STEPS; n++) {


    // Update energy level step
    grid_update(grid, rows, cols, &f_stack);

    // Flash step
    sum += grid_flash(grid, rows, cols, &f_stack);

  }

    

  printf("Number of flashes after %d steps: %llu\n", NUM_STEPS, sum);
  
  stack_free(&f_stack);
  for(int i=0; i<MAX_ROWS; i++)
    free(grid[i]);
  free(grid);
  free(buff);
  

  return 0;
}