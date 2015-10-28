
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "bd_caves.h"
#include "bd_lib.h"
#include "bd_game.h"

#include "SDL.h"
#include "main.h"

#define SDL_ZOOM 25


int main(int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__))) 
{
	srand(time(NULL));

	int zoom = SDL_ZOOM;

	sdl_init(CAVE_WIDTH*zoom, CAVE_HEIGHT*zoom,"Boudlerdash",60);


	uint32_t* pixelarray = malloc (CAVE_HEIGHT *zoom * CAVE_WIDTH * zoom * sizeof(uint32_t));
	memset(pixelarray,0,CAVE_HEIGHT * zoom * CAVE_WIDTH * zoom);

	char display[CAVE_WIDTH][CAVE_HEIGHT];

	int curr_level = 0;
	int curr_cave = 0;
	struct bd_game_struct_t* bd_game = bd_game_initialize(curr_cave,curr_level);

	int start_tick = SDL_GetTicks();

	while(sdl_handle_events(pixelarray)) 
	{
		int current_tick = SDL_GetTicks();

		while( (current_tick - start_tick) > 64)
		{
			bd_game_process(&bd_game);
			release_upped_keys();
			current_tick = SDL_GetTicks();
			start_tick+=64;
		}

		bd_game_render(bd_game,display);

		for(int y = 0; y < CAVE_HEIGHT; y++) 
		{
			for(int x = 0; x < CAVE_WIDTH; x++) 
			{
				int colors[3];	
				get_colors(display[x][y],bd_game->Tick,colors);

				uint32_t col = (colors[0]<<16)+(colors[1]<<8)+colors[2];

				if(pixelarray[((y*zoom)*CAVE_WIDTH*zoom)+x*zoom] != col)
					for(int a = 0; a < zoom;a++)
					{
						for(int b = 0;b < zoom;b++)
						{
							pixelarray[((y*zoom+a)*CAVE_WIDTH*zoom)+x*zoom+b] = col;
						}
					}
			}
		}
	}
	free(pixelarray);
	sdl_deinit();
	return 0;
}



