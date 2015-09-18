#ifndef _BD_GAME_H
#define _BD_GAME_H


#include "bd_lib.h"


#define CAVEMAP_COVERED 0

struct bd_game_struct_t 
{
	int Tick;
	int Difficulty;
	int DiamonValue;
	int DiamonValueBonus;
	int DiamondsRequired;
	int CaveTime;
	int* Colors;
	int MagicWallActive;
	int MagicWallTime;
	int AmoebaTime;
	char covered[CAVE_WIDTH][CAVE_HEIGHT];
	int cavemap[CAVE_WIDTH][CAVE_HEIGHT];
} __attribute__ ((packed));


struct bd_game_struct_t* bd_game_initialize(int level,int difficulty);

void bd_game_process(struct bd_game_struct_t* bd_game,int direction);
void bd_game_render(struct bd_game_struct_t* bd_game,char display[CAVE_WIDTH][CAVE_HEIGHT]);

#endif
