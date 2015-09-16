#ifndef _BD_GAME_H
#define _BD_GAME_H


#include "bd_lib.h"

struct bd_game_struct_t 
{
	int Difficulty;
	int DiamonValue;
	int DiamonValueBonus;
	int DiamondsRequired;
	int CaveTime;
	int Colors[3];
	int MagicWallTime;
	int AmoebaTime;
	int cavemap[CAVE_WIDTH][CAVE_HEIGHT];
} __attribute__ ((packed));


#endif
