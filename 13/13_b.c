#include<stdio.h>
#include<stdlib.h>

// Generic stack
typedef struct stack {
  void *val;
  struct stack *next, *prev;
} stack_t;

void stack_append(stack_t **s, void *v) {
  stack_t *t = (stack_t *)malloc(sizeof(*t));
  t->next = *s;
  t->prev = NULL;
  t->val = v;
  if(*s != NULL) (*s)->prev = t;
  *s = t;
}

void stack_pop(stack_t **h, stack_t *s) {
  if(s->next != NULL) s->next->prev = s->prev;
  if(s->prev != NULL) s->prev->next = s->next;
  if(s->val  != NULL) free(s->val);
  if(*h == s) *h = s->next;
  free(s);
}

void stack_free(stack_t **s) {
  while(*s != NULL) {
    stack_t *t = *s;
    *s = (*s)->next;
    if(t->val != NULL) free(t->val);
    free(t);
  }
}

typedef struct point {
  unsigned long long x,y;
} point_t;

void points_append(stack_t **s, unsigned long long x, unsigned long long y) {
  point_t *p = (point_t *)malloc(sizeof(*p));
  p->x = x;
  p->y = y;
  stack_append(s, (void *)p);
}

point_t *points_find(stack_t *s, unsigned long long x, unsigned long long y) {
  while(s != NULL) {
    point_t *p = (point_t *) s->val;
    s = s->next;
    if(p->x == x && p->y == y) return p;
  }
  return NULL;
}

void points_fold(stack_t **points, unsigned long long *len_x, 
    unsigned long long *len_y, unsigned long long n, int is_ver) {
  
  // For each point, see if they lie beyond the soon-to-be excluded zone
  stack_t *new_p = NULL;
  stack_t *t = *points;
  while(t != NULL) {
    int del = 0;
    point_t *p0 = (point_t *)(t->val);
    // Vertical fold
    if(is_ver && p0->y > n) {
      // See if point p1 already exists, add it if it doesn't
      point_t *p1 = points_find(*points, p0->x, 2*n - p0->y);
      if(p1 == NULL) points_append(&new_p, p0->x, 2*n - p0->y);
      // Set to remove this point
      del = 1;
    }
    // Horizontal fold
    if(!is_ver && p0->x > n) {
      point_t *p1 = points_find(*points, 2*n - p0->x, p0->y);
      if(p1 == NULL) points_append(&new_p, 2*n - p0->x, p0->y);
      del = 1;
    }
    stack_t *x = t->next;
    // Delete current point if it was set to delete
    if(del) stack_pop(points, t);
    t = x;
  }

  t = new_p;
  while(t != NULL) {
    stack_t *x = t;
    t = t->next;
    stack_append(points, x->val);
    free(x);
  }

  // Update new lengths
  if(is_ver) *len_y = n;
  else *len_x = n;
}

int main() {
  // Stack points
  stack_t *points = NULL;
  unsigned long long x,y;
  unsigned long long len_x, len_y;
  len_x = len_y = 0;
  while(scanf(" %llu,%llu", &x, &y) > 0) {
    len_x = len_x < x ? x : len_x;
    len_y = len_y < y ? y : len_y;
    points_append(&points, x, y);
  }
  // Actual lengths are not the same as largest point
  len_y++;
  len_x++;
  
  // Do folds
  unsigned long long n;
  char c;
  while(scanf(" fold along %c=%llu", &c, &n) > 0) {
    if(c != 'x' && c !='y') {
      printf("[!] Invalid fold %c\n", c);
      exit(-1);
    }
    points_fold(&points, &len_x, &len_y, n, c == 'y');
  }

  // Build grid
  short **grid;
  grid = (short **)malloc(len_y * sizeof(*grid));
  for(unsigned long long i=0; i<len_y; i++) {
    grid[i] = (short *)malloc(len_x * sizeof(*(grid[i])));
    for(unsigned long long j=0; j<len_x; j++)
      grid[i][j] = 0;
  }
  
  // Apply points
  stack_t *t = points;
  while(t != NULL) {
    point_t *p = (point_t *)(t->val);
    t = t->next;
    grid[p->y][p->x] = 1;
  }
  stack_free(&points);
  
  for(unsigned long long y=0; y<len_y; y++) {
    for(unsigned long long x=0; x<len_x; x++) {
      printf("%c", grid[y][x] ? '#' : '.');
    }
    printf("\n");
  }
  
  for(unsigned long long i=0; i<len_y; i++)
    free(grid[i]);
  free(grid);
  
  return 0;
}