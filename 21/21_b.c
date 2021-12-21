#include<stdio.h>
#include<stdlib.h>

// May overflow the maximum stack limit - should switch to dynamic allocation

#define NUM_PLAYERS 2

#define NUM_SPLITS 3
#define NUM_THROWS 3
#define MAX_POS 10
#define WIN_SCORE 21
#define NUM_OUTCOMES 7

// Least amount of points in 1 round  (2 turns) - 1
// Least amount of points in 2 rounds (4 turns) - 5  (1+4)
// Least amount of points in 3 rounds (6 turns) - 12 (1+4+7)
// ...
// 10 turns
#define MAX_TURNS 10



void game_count(int pos[], unsigned long long wins[]) {
  // Dice frequencies and values
  char rolls_freq[NUM_OUTCOMES] = {1, 3, 6, 7, 6, 3, 1};
  char rolls_delt[NUM_OUTCOMES] = {3, 4, 5, 6, 7, 8, 9};
  
  int npos, nscr;

  // Dynamic Programming matrix with PlayerxScoresxScoresxPositionsxPositions
  unsigned long long dp[MAX_TURNS][NUM_PLAYERS][WIN_SCORE][WIN_SCORE][MAX_POS][MAX_POS] = {0};
  
  // Initial universe
  // Turn 0, P1 to play, Score1 0, Score 2 0, P1 at Pos1, P2 at Pos2
  dp[0][0][0][0][pos[0]][pos[1]] = 1;
  
  // Cursed loop
  for (int t=0; t<MAX_TURNS; t++) {
  for (int pid=0; pid<NUM_PLAYERS; pid++) {
  for (int scr1=0; scr1<WIN_SCORE; scr1++) {
  for (int scr2=0; scr2<WIN_SCORE; scr2++) {
  for (int pos1=0; pos1<MAX_POS; pos1++) {
  for (int pos2=0; pos2<MAX_POS; pos2++) {
    
    unsigned long long universes = dp[t][pid][scr1][scr2][pos1][pos2];
    // Skip if no such universes available
    if(universes == 0) continue;
    
    for (int r=0; r<NUM_OUTCOMES; r++) {
      unsigned long long nuniv = universes * rolls_freq[r];
      if (pid == 0) {
        npos = pos1;
        nscr = scr1;
      } else {
        npos = pos2;
        nscr = scr2;
      }
      
      npos = (npos + rolls_delt[r]) % MAX_POS;
      nscr = nscr + npos;
      if(npos == 0) nscr += MAX_POS;

      // Already won
      if (nscr >= WIN_SCORE) {
        wins[pid] += nuniv;
        continue;
      }
      
      // Update for the next player's turn or for the next round
      if (pid == 0)
        dp[t][1][nscr][scr2][npos][pos2] += nuniv;
      else if(t + 1 < MAX_TURNS)
        dp[t+1][0][scr1][nscr][pos1][npos] += nuniv;
    }
  }}}}}}
  // Ta tudo bem agora
  
}



int main() {
  
  int x, s;
  int pos[NUM_PLAYERS];
  unsigned long long wins[NUM_PLAYERS];
  int nplayers = 0;
  while(scanf("Player %d starting position: %d\n", &x, &s) > 0) {
    pos[x-1] = s;
    wins[x-1] = 0;
    nplayers++;
  }
  
  game_count(pos, wins);
  
  printf("Most wins: %llu\n", wins[0] > wins[1] ? wins[0] : wins[1]);
  
  
  return 0;
}