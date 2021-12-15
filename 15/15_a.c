#include<stdio.h>
#include<stdlib.h>


#define MAX_ROWS 100
#define MAX_COLS 100

#define RISK_LIMIT 10

#define QUEUE_PATH_IF_UNSEEN(x,y,s,g,l,p) { \
  if(g[y][x] < l) {\
    queue_enqueue(p, (void *)path_new(x,y,sum + g[y][x])); \
    g[y][x] = l; \
  } \
}

#define uint64_t unsigned long long

#define PARENT(i) (i-1)/2
#define LEFT(i)  (2*i + 1)
#define RIGHT(i) (2*i + 2)

// Priority queue using a min heap
// Function 'cmp(a,b)' must return <0 when a_weight < b_weight 
typedef struct queue {
  unsigned long long len, size;
  int (*cmp)(void*, void*);
  void **values;
} queue_t;

int queue_init(queue_t **q, unsigned long long size, int (*e)(void*,void*)) {
  queue_t *t = (queue_t *)malloc(sizeof(*t));
  if(t == NULL) return -1;
  t->len = 0;
  t->size = size;
  t->values = (void **)malloc(size * sizeof(*(t->values)));
  if(t->values == NULL) return -1;
  t->cmp = e;
  for(unsigned long long i=0; i<size; i++)
    t->values[i] = NULL;
  *q = t;
  return 0;
}

void queue_heapify_up(queue_t *q, unsigned long long i) {
  while(i != 0) {
    unsigned long long p = PARENT(i);
    if(q->cmp(q->values[i], q->values[p]) >= 0) break;
    
    void *a = q->values[p];
    q->values[p] = q->values[i];
    q->values[i] = a;
    i = p;
  }
}

void queue_heapify_down(queue_t *q, unsigned long long i) {
  while(i < q->len) {
    unsigned long long lft, rgt, min;
    lft = LEFT(i);
    rgt = RIGHT(i);
    min = i;
    
    if(lft < q->len) if(q->cmp(q->values[lft], q->values[min]) < 0)
      min = lft;
    if(rgt < q->len) if(q->cmp(q->values[rgt], q->values[min]) < 0)
      min = rgt;
    if(min == i) break;
    
    void *a = q->values[i];
    q->values[i] = q->values[min];
    q->values[min] = a;
    i = min;
  }
}

int queue_enqueue(queue_t *q, void *v) {
  if(q->len == q->size) return -1;
  q->values[q->len] = v;
  queue_heapify_up(q, q->len);
  q->len++;
  return 0;
}

void *queue_pop(queue_t *q) {
  if(!q->len) return NULL;
  void *v = q->values[0];
  q->values[0] = q->values[q->len-1];
  q->values[q->len-1] = NULL;
  q->len--;
  queue_heapify_down(q, 0);
  return v;
}

void queue_free(queue_t **q) {
  queue_t *t = *q;
  if(t == NULL) return;
  for(unsigned long long i=0; i<t->len; i++)
    if(t->values[i] != NULL) free(t->values[i]);
  free(t->values);
  free(t);
  *q = NULL;
}

// Structs to hold path information
typedef struct path {
  uint64_t x, y, sum;
} path_t;

int path_cmp(void *v1, void *v2) {
  unsigned long long s1 = ((path_t *)v1)->sum;
  unsigned long long s2 = ((path_t *)v2)->sum;
  if(s1 < s2) return -1;
  else if(s1 > s2) return 1;
  return 0;
}

path_t *path_new(uint64_t x, uint64_t y, uint64_t sum) {
  path_t *p = (path_t *)malloc(sizeof(*p));
  p->x = x;
  p->y = y;
  p->sum = sum;
  return p;
}

uint64_t find_path(char **grid, uint64_t nrows, uint64_t ncols,
    uint64_t sx, uint64_t sy, uint64_t ex, uint64_t ey) {

  // Underflow for max risk possible
  uint64_t res = 0 - 1;
  
  // Queue start point
  queue_t *paths = NULL;
  if(queue_init(&paths, nrows * ncols, path_cmp)) {
    printf("[!] No memory!\n");
    return res;
  }
  queue_enqueue(paths, (void *)path_new(sx, sy, 0));

  while(paths != NULL) {
    path_t *p = (path_t*) queue_pop(paths);
    uint64_t x = p->x;
    uint64_t y = p->y;
    uint64_t sum = p->sum;
    free(p);
    
    // Reached end point
    if(x == ex && y == ey) {
      res = sum;
      break;
    }

    // Queue neighbours
    if(y>0) QUEUE_PATH_IF_UNSEEN(x, y-1, sum, grid, RISK_LIMIT, paths);
    if(x>0) QUEUE_PATH_IF_UNSEEN(x-1, y, sum, grid, RISK_LIMIT, paths);
    if(y<nrows-1) QUEUE_PATH_IF_UNSEEN(x, y+1, sum, grid, RISK_LIMIT, paths);
    if(x<ncols-1) QUEUE_PATH_IF_UNSEEN(x+1, y, sum, grid, RISK_LIMIT, paths);

  }
  
  queue_free(&paths);
  
  return res;
}

int main() {
  char **grid;
  
  grid = (char **)malloc(MAX_ROWS * sizeof(*grid));
  for(int i=0; i<MAX_ROWS; i++) {
    grid[i] = (char *)malloc(MAX_COLS * sizeof(*(grid[i])));
    for(int j=0; j<MAX_COLS; j++)
      grid[i][j] = 0;
  }
  
  // Read grid
  char c;
  uint64_t nrows, ncols, col;
  nrows = col = ncols = 0;
  while(scanf("%c", &c) > 0) {
    if(c == '\n') {
      nrows++;
      col = 0;
      continue;
    }
    if(c < '0' || c > '9') continue;
    if(!nrows) ncols++;
    grid[nrows][col] = c - '0';
    col++;
  }
  
  uint64_t risk = find_path(grid, nrows, ncols, 0, 0, ncols-1, nrows-1);
  printf("Path with lowest risk: %llu\n", risk);

  for(int i=0; i<MAX_ROWS; i++)
    free(grid[i]);
  free(grid);
  
  return 0;
}