#include <stdlib.h>

#include "bd_game.h"


struct bd_game_struct_t* bd_game_initialize(int level,int difficulty)
{

	struct bd_game_struct_t* bd_game;

	bd_game = (struct bd_game_struct_t *)malloc(sizeof(struct bd_game_struct_t));



	return bd_game;
}

