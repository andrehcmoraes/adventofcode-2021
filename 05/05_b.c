#include<stdio.h>
#include<stdlib.h>

#define MAX_X 1000
#define MAX_Y 1000
#define MIN_POINTS 2

#define SET_DELTA(a,b,d) \
    if(a == b) d = 0; \
    else if (a < b) d = 1; \
    else d = -1;

#define ADD_DELTA(a,b,d) if( a != b) { a += d; }

typedef struct map {
  int **pos;
  int x, y;
} map_t;

void map_init(map_t **map, int x, int y) {
  map_t *m = (map_t *)malloc(sizeof(*m));
  int **p = (int **)malloc(y * sizeof(*p));
  for(int i=0; i<MAX_Y; i++) {
    p[i] = (int *)malloc(x * sizeof(*(p[i])));
    for(int j=0; j<MAX_X; j++)
      p[i][j] = 0;
  }
  m->pos = p;
  m->x = x;
  m->y = y;
  *map = m;
}

void map_free(map_t **map) {
  map_t *m = *map;
  if(m->pos != NULL) {
    for(int i=0; i < m->y; i++) {
      if(m->pos[i] != NULL) free(m->pos[i]);
    }
    free(m->pos);
  }
  free(m);
  *map = NULL;
}

int main() {
  int x1,x2;
  int y1,y2;
  int dx,dy;
  int n = 0;
  int res = 0;

  map_t *map = NULL;
  map_init(&map, MAX_X, MAX_Y);
  
  while(fscanf(stdin, " %d,%d -> %d,%d", &x1, &y1, &x2, &y2) > 0) {
    // Set directions
    SET_DELTA(x1,x2,dx);
    SET_DELTA(y1,y2,dy);
    
    do {
      // Count only once
      map->pos[x1][y1]++;
      if(map->pos[x1][y1] == MIN_POINTS) n++;
      
      // See if last point was reached
      res = (x1 != x2 || y1 != y2);
      
      // Move if necessary
      ADD_DELTA(x1,x2,dx);
      ADD_DELTA(y1,y2,dy);
    } while( res );
  }
  
  printf("Positions with min points: %d\n", n);

  map_free(&map);
  return 0;
}
