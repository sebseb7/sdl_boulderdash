
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "bd_caves.h"
#include "bd_lib.h"
#include "bd_game.h"

#include "SDL.h"
#include "sdl_util.h"

#define SDL_ZOOM 25

static int keymap;
static int releasemap;
static int ackmap;

int getkey(int key)
{
	if((keymap >> key) & 1)
	{
		ackmap |= (1 << key);
		return 1;
	}
	return 0;
}

static void release_upped_keys(void)
{
	for(int i = 0; i < 8;i++)
	{
		if(releasemap & (1<<i))
		{
			keymap &= ~(1 << i);
		}
	}
	releasemap = 0;
}


static void keyup(int key)
{
	if(ackmap & (1<<key))
	{
		keymap &= ~(1 << key);
	}
	else
	{
		releasemap |= (1 << key);
	}
}

static void keydown(int key)
{
	keymap = 1 << key;
	ackmap &= ~(1 << key);
}

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

	int running = 1;

	int start_tick = SDL_GetTicks();

	releasemap=0;


	while(running) 
	{
		SDL_Event ev;
		while(SDL_PollEvent(&ev)) 
		{
			switch(ev.type) {
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYUP:
					switch(ev.key.keysym.sym) 
					{
						case SDLK_UP:
							keyup(0);
							break;
						case SDLK_RIGHT:
							keyup(1);
							break;
						case SDLK_DOWN:
							keyup(2);
							break;
						case SDLK_LEFT:
							keyup(3);
							break;
						case SDLK_F1:
							keyup(4);
							break;
						case SDLK_F2:
							keyup(5);
							break;
						case SDLK_F3:
							keyup(6);
							break;
						case SDLK_F4:
							keyup(7);
							break;
						default: break;
					}
					break;
				case SDL_KEYDOWN:
					switch(ev.key.keysym.sym) 
					{
						case SDLK_ESCAPE:
							running = 0;
							break;
						case SDLK_UP:
							keydown(0);
							break;
						case SDLK_RIGHT:
							keydown(1);
							break;
						case SDLK_DOWN:
							keydown(2);
							break;
						case SDLK_LEFT:
							keydown(3);
							break;
						case SDLK_F1:
							keydown(4);
							break;
						case SDLK_F2:
							keydown(5);
							break;
						case SDLK_F3:
							keydown(6);
							break;
						case SDLK_F4:
							keydown(7);
							/*
							   zoom=20;
							   sdl_windowsize(CAVE_WIDTH*zoom, CAVE_HEIGHT*zoom);


							   free(pixelarray);
							   pixelarray = malloc (CAVE_HEIGHT *zoom * CAVE_WIDTH * zoom * sizeof(uint32_t));
							   memset(pixelarray,0,CAVE_HEIGHT *zoom * CAVE_WIDTH * zoom * sizeof(uint32_t));*/
							break;
						default: break;
					}
				default: break;
			}
		}

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

		sdl_loop(pixelarray);
		/*
		 *try: SDL_RenderFillRects(SDL_Renderer*   renderer,const SDL_Rect* rects,int             count)
		 *
		 */
	}
	free(pixelarray);
	sdl_deinit();
	return 0;
}



