#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define BOARD_COLS 5
#define BOARD_ROWS 5

#define MAX_NUM 100

// Stack to hold structs
typedef struct stack {
  void *value;
  struct stack *next, *prev;
} stack_t;

// Helper struct to hold boards
typedef struct board {
  int pos[BOARD_ROWS][BOARD_COLS];
  int num[MAX_NUM];
  int sum;
} board_t;

void stack_append(stack_t **s, void *v) {
  stack_t *t = (stack_t *)malloc(sizeof(*t));
  t->prev = NULL;
  t->next = *s;
  t->value = v;
  if(*s != NULL) (*s)->prev = t;
  *s = t;
  return;
}


void stack_free(stack_t **s) {
  while(*s != NULL) {
    stack_t *t = *s;
    *s = (*s)->next;
    if(t->value != NULL) free(t->value);
    free(t);
  }
}

void nums_add(stack_t **s, int n) {
  int *p = (int *)malloc(sizeof(*p));
  *p = n;
  stack_append(s, (void *)p);
}

void board_add(stack_t **s) {
  board_t *b = (board_t *)malloc(sizeof(*b));
  b->sum = 0;
  for(int i=0; i<BOARD_ROWS; i++) {
    for(int j=0; j<BOARD_COLS; j++)
      b->pos[i][j] = -1;
  }
  for(int i=0; i<MAX_NUM; i++) b->num[i] = -1;
  stack_append(s, (void *)b);
}

void board_mark(board_t *b, int n) {
  // This board doesn't have this number
  if(b->num[n] < 0) return;

  int x = b->num[n];
  int i = x / BOARD_ROWS;
  int j = (x - i*BOARD_ROWS) % BOARD_COLS;
  
  // Already marked
  if(b->pos[i][j] < 0) return;
  
  b->sum -= n;
  b->pos[i][j] = -1;
}

int board_check(board_t *b, int n) {
  int res;
  
  int x = b->num[n] > 0 ? b->num[n] : 0 - b->num[n];
  int row = x / BOARD_ROWS;
  int col = (x - row*BOARD_ROWS) % BOARD_COLS;
  
  // Check vertical bingo
  res = 1;
  for(int i=0; i<BOARD_ROWS; i++) {
    if(b->pos[i][col] >= 0) {
      res = 0;
      break;
    }
  }
  if(res) return res;
  
  // Check horizontal bingo
  res = 1;
  for(int j=0; j<BOARD_COLS; j++) {
    if(b->pos[row][j] >= 0) {
      res = 0;
      break;
    }
  }
  return res;
}

int main() {
  int n;
  char c;
  
  stack_t *nums = NULL;
  stack_t *first = NULL;
  
  stack_t *boards = NULL;
  
  // Read until first non-comma
  while(fscanf(stdin, " %d%c", &n, &c) > 0) {
    nums_add(&nums, n);
    if(first == NULL) first = nums;
    if(c != ',') break;
  }

  int ndigits = 0;
  for(int i = MAX_NUM; i > 0; i/=10) ndigits++;
  
  int nrow = 0;
  int nbytes = 0;
  char buf[BOARD_COLS * (ndigits+1)];
  board_t *cur_board = NULL;
  
  // Read until no more empty lines
  while(fgets(buf, sizeof(buf), stdin) > 0) {
    if(buf[0] == '\n') {
      // Add new board
      nrow = 0;
      board_add(&boards);
      cur_board = (board_t *)(boards->value);
      continue;
    }
    if(nrow >= BOARD_ROWS) continue;
    
    char *s = buf;
    for(int j=0; j<BOARD_COLS; j++) {
      sscanf(s, " %d%n", &n, &nbytes);
      cur_board->pos[nrow][j] = n;
      cur_board->sum += n;
      cur_board->num[n] = (nrow*BOARD_ROWS) + j;
      s += nbytes;
    }
    nrow++;
  }
  
  
  stack_t *t = first;
  int res = 0;
  while(t != NULL) {
    n = *((int*)t->value);
    
    stack_t *b = boards;
    while(b != NULL) {
      cur_board = (board_t *)b->value;
      board_mark(cur_board, n);
      res = board_check(cur_board, n);
      if(res) {
        printf("BINGO at %d! Sum * n=%d\n", n, cur_board->sum *n);
        break;
      }
      b = b->next;
    }
    if(res) break;

    // Reverse order
    t = t->prev;
  }

  stack_free(&nums);
  stack_free(&boards);
  return 0;
}
