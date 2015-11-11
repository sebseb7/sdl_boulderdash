
#include <time.h>
#include <stdlib.h>


#include "bd_game.h"
#include "sdl_util.h"

int main(int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__))) 
{
	srand(time(NULL));

	int zoom = 21;

	unsigned int* pixelbuffer = sdl_init(CAVE_WIDTH*zoom, (INFO_HEIGHT+CAVE_HEIGHT)*zoom,"Boulderdash",60);

	struct bd_game_struct_t* bd_game = bd_game_initialize(0,0);

	int limiter=0;

	while(sdl_handle_events(pixelbuffer)) //limits loop to 60fps
	{
		while(sdl_limit_fps(&limiter,8))
		{
			bd_game_process(&bd_game,getkey);
			release_upped_keys();
		}

		bd_game_render(bd_game,pixelbuffer,zoom);

	}
	sdl_deinit();
	return 0;
}



