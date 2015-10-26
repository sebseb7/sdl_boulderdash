
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "bd_caves.h"
#include "bd_lib.h"
#include "bd_game.h"

#include "SDL.h"
#include "sdl_util.h"

#define SDL_ZOOM 25

int keypressmap[8];
int keymap;


int getkey(int key)
{
	if(keypressmap[key] > 15)
	{
		return 1;
	}
	else if((keymap >> key) & 1)
	{
		keymap &= ~(1 << key);
		return 1;
	}
	return 0;
}


int main(int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__))) 
{
	srand(time(NULL));

	sdl_init(CAVE_WIDTH*SDL_ZOOM, CAVE_HEIGHT*SDL_ZOOM,"Boudlerdash",60);


	uint32_t **pixelarray = malloc (CAVE_HEIGHT *SDL_ZOOM* sizeof(int *) + (CAVE_HEIGHT * SDL_ZOOM* (SDL_ZOOM * CAVE_WIDTH * sizeof(uint32_t))));

	for (int i = 0; i < CAVE_HEIGHT*SDL_ZOOM; i++) 
	{
		pixelarray[i] = (uint32_t*)(&pixelarray[CAVE_HEIGHT * SDL_ZOOM])+(CAVE_WIDTH*SDL_ZOOM*i);
	}

	char display[CAVE_WIDTH][CAVE_HEIGHT];

	int curr_level = 0;
	int curr_cave = 0;
	struct bd_game_struct_t* bd_game = bd_game_initialize(curr_cave,curr_level);

	int running = 1;

	int start_tick = SDL_GetTicks();
	
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
							keypressmap[0]=0;
							break;
						case SDLK_RIGHT:
							keypressmap[1]=0;
							break;
						case SDLK_DOWN:
							keypressmap[2]=0;
							break;
						case SDLK_LEFT:
							keypressmap[3]=0;
							break;
						case SDLK_F1:
							keypressmap[4]=0;
							break;
						case SDLK_F2:
							keypressmap[5]=0;
							break;
						case SDLK_F3:
							keypressmap[6]=0;
							break;
						case SDLK_F4:
							keypressmap[7]=0;
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
							keymap |= 1 << 0;
							keypressmap[0]=1;
							break;
						case SDLK_RIGHT:
							keymap |= 1 << 1;
							keypressmap[1]=1;
							break;
						case SDLK_DOWN:
							keymap |= 1 << 2;
							keypressmap[2]=1;
							break;
						case SDLK_LEFT:
							keymap |= 1 << 3;
							keypressmap[3]=1;
							break;
						case SDLK_F1:
							keymap |= 1 << 4;
							keypressmap[4]=1;
							break;
						case SDLK_F2:
							keymap |= 1 << 5;
							keypressmap[5]=1;
							break;
						case SDLK_F3:
							keymap |= 1 << 6;
							keypressmap[6]=1;
							break;
						case SDLK_F4:
							keymap |= 1 << 7;
							keypressmap[7]=1;
							break;
						default: break;
					}
				default: break;
			}
		}

		for(int i = 0; i < 4; i++)
			if(keypressmap[i]>0)keypressmap[i]++;

		int current_tick = SDL_GetTicks();

		while( (current_tick - start_tick) > 64)
		{
			bd_game_process(&bd_game);
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

				if(pixelarray[y*SDL_ZOOM][x*SDL_ZOOM] != col)
					for(int a = 0; a < SDL_ZOOM;a++)
					{
						for(int b = 0;b < SDL_ZOOM;b++)
						{
							pixelarray[y*SDL_ZOOM+a][x*SDL_ZOOM+b] = col;
						}
					}
			}
		}
		
		sdl_loop((uint32_t*)&pixelarray[CAVE_HEIGHT * SDL_ZOOM]);
		/*
		 *try: SDL_RenderFillRects(SDL_Renderer*   renderer,const SDL_Rect* rects,int             count)
		 *
		 */
	}

	sdl_deinit();
	return 0;
}



