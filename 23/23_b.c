#include<stdio.h>
#include<stdlib.h>

// Node orientation
//#########################
//# 0-1---2---3---4---5-6 #
//###  \ / \ / \ / \ /  ###
//  #   7 # B # F # J   #
//  #   | # | # | # |   #
//  #   8 # C # G # K   #
//  #   | # | # | # |   #
//  #   9 # D # H # L   #
//  #   | # | # | # |   #
//  #   A # E # I # M   #
//  #####################

// Very naive heuristic - Improve for correctness and better queue bound
#define BAD_SOLUTION(a,b) (a < b - 2)
#define MAX_QUEUE 10000000

#define ROOMS_NUM 4
#define ROOMS_SIZE 4
#define HALLWAY_LEN (ROOMS_NUM + 3)
#define IS_HALLWAY(x) (x<HALLWAY_LEN)
#define NUM_AMPHIPODS (ROOMS_SIZE*ROOMS_NUM)
#define NODES_SIZE (HALLWAY_LEN + NUM_AMPHIPODS)

#define AMPHIPOD(c) ((c-'A')+1)

#define VALID_ROOM_MIN(x) (HALLWAY_LEN + ((x-1)*ROOMS_SIZE))
#define VALID_ROOM_MAX(x) (VALID_ROOM_MIN(x)+ROOMS_SIZE-1)

int costs[] = {0, 1, 10, 100, 1000};

// Priority queue using a min heap
// Function 'cmp(a,b)' must return <0 when a_weight < b_weight
#define PARENT(i) (i-1)/2
#define LEFT(i)  (2*i + 1)
#define RIGHT(i) (2*i + 2)
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

void queue_clear(queue_t *q, void (*f)(void*)) {
  if(q == NULL) return;
  if(f == NULL) f = free;
  for(unsigned long long i=0; i<q->len; i++)
    if(q->values[i] != NULL) f(q->values[i]);
  q->len = 0;
}

void queue_free(queue_t **q,  void (*f)(void*)) {
  if(*q == NULL) return;
  queue_clear(*q, f);
  free((*q)->values);
  free(*q);
  *q = NULL;
}

// Struct for partial solutions
typedef struct pathstate{
  int pos[NODES_SIZE];
  int nclr;
  unsigned long long sum;
} path_t;

// Compare two paths
int path_cmp(void *v1, void *v2) {
  unsigned long long s1 = ((path_t *)v1)->sum;
  unsigned long long s2 = ((path_t *)v2)->sum;
  if(s1 < s2) return -1;
  else if(s1 > s2) return 1;
  return 0;
}

// Queue a savestate
void path_save(queue_t *h, int pos[], int nclr, unsigned long long sum) {
  path_t *p = (path_t *)malloc(sizeof(*p));
  p->sum = sum;
  p->nclr = nclr;
  for(int i=0; i<NODES_SIZE; i++)
    p->pos[i] = pos[i];
  
  if(queue_enqueue(h, (void *)p) != 0) {
    printf("[!] Queue exceeded!\n");
    queue_free(&h, NULL);
    exit(-1);
  }
  
}

// Load a savestate from the queue
void path_load(queue_t *h, int pos[], int *nclr, unsigned long long *sum) {
  path_t *p = (path_t *)queue_pop(h);
  *sum = p->sum;
  *nclr = p->nclr;
  for(int i=0; i<NODES_SIZE; i++)
    pos[i] = p->pos[i];
  free(p);
}

// 'i' is always a hallway tile
// 'j' is always a room tile
unsigned long long graph_hall2room(int i, int j, int rmin, int p, int pos[]) {
  
  // Find the entrances to the room that contains 'j'
  int elft = ((j-HALLWAY_LEN)/ROOMS_SIZE) + 1;
  int ergt = elft + 1;

  unsigned long long cost = 0;
  int guys = 0;

  // Adjust extra tile left
  if(i == 0) {
    if(pos[i]) guys++;
    cost++;
    i++;
  }

  // Adjust extra tile right
  if(i == HALLWAY_LEN-1) {
    if(pos[i]) guys++;
    cost++;
    i--;
  }

  // Walk hallway to nearest entrance
  if(i <= elft) {
    for(; i<elft; i++) {
      if(pos[i]) guys++;
      cost+=2;
    }
    if(pos[elft]) guys++;
  } else {
    for(; i>ergt; i--) {
      if(pos[i]) guys++;
      cost+=2;
    }
    if(pos[ergt]) guys++;
  }

  // Enter room
  cost+=2;

  // Go further into the room to 'j'
  for(i=rmin; i<j; i++) {
    if(pos[i]) guys++;
    cost++;
  }
  if(pos[j]) guys++;
  
  cost = cost * costs[p];

  // Only one person was supposed to be in this path
  if(guys > 1) return 0;
  
  // Return total cost
  return cost;
  
}

unsigned long long graph_solve(int pos[]) {
  unsigned long long sum = 0;
  queue_t *paths = NULL;
  
  // Max possible solutions = TILES * AMPHIODS
  if(queue_init(&paths, MAX_QUEUE, path_cmp)) {
    printf("[!] No memory!\n");
    return sum;
  }
  
  // Initial ampipods in correct positions in valid rooms
  int nclr = 0;
  int val = 1;
  for(int v=HALLWAY_LEN; v<NODES_SIZE; v+=ROOMS_SIZE) {
    for(int i=ROOMS_SIZE-1; i>=0; i--) {
      if(pos[v+i] == val) nclr++;
      else break;
    }
    val++;
  }
  int maxclr = nclr;
  // Save initial path
  path_save(paths, pos, nclr, sum);
  
  // While there are still paths in queue
  while(paths->len > 0) {
    // Load path with lowest sum
    path_load(paths, pos, &nclr, &sum);

    // Solution is completed
    if(nclr == NUM_AMPHIPODS) break;

    // Skip if solution is bad
    if(BAD_SOLUTION(nclr,maxclr)) continue;

    // Update highest clear
    maxclr = nclr > maxclr ? nclr : maxclr;

    if(paths->len > MAX_QUEUE - NODES_SIZE) { 
      printf("[!] Max queue exceeded at %llu\n", paths->len);
      break;
    }
    
    // For each tile
    for(int i=0; i<NODES_SIZE; i++) {
      
      // No one here, skip
      if(!pos[i]) continue;
      

      // Check guy status
      int p = pos[i];
      int is_hall = IS_HALLWAY(i);
      int rmin = VALID_ROOM_MIN(p);
      int rmax = VALID_ROOM_MAX(p);
      int own_room = (rmin <= i && i <= rmax);

      // Check room status
      // If room is only holding correct residents or empty spaces
      int room_valid = 1;

      // See if room has only correct residents with no remaining spaces
      int room_complete = 1;
      
      for(int j=rmin; j<=rmax; j++) {
        if(pos[j] != p) {
          room_complete = 0;
          if(pos[j]) room_valid = 0;
        }
        
      }
      
      // Skip this guy if he's at his room with his buddies
      if(room_complete) continue;

      // His room is valid and this guy is inside
      if(room_valid && own_room) continue;

      // If guy is at hallway
      if(is_hall) {
        // If his room contains invalid residents, skip
        if(!room_valid) continue;
        
        // If his room is ok, try to move him to his room as further as possible
        for(int j=rmax; j>=rmin; j--) {
          // Someone is there, skip
          if(pos[j]) continue;
          
          unsigned long long cost = graph_hall2room(i, j, rmin, p, pos);
          if(!cost) continue;
          
          // Queue new path
          pos[j] = p;
          pos[i] = 0;
          path_save(paths, pos, nclr+1, sum+cost);
          pos[j] = 0;
          pos[i] = p;
          break;
        }
        continue;
      }
      
      // If guy is not at hallway, move to every hallway tile
      for(int j=0; j<HALLWAY_LEN; j++) {
        // Someone is there, skip
        if(pos[j]) continue;
        
        // Exit for the current room
        int rext = (((i-HALLWAY_LEN)/ROOMS_SIZE)*ROOMS_SIZE)+HALLWAY_LEN;
        
        unsigned long long cost = graph_hall2room(j, i, rext, p, pos);
        if(!cost) continue;
        
        // Queue new path
        pos[j] = p;
        pos[i] = 0;
        path_save(paths, pos, nclr, sum+cost);
        pos[j] = 0;
        pos[i] = p;
      }
      
    }
    
  }
  
  if(nclr < NUM_AMPHIPODS) {
    printf("[!] Couldn't find solution! Best clears: %d\n", maxclr);
  }
  
  queue_free(&paths, NULL);
  
  return sum;
}

int main() {
  int pos[NODES_SIZE] = {0};

  // Parse input
  int p = 0;
  int line = 0;
  char c;
  while(scanf("%c", &c) > 0) {
    if(c == '\n') {
      line++;
      if(line == 2) p = HALLWAY_LEN;
      if(line == 3) p = HALLWAY_LEN + ROOMS_SIZE - 1;
      continue;
    }
    if(line < 2) continue;
    if(c < 'A' || c > 'Z') continue;
    
    pos[p] = AMPHIPOD(c);
    p += ROOMS_SIZE;
  }
  // Insert additional input
  pos[ 8] = AMPHIPOD('D');
  pos[ 9] = AMPHIPOD('D');
  pos[12] = AMPHIPOD('C');
  pos[13] = AMPHIPOD('B');
  pos[16] = AMPHIPOD('B');
  pos[17] = AMPHIPOD('A');
  pos[20] = AMPHIPOD('A');
  pos[21] = AMPHIPOD('C');
  
  // 48304
  unsigned long long sum = graph_solve(pos);
  printf("Least energy required: %llu\n", sum);

  
  return 0;
}