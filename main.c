
#include <time.h>
#include <string.h>

#include "bd_game.h"

#include "SDL.h"
#include "sdl_util.h"
#include "main.h"

int main(int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__))) 
{
	srand(time(NULL));

	int zoom = 25;

	sdl_init(CAVE_WIDTH*zoom, (INFO_HEIGHT+CAVE_HEIGHT)*zoom,"Boudlerdash",60);


	unsigned int* pixelbuffer = malloc ((INFO_HEIGHT+CAVE_HEIGHT)*zoom * CAVE_WIDTH * zoom * sizeof(uint32_t));
	memset(pixelbuffer,0,(INFO_HEIGHT+CAVE_HEIGHT) * zoom * CAVE_WIDTH * zoom);

	struct bd_game_struct_t* bd_game = bd_game_initialize(0,0);

	int start_tick = SDL_GetTicks();

	while(sdl_handle_events(pixelbuffer)) 
	{
		int current_tick = SDL_GetTicks();

		while( (current_tick - start_tick) > 64)
		{
			bd_game_process(&bd_game,getkey);
			release_upped_keys();
			current_tick = SDL_GetTicks();
			start_tick+=64;
		}

		bd_game_render(bd_game,pixelbuffer,zoom);

	}
	free(pixelbuffer);
	sdl_deinit();
	return 0;
}



