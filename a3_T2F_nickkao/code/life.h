
#ifndef LIFE_H

#define LIFE_H 1

#define N (2 * 1024)


void initLife();
int life(long oldWorld[N][N], long newWorld[N][N]);
int life_orig(long oldWorld[N][N], long newWorld[N][N]);
int checkHealth(long newCell, long oldCell);
void printWorld(long world[N][N]);

#endif

