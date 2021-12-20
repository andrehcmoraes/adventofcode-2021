#include<stdio.h>
#include<stdlib.h>

// VERY SLOW SOLUTION - ~15s
// Really should use a hashmap!

// Scuffed - replace with dynamic allocation
#define MAX_SCANNERS 40
#define MAX_BEACONS_PER_SCANNER 30
#define MAX_BEACONS MAX_SCANNERS*MAX_BEACONS_PER_SCANNER

#define ROTATIONS_NUM 24
#define MIN_OVERLAPPING 12

typedef struct point {
  int x, y, z;
} point_t;

typedef struct scanner {
  int m;
  point_t points[MAX_BEACONS];
} scanner_t;

scanner_t scan[MAX_SCANNERS];

int point_equals(point_t *a, point_t *b) {
  return (a->x == b->x) && (a->y == b->y) && (a->z == b->z);
}


void point_copy(point_t *a, point_t *b) {
  a->x = b->x;
  a->y = b->y;
  a->z = b->z;
}

void point_subtract(point_t *a, point_t *b) {
  a->x -= b->x;
  a->y -= b->y;
  a->z -= b->z;
}

// (x,y,z) -> (x,z,-y)
point_t *point_roll(point_t *p) {
  int t = p->y;
  p->y = p->z;
  p->z = -t;
  return p;
}

// (x,y,z) -> (-y,x,z)
point_t *point_turn(point_t *p) {
  int t = p->x;
  p->x = 0 - p->y;
  p->y = t;
  return p;
}

// Put all 24 rotations of point 'p' in aray 'r'
void point_rotations(point_t *p, point_t r[]) {
  int i=0;
  for(int c=0; c<2; c++) {
    for(int s=0; s<3; s++) {
      point_roll(p);
      point_copy(&(r[i]), p);
      i++;
      for(int l=0; l< 3; l++) {
        point_turn(p);
        point_copy(&(r[i]), p);
        i++;
      }
    }
    point_roll(point_turn(point_roll(p)));
  }

}

// Merge all points from Scanner 'a' to Scanner 'b'
// by rotating all points from 'a' by 'i' and adding delta 'd'
int scanner_merge(point_t *d, int i, int a, int b, int ma, int mb) {
  point_t r[ROTATIONS_NUM];
  point_t p;
  int sum = 0;
  int found = 0;

  for(int ai=0; ai<ma; ai++) {
    point_copy(&p, &(scan[a].points[ai]));
    point_rotations(&p, r);
    point_copy(&p, &(r[i]));
    point_t delta = {.x=d->x + p.x, .y=d->y + p.y, .z=d->z + p.z};
    
    // Check if point is already in 'b'
    // Slow - improve with hash set
    found = 0;
    for(int bi=0; bi<mb; bi++) {
      if(!point_equals(&delta, &(scan[b].points[bi]))) continue;
      
      sum++;
      found = 1;
      break;
    }
    // Skip points that are already in 'b'
    if(found) continue;

    // Add new point to 'b'
    point_copy(&(scan[b].points[mb]), &delta);
    mb++;
  }
  
  // New number of points
  scan[b].m = mb;
  return sum;
}

// Rotate all points in scanner 'a' and see if there exists a candidate delta
// Such that for each point 'p' in both 'a' and 'b', p_{a} - p{b} = delta
int scanner_rotate(point_t *d, int i, int a, int b, int ma, int mb) {
  int n = 0;
  int idx;
  int cand_count[ma * mb];
  point_t cand_points[ma * mb];
  point_t r[ROTATIONS_NUM];
  point_t p;

  for(int ai=0; ai<ma; ai++) {
    // Get the ith rotation of point 'ai'
    point_copy(&p, &(scan[a].points[ai]));
    point_rotations(&p, r);
    point_copy(&p, &(r[i]));
    for(int bi=0; bi<mb; bi++) {
      point_t delta = {.x=scan[b].points[bi].x - p.x, scan[b].points[bi].y - p.y, scan[b].points[bi].z - p.z};
      
      // Check if result is already a candidate
      // Slow - improve with hash set
      for(idx=0; idx<n; idx++)
        if(point_equals(&(cand_points[idx]), &delta)) break;
      
      // New candidate
      if(idx == n) { 
        point_copy(&(cand_points[idx]), &delta);
        cand_count[idx] = 0;
        n++;
      }
      
      // Check if candidate meets the criteria
      cand_count[idx]++;
      if(cand_count[idx] >= MIN_OVERLAPPING) {
        point_copy(d, &delta);
        return 1;
      }
    }
  }

  return 0;
}

// Try to match scanner 'a' with scanner 'b'
int scanner_match(int a, int b, int ma, int mb) {
  point_t d;
  int matched = 0;

  for(int i=0; i<ROTATIONS_NUM; i++) {
    matched = scanner_rotate(&d, i, a, b, ma, mb);
    if(matched) {
      // Scanner 'a' matched Scanner 'b'
      scanner_merge(&d, i, a, b, ma, mb);
      return 1;
    }
  }
  return 0;
}

int main() {
  char c;
  int n, m;
  int x,y,z;
  
  // Read scanners
  while(scanf("%c", &c) > 0)
    if(c == '-') break;
  n = 0;
  while(scanf("-- scanner %d ---", &x) > 0) {
    m = 0;
    while(scanf(" %d,%d,%d\n", &x, &y, &z) > 0) {
      scan[n].points[m].x = x;
      scan[n].points[m].y = y;
      scan[n].points[m].z = z;
      m++;
    }
    scan[n].m = m;
    n++;
  }

  int matched = 0;
  // For each scanner - try to match it with another scanner
  // When match happens - transfer all points from one scanner to another
  for(int a=n; a>=0; a--) {
    for(int b=a-1; b>=0; b--) {
      matched = scanner_match(a, b, scan[a].m, scan[b].m);
      if(matched) break;
    }
  }

  // Scan 0 has all points
  printf("Total beacons: %d\n", scan[0].m);
  
  return 0;
}