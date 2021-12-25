#include<stdio.h>
#include<stdlib.h>

// Double linked stack
typedef struct stack {
  void *val;
  struct stack *next, *prev;
} stack_t;

void stack_append(stack_t **s, void *v) {
  stack_t *t = (stack_t *)malloc(sizeof(*t));
  t->next = *s;
  t->prev = NULL;
  t->val = v;
  if(*s != NULL) {
    if((*s)->prev != NULL) {
      (*s)->prev->next = t;
      t->prev = (*s)->prev;
    }
    (*s)->prev = t;
  }
  *s = t;
}

void stack_free(stack_t **s, void (*f)(void*)) {
  if(f == NULL) f = free;
  while(*s != NULL) {
    stack_t *t = *s;
    *s = (*s)->next;
    if(t->val != NULL) f(t->val);
    free(t);
  }
}

typedef struct point {
  int x,y;
} point_t;

void points_append(stack_t **s, int x, int y) {
  point_t *p = (point_t *)malloc(sizeof(*p));
  p->x = x;
  p->y = y;
  stack_append(s, (void *)p);
}

unsigned long long grid_fold(short **grid, int *lx, int *ly, int n, int is_ver) {
  unsigned long long visible = 0;
  int len_x = *lx;
  int len_y = *ly;
  int nx, ny, dx, dy, z;
  dx = dy = 0;
  nx = ny = 0;

  if(is_ver) {
    // Fold vertically
    ny = 1+2*n;
    len_y = n;
    dy = 1;
  } else {
    // Fold horizontally
    nx = 1+2*n;
    len_x = n;
    dx = 1;
  }

  // Fold
  z = dx ? nx : 0;
  for(int y=0; y<len_y; y++) {
    ny = dy ? ny - 1 : y;
    for(int x=0; x<len_x; x++) {
      nx = dx ? nx - 1 : x;
      if(nx >= *lx || ny >= *ly) continue; 
      if(grid[ny][nx]) grid[y][x] = grid[ny][nx];
    }
    nx = dx ? z : 0;
  }
  
  
  // Count visible
  for(int y=0; y<len_y; y++) {
    for(int x=0; x<len_x; x++) {
        if(grid[y][x]) visible++;
    }
  }
  
  *lx = len_x;
  *ly = len_y;
  return visible;
}

int main() {
  // Stack points
  stack_t *points = NULL;
  int x,y;
  int len_x, len_y;
  len_x = len_y = 0;
  while(scanf(" %d,%d", &x, &y) > 0) {
    len_x = len_x < x ? x : len_x;
    len_y = len_y < y ? y : len_y;
    points_append(&points, x, y);
  }
  len_y++;
  len_x++;

  // Build grid
  short **grid;
  grid = (short **)malloc(len_y * sizeof(*grid));
  for(int i=0; i<len_y; i++) {
    grid[i] = (short *)malloc(len_x * sizeof(*(grid[i])));
    for(int j=0; j<len_x; j++)
      grid[i][j] = 0;
  }


  // Apply points
  stack_t *t = points;
  while(t != NULL) {
    point_t *p = (point_t *)(t->val);
    t = t->next;
    grid[p->y][p->x] = 1;
  }
  stack_free(&points, NULL);
  
  // Do folds
  int n;
  char c;
  int lx, ly;

  lx = len_x;
  ly = len_y;
  unsigned long long res;
  while(scanf(" fold along %c=%d", &c, &n) > 0) {
    if(c != 'x' && c !='y') {
      printf("[!] Invalid fold %c\n", c);
      exit(-1);
    }
    res = grid_fold(grid, &lx, &ly, n, c == 'y');
    
    // First fold only
    break;
  }
  // 755
  printf("Visible points:%llu\n", res);

  for(int i=0; i<len_y; i++)
    free(grid[i]);
  free(grid);
  
  return 0;
}