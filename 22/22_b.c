#include<stdio.h>
#include<stdlib.h>

#ifdef USE_BIG_ENUM
  // Requires OPENSSL - install libssl-dev
  // Compile with
  // make day22_bn
  #include <openssl/bn.h>
  #define ADD_BIGNUM(a,b) BN_add(a,a,b);
  #define MUL_BIGNUM(a,b) BN_mul(a,a,b,ctx);
  #define SET_BIGNUM(a,b) BN_set_word(a,b);
  #define INI_BIGNUM(a) { a =  BN_new(); BN_zero(a); }
  #define FRE_BIGNUM(a) BN_free(a);
  #define BIGNUMBA BIGNUM*

  BN_CTX *ctx;
#else  
  #define ADD_BIGNUM(a,b) a = a+b;
  #define INI_BIGNUM(a) a = ((unsigned long long) 0);
  #define MUL_BIGNUM(a,b) a = a*b;
  #define SET_BIGNUM(a,b) a=b;
  #define FRE_BIGNUM(a) a=0;
  #define BIGNUMBA unsigned long long
#endif

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

void *stack_pop(stack_t **h) {
  stack_t *s = *h;
  if(s == NULL) return NULL;
  void *v = s->val;
  (*h) = (*h)->next;
  if(*h != NULL) (*h)->prev = NULL;
  free(s);
  return v;
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

void stack_del(stack_t **h, stack_t *s) {
  if(s->next != NULL) s->next->prev = s->prev;
  if(s->prev != NULL) s->prev->next = s->next;
  if(s->val  != NULL) free(s->val);
  if(*h == s) *h = s->next;
  free(s);
}

typedef struct cuboid {
  int x1,x2,y1,y2,z1,z2;
} cuboid_t;

// Cuboid volume - uga buga version
BIGNUMBA cuboid_vol(cuboid_t *c) {
  BIGNUMBA a;
  BIGNUMBA b;
  INI_BIGNUM(a);
  INI_BIGNUM(b);
  
  SET_BIGNUM(a, (c->x2 - c->x1 + 1));
  SET_BIGNUM(b, (c->y2 - c->y1 + 1));
  MUL_BIGNUM(a,b);
  SET_BIGNUM(b, (c->z2 - c->z1 + 1));
  MUL_BIGNUM(a,b);
  FRE_BIGNUM(b);
  return a;
}

void cuboid_copy(cuboid_t *a, cuboid_t *b) {
  a->x1=b->x1; a->x2=b->x2;
  a->y1=b->y1; a->y2=b->y2;
  a->z1=b->z1; a->z2=b->z2;
}

// Add new cuboid to stack
void cuboid_add(stack_t **regions, cuboid_t *cube) {
  cuboid_t *c = (cuboid_t *)malloc(sizeof(*c));
  cuboid_copy(c, cube);
  stack_append(regions, (void *)c);
}

// Check if cuboid intersects another cuboid
int cuboid_intersect(cuboid_t *a, cuboid_t *b) {
  int hit = 0;
  hit += a->x1 > b->x2;
  hit += a->x2 < b->x1;
  hit += a->y1 > b->y2;
  hit += a->y2 < b->y1;
  hit += a->z1 > b->z2;
  hit += a->z2 < b->z1;
  return !hit;
}

void cuboid_split(stack_t **newr, cuboid_t *a, cuboid_t *b) {
  cuboid_t c;
  if(a->x1 < b->x1) {
    cuboid_copy(&c, a);
    c.x2 = b->x1-1;
    cuboid_add(newr, &c);
    a->x1 = b->x1;
  }
  if(a->x2 > b->x2) {
    cuboid_copy(&c, a);
    c.x1 = b->x2+1;
    cuboid_add(newr, &c);
    a->x2 = b->x2;
  }

  if(a->y1 < b->y1) {
    cuboid_copy(&c, a);
    c.y2 = b->y1-1;
    cuboid_add(newr, &c);
    a->y1 = b->y1;
  }
  if(a->y2 > b->y2) {
    cuboid_copy(&c, a);
    c.y1 = b->y2+1;
    cuboid_add(newr, &c);
    a->y2 = b->y2;
  }
  if(a->z1 < b->z1) {
    cuboid_copy(&c, a);
    c.z2 = b->z1-1;
    cuboid_add(newr, &c);
    a->z1 = b->z1;
  }
  if(a->z2 > b->z2) {
    cuboid_copy(&c, a);
    c.z1 = b->z2+1;
    cuboid_add(newr, &c);
    a->z2 = b->z2;
  }
}

void cuboid_collide(int on, stack_t **regions, cuboid_t *cbd) {
  int intersected = 0;
  stack_t *newr = NULL;
  stack_t *t = *regions;
  
  while(t != NULL) {
    stack_t *x = t;
    cuboid_t *c = (cuboid_t *)t->val;
    t = t->next;
    
    int hit1 = cuboid_intersect(c, cbd);
    int hit2 = cuboid_intersect(cbd, c);
    
    // Cuboids do not intersect
    if(!hit1 && !hit2) continue;
    
    intersected = 1;
    
    // Partial intersect
    if(on) cuboid_split(&newr, cbd, c);
    else cuboid_split(&newr, c, cbd);
    
    if(!on) stack_del(regions, x);
  }
  
  // Only add if cuboid is on and there were no intersections
  if(on && !intersected) cuboid_add(regions, cbd);
  
  // For every new cuboid candidate, check if it doesn't intercept other cubes
  while(newr != NULL) {
    cuboid_t *c = (cuboid_t *)stack_pop(&newr);
    cuboid_collide(1, regions, c);
    free(c);
  }
  
}

BIGNUMBA cuboid_count(stack_t *t) {
  BIGNUMBA sum;
  
  INI_BIGNUM(sum);
  while(t != NULL) {
    cuboid_t *c = (cuboid_t *)t->val;
    t = t->next;
    
    #ifndef USE_BIG_ENUM
    // Definetely WILL happen
    if(sum+cuboid_vol(c) < sum) {
      printf("[!] OVERFLOW DETECTED!!!\nRecompile with BIGNUM\n");
      return -1;
    }
    #endif
    BIGNUMBA delta = cuboid_vol(c);
    ADD_BIGNUM(sum, delta);
    FRE_BIGNUM(delta);
  }
  
  return sum;
}

int main() {
  #ifdef USE_BIG_ENUM
    printf("[*] Using BIGENUM...\n");
    ctx = BN_CTX_new();
  #else
    printf("[*] Using unsigned long long...\n");
  #endif
  
  char opt[4];
  int on, x1,x2,y1,y2,z1,z2;

  stack_t *regions=NULL;
  
  while(scanf(" %s", opt) > 0) {
    if(scanf(" x=%d..%d,y=%d..%d,z=%d..%d\n", &x1, &x2, &y1, &y2, &z1, &z2)<=0)
      break;
    
    cuboid_t cbd = {.x1=x1, .x2=x2, .y1=y1, .y2=y2, .z1=z1, .z2=z2};
    on = (opt[1] == 'n');
    cuboid_collide(on, &regions, &cbd);
  }
  
  BIGNUMBA sum = cuboid_count(regions);
  #ifndef USE_BIG_ENUM
    printf("Sum of online cubes: %llu\n", sum);
  #else
    char *str = BN_bn2dec(sum);
    printf("Sum of online cubes: %s\n", str);
    BN_CTX_free(ctx);
    OPENSSL_free(str);
  #endif
  
  FRE_BIGNUM(sum);
  stack_free(&regions, NULL);

  return 0;
}