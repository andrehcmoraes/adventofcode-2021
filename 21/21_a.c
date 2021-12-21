#include<stdio.h>
#include<stdlib.h>

#define MAX_PLAYERS 2
#define NUM_THROWS 3
#define MAX_DICE 100
#define MAX_POS 10
#define WIN_SCORE 1000

typedef struct player {
  unsigned long long pos, score;
} player_t;

player_t players[MAX_PLAYERS];
unsigned long long dice = 1;
unsigned long long dcount = 0;

unsigned long long dice_throw(int n) {
  unsigned long long s = 0;
  for(int i=0; i<n; i++) {
    s += dice;
    dice++;
    if(dice > MAX_DICE) dice = 1;
  }
  dcount += n;
  return s;
}


int game_play(int n, unsigned long long score) {
  unsigned long long maxscr = 0;
  int p;
  while(maxscr < score) {
    for(p=0; p<n; p++) {
      unsigned long long pos = players[p].pos;
      pos += dice_throw(NUM_THROWS) % MAX_POS;
      players[p].pos = pos > MAX_POS ? pos - MAX_POS : pos;
      players[p].score += players[p].pos;
      maxscr = maxscr < players[p].score ? players[p].score : maxscr;
      if(maxscr >= score) break;
    }
  }
  
  return p;
}

int main() {
  
  int nplayers, x, s;
  nplayers = 0;
  while(scanf("Player %d starting position: %d\n", &x, &s) > 0) {
    players[x-1].pos = s;
    players[x-1].score = 0;
    nplayers++;
  }
  
  int winp = game_play(nplayers, WIN_SCORE);
  printf("\nP%d won with %llu\n", winp+1, players[winp].score);


  unsigned long long lostscr = players[winp].score;
  for(int i=0; i<nplayers; i++)
    lostscr = lostscr > players[i].score ? players[i].score : lostscr;
  
  printf("Losing score * dcount: %llu\n", lostscr*dcount);
  
  
  return 0;
}