#ifndef _BD_GAME_H
#define _BD_GAME_H


#include "bd_lib.h"


#define CAVEMAP_COVERED 0

struct bd_game_struct_t 
{
	int Tick;
	int Cave;
	int Difficulty;
	int DiamonValue;
	int DiamonValueBonus;
	int DiamondsRequired;
	int DiamondsCollected;
	int Won;
	int Lost;
	int CaveTime;
	int* Colors;
	int MagicWallActive;
	int MagicWallTime;
	int AmoebaTime;
	int AmoebaSpace;
	int SlimeSeed1;
	int SlimeSeed2;
	int Intermission;
	char covered[CAVE_WIDTH][CAVE_HEIGHT];
	int cavemap[CAVE_WIDTH][CAVE_HEIGHT];
} __attribute__ ((packed));


struct bd_game_struct_t* bd_game_initialize(int level,int difficulty);

void bd_game_process(struct bd_game_struct_t** bd_game,int getkey(int));
void bd_game_render(struct bd_game_struct_t* bd_game,unsigned int* pixelbuffer,int zoom);

#endif
