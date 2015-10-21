
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "bd_caves.h"
#include "bd_lib.h"
#include "bd_game.h"

#include <SDL.h>

#define SDL_ZOOM 25

int keypressmap[4];
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

	SDL_Window* window = SDL_CreateWindow( "Boudlerdash", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, CAVE_WIDTH*SDL_ZOOM, CAVE_HEIGHT*SDL_ZOOM, SDL_WINDOW_SHOWN );
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Texture* texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STATIC,CAVE_WIDTH*SDL_ZOOM, CAVE_HEIGHT*SDL_ZOOM);

	uint32_t pixels[CAVE_HEIGHT*SDL_ZOOM][CAVE_WIDTH*SDL_ZOOM];
	char display[CAVE_WIDTH][CAVE_HEIGHT];

	int curr_level = 0;
	int curr_cave = 0;
	struct bd_game_struct_t* bd_game = bd_game_initialize(curr_cave,curr_level);

	int running = 1;

	const int fps = 60;
	const int fpsMill = 1000/fps;
	
	while(running) 
	{
		int current_time=SDL_GetTicks();

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
						default: break;
					}
				default: break;
			}
		}
						
		for(int i = 0; i < 4; i++)
			if(keypressmap[i]>0)keypressmap[i]++;

		bd_game_process(&bd_game);
	

		bd_game_render(bd_game,display);
	
		for(int y = 0; y < CAVE_HEIGHT; y++) 
		{
			for(int x = 0; x < CAVE_WIDTH; x++) 
			{
				int colors[3];	
				get_colors(display[x][y],bd_game->Tick,colors);
		
				uint32_t col = (colors[0]<<16)+(colors[1]<<8)+colors[2];

				if(pixels[y*SDL_ZOOM][x*SDL_ZOOM] != col)
					for(int a = 0; a < SDL_ZOOM;a++)
					{
						for(int b = 0;b < SDL_ZOOM;b++)
						{
							pixels[y*SDL_ZOOM+a][x*SDL_ZOOM+b] = col;
						}
					}
			}
		}
		
		SDL_UpdateTexture(texture, NULL, pixels, CAVE_WIDTH*SDL_ZOOM * sizeof(Uint32));//update only the updated rects
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		int currentSpeed = SDL_GetTicks() - current_time;
		if(fpsMill > currentSpeed) {
			SDL_Delay(fpsMill - currentSpeed);
		}
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;

}



