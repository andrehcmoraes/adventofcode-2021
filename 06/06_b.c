#include<stdio.h>
#include<stdlib.h>

#define NUM_DAYS 256
#define TIMER_DELAY 2
#define TIMER_PERIOD 6

#ifdef USE_BIG_ENUM
  // Requires OPENSSL - install libssl-dev
  // Compile with
  // make day06_bn
  #include <openssl/bn.h>
  #define ADD_BIGNUM(a,b) BN_add(a,a,b);
  #define INI_BIGNUM(a) { a =  BN_new(); BN_zero(a); }
  #define SET_BIGNUM(a,b) BN_copy(a,b);
  #define ZER_BIGNUM(a) BN_is_zero(a)
  #define BN_ONE BN_value_one()
#else  
  #define ADD_BIGNUM(a,b) a = a+b;
  #define INI_BIGNUM(a) a = ((unsigned long long) 0);
  #define SET_BIGNUM(a,b) a = b;
  #define ZER_BIGNUM(a) a == ((unsigned long long) 0)
  #define BN_ONE ((unsigned long long) 1)
#endif

int main() {
  
  #ifdef USE_BIG_ENUM
    printf("[*] Using BIGENUM...\n");
    BN_CTX *ctx;
    BIGNUM *sum;
    BIGNUM **timers;
    
    ctx = BN_CTX_new();
    
    timers = (BIGNUM **)malloc(NUM_DAYS * sizeof(*timers));
  #else
    printf("[*] Using unsigned long long...\n");
    unsigned long long sum;
    unsigned long long timers[NUM_DAYS];
  #endif

  INI_BIGNUM(sum);
  for(int i=0; i<NUM_DAYS; i++)
    INI_BIGNUM(timers[i]);
  
  // Read until tail non-comma
  int n;
  char c;
  
  while(fscanf(stdin, " %d%c", &n, &c) > 0) {
    ADD_BIGNUM(sum, BN_ONE);
    ADD_BIGNUM(timers[n], BN_ONE);
    if(c != ',') break;
  }

  
  for(int i=0; i<NUM_DAYS; i++) {
    if(ZER_BIGNUM(timers[i])) continue;
    
    n = i + TIMER_PERIOD+1;
    if(n < NUM_DAYS) ADD_BIGNUM(timers[n], timers[i]);
    n = i + TIMER_DELAY + TIMER_PERIOD+1;
    if(n < NUM_DAYS) ADD_BIGNUM(timers[n], timers[i]);
    
    #ifndef USE_BIG_ENUM
      // Hopefully won't ever happen...
      if(sum+timers[i] < sum) {
        printf("[!] OVERFLOW DETECTED!!!\nRecompile with BIGNUM\n");
        return -1;
      }
    #endif

    ADD_BIGNUM(sum, timers[i]);
  }
  
  printf("Fish at %d days: ", NUM_DAYS);
  #ifdef USE_BIG_ENUM
    char *str = BN_bn2dec(sum);
    printf("%s\n", str);
    for(int i=0; i<NUM_DAYS; i++)
      BN_free(timers[i]);
    BN_CTX_free(ctx);
    BN_free(sum);
    free(timers);
    free(str);
  #else
    printf("%llu\n", sum);
  #endif
  
  return 0;
}
