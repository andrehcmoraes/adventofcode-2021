#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// 'start' string has len=5
#define MAX_NODE_LEN 5
#define MAX_LINE_LEN ((2*MAX_NODE_LEN) + 1)

#define START_NODE "start"
#define END_NODE "end"

#define MAX_NODES 50

#define MAX_SMALL 1

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

void *stack_pop(stack_t **h) {
  stack_t *s = *h;
  if(s == NULL) return NULL;
  void *v = s->val;
  (*h) = (*h)->next;
  if(*h != NULL) (*h)->prev = NULL;
  free(s);
  return v;
}

// Structs to hold node information
typedef struct node {
  int index, small;
} node_t;

typedef struct node_dict {
  node_t node;
  char name[MAX_NODE_LEN+1];
} node_dict_t;


void node_append(stack_t **t, int index, int small) {
  node_t *v = (node_t *)malloc(sizeof(*v));
  
  v->index = index;
  v->small = small;
  
  stack_append(t, (void *)v);
}

// Dictionary struct with stack to handle collision
typedef struct dictionary {
  stack_t **vals;
  unsigned int size, len;
} dict_t;

// Scuffed hash - dbj2
unsigned int dict_hash(char *str, int len) {
  unsigned long hash = 5381;
    for(int i=0; i<len; i++)
      hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */
  return hash;
}

void dict_init(dict_t **d, unsigned int size) {
  dict_t *t;
  t = (dict_t *)malloc(sizeof(*t));
  t->size = size;
  t->len = 0;
  t->vals = (stack_t **)malloc(size * sizeof(*(t->vals)));
  for(int i=0; i<size; i++)
    t->vals[i] = NULL;
  *d = t;
}

void dict_add(dict_t *d, unsigned int index, char *str, int len) {
  unsigned int h = dict_hash(str, len);
  h = h % d->size;
  
  int small = 1;
  for(int x=0; x<len; x++) {
    if(str[x] < 'a' || str[x] > 'z') {
      small= 0;
      break;
    }
  }

  node_dict_t *n = (node_dict_t *)malloc(sizeof(*n));
  strncpy(n->name, str, len);
  n->node.small = small;
  n->node.index = index;
  stack_append(&(d->vals[h]), (void *)n);
  
  d->len++;
}

node_t *dict_get(dict_t *d, char *str, int len) {
  unsigned int h = dict_hash(str, len);
  h = h % d->size;

  stack_t *t = d->vals[h];
  while(t != NULL) {
    node_dict_t *v = (node_dict_t *) t->val;
    if(strncmp(v->name, str, len) == 0) return &(v->node);
    t = t->next;
  }
  return NULL;
}

void dict_free(dict_t **d, void (*f)(void*)) {
  dict_t *t = *d;
  for(int i=0; i<t->size; i++)
    stack_free(&(t->vals[i]), f);
  free(t->vals);
  free(t);
  *d = NULL;
}

// Struct to hold path
typedef struct path {
  int current;
  int *visited;
} path_t;

void path_append(stack_t **t, int c, int *v, int n) {
  path_t *p = (path_t *)malloc(sizeof(*p));
  p->current = c;

  p->visited = (int *) malloc(n * sizeof(*(p->visited)));
  for(int j=0; j<n; j++)
    p->visited[j] = v[j]; 
  p->visited[c]++;
  
  stack_append(t, (void *)p);
}

void path_pop(stack_t **t, int *c, int *v, int n) {
  path_t *p = (path_t *)stack_pop(t);
  
  *c = p->current;
  for(int j=0; j<n; j++)
    v[j] = p->visited[j]; 

  free(p->visited);
  free(p);
}

// While a explicit DFS stack can recover the paths just the same, it is harder
// to keep track of the path - must queue entire path so far for each node
unsigned long long find_paths(stack_t **adj, int num_nodes, int nstart, int nend) {
  unsigned long long sum;
  int visited[num_nodes];
  int current;

  for(int j=0;j<num_nodes;j++)
    visited[j] = 0;

  // Stack start
  stack_t *paths = NULL;
  path_append(&paths, nstart, visited, num_nodes);

  sum = 0;
  while(paths != NULL) {
    
    // Pop stack
    path_pop(&paths, &current, visited, num_nodes);
    
    // Reached the end
    if(current == nend) {
      sum++;
      continue;
    }

    stack_t *t = adj[current];
    while(t!=NULL) {
      node_t *v = (node_t *)t->val;
      t = t->next;
      
      // Don't requeue start
      if(v->index == nstart) continue;
      
      // Check visited if target node is small and is not endnode
      if(v->index != nend && v->small && visited[v->index] >= MAX_SMALL)
        continue;
      
      // Queue next node
      path_append(&paths, v->index, visited, num_nodes);
    }
  }
  // Just to be sure
  stack_free(&paths, NULL);
  
  return sum;
}

int main() {
  stack_t **adj;
  dict_t *dict = NULL;
  
  char delim[] = "-";
  char *buff;
  char *node_a, *node_b;

  buff = (char *)malloc((MAX_LINE_LEN + 1) * sizeof(*buff));
  node_a = (char *)malloc((MAX_NODE_LEN +1) * sizeof(*node_a));
  node_b = (char *)malloc((MAX_NODE_LEN +1) * sizeof(*node_b));
  
  adj = (stack_t **)malloc(MAX_NODES * sizeof(*adj));
  for(int i=0; i<MAX_NODES; i++)
    adj[i] = NULL;
  
  dict_init(&dict, 2*MAX_NODES);
  
  int n = 0;
  while(scanf(" %s", buff) != EOF) {
    // Parse the node names
    int len = strlen(buff);
    int len_a = strcspn(buff, delim);
    int len_b = len-len_a-1;
    strncpy(node_a, buff, len_a);
    strncpy(node_b, buff+len_a+1, len_b);
    node_a[len_a] = '\0';
    node_b[len_b] = '\0';
    
    // Get the indexes for these nodes
    node_t *na = dict_get(dict, node_a, len_a);
    node_t *nb = dict_get(dict, node_b, len_b);
    
    // Assign an index to any node that doesn't have one yet
    if(na == NULL) { 
      dict_add(dict, n, node_a, len_a);
      na = dict_get(dict, node_a, len_a);
      n++;
    }
    if(nb == NULL) {
      dict_add(dict, n, node_b, len_b);
      nb = dict_get(dict, node_b, len_b);
      n++;
    }
    
    // Append adjacencies to both nodes
    node_append(&(adj[na->index]), nb->index, nb->small);
    node_append(&(adj[nb->index]), na->index, na->small);
    
  }
  
  // Get start and end indexes - ignore small field
  node_t *nstart = dict_get(dict, START_NODE, strlen(START_NODE));
  node_t *nend = dict_get(dict, END_NODE, strlen(END_NODE));

  // Make paths
  unsigned long long sum = find_paths(adj, n, nstart->index, nend->index);
  printf("Total paths found:%llu\n", sum);
  
  free(buff);
  free(node_a);
  free(node_b);
  for(int i=0; i<MAX_NODES; i++)
    stack_free(&(adj[i]), NULL);
  free(adj);
  dict_free(&dict, NULL);

  return 0;
}