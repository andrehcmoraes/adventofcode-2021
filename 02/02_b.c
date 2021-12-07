#include<stdio.h>
#include<string.h>


// Nice modular macros for movement operations
#define FOREACH_MOVE(MOVE) \
        MOVE(forward)   \
        MOVE(up)  \
        MOVE(down)   \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,
#define GENERATE_STRING_LEN(STRINGL) strlen(#STRINGL),

enum MOVE_ENUM {
    FOREACH_MOVE(GENERATE_ENUM)
};

static const char *MOVE_STRING[] = {
    FOREACH_MOVE(GENERATE_STRING)
};

static const unsigned int MOVE_STRING_LENS[] = {
  FOREACH_MOVE(GENERATE_STRING_LEN)
};
// Now MOVE_STRING[forward] yields "forward" and MOVE_STRING_LENS[up] yields '2'

#define MOVE_SIZE sizeof(MOVE_STRING) / sizeof(const char *)
#define MAX_LEN(LENA, LENB) LENA > LENB ? LENA : LENB

#define STR_EQL(A,B) (strncmp(A,B,MAX_MOVE_LEN) == 0)

static char *FORMAT_READ=" %%%ds %%d";

int main() {
  // Calculate MAX length - couldn't make it work in a macro
  unsigned int MAX_MOVE_LEN = 0;
  for(int i=0; i<MOVE_SIZE;i++) 
    MAX_MOVE_LEN = MAX_LEN(MAX_MOVE_LEN, MOVE_STRING_LENS[i]);
  
  int x = 0;
  int y = 0;
  int aim = 0;
  int delta = 0;

  // Make scanf read a fixed size string %s and %d
  // Need to build a format string separately first
  int ndigits = 0;
  for(int i = MAX_MOVE_LEN+1; i > 0; i/=10) ndigits++;
  
  char buff[MAX_MOVE_LEN+1];
  char format[strlen(FORMAT_READ) + ndigits];
  sprintf(format, FORMAT_READ, MAX_MOVE_LEN+1);
  
  while(fscanf(stdin, format, buff, &delta) > 0) {
    if(STR_EQL(buff, MOVE_STRING[forward])) {
      x += delta;
      y += aim * delta;
    } else if(STR_EQL(buff, MOVE_STRING[up]))
      aim -= delta;
    else if(STR_EQL(buff, MOVE_STRING[down]))
      aim += delta;
  }
  
  printf("Depth * Pos = %d\n", x*y);
  
  return 0;
}
