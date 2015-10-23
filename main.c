
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "bd_caves.h"
#include "bd_lib.h"
#include "bd_game.h"

#include "SDL.h"

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

static void SetSDLIcon(SDL_Window* window)
{
	// this will "paste" the struct my_icon into this function
#include "bd_icon.c"

	// these masks are needed to tell SDL_CreateRGBSurface(From)
	// to assume the data it gets is byte-wise RGB(A) data
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	int shift = (bd_icon.bytes_per_pixel == 3) ? 8 : 0;
	rmask = 0xff000000 >> shift;
	gmask = 0x00ff0000 >> shift;
	bmask = 0x0000ff00 >> shift;
	amask = 0x000000ff >> shift;
#else // little endian, like x86
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = (bd_icon.bytes_per_pixel == 3) ? 0 : 0xff000000;
#endif

	SDL_Surface* icon = SDL_CreateRGBSurfaceFrom((void*)bd_icon.pixel_data, bd_icon.width,
			bd_icon.height, bd_icon.bytes_per_pixel*8, bd_icon.bytes_per_pixel*bd_icon.width,
			rmask, gmask, bmask, amask);
	SDL_SetWindowIcon(window, icon);

	SDL_FreeSurface(icon);
}

int main(int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__))) 
{
	srand(time(NULL));

	SDL_Window* window = SDL_CreateWindow( "Boudlerdash", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, CAVE_WIDTH*SDL_ZOOM, CAVE_HEIGHT*SDL_ZOOM, SDL_WINDOW_SHOWN );
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Texture* texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STATIC,CAVE_WIDTH*SDL_ZOOM, CAVE_HEIGHT*SDL_ZOOM);

	SetSDLIcon(window);

	uint32_t **pixelarray = malloc (CAVE_HEIGHT *SDL_ZOOM* sizeof(int *) + (CAVE_HEIGHT * SDL_ZOOM* (SDL_ZOOM * CAVE_WIDTH * sizeof(uint32_t))));
	uint32_t *offs = (uint32_t*)&pixelarray[CAVE_HEIGHT * SDL_ZOOM]; 
	uint32_t *pixeldata = (uint32_t*)&pixelarray[CAVE_HEIGHT * SDL_ZOOM]; 

	for (int i = 0; i < CAVE_HEIGHT*SDL_ZOOM; i++, offs += CAVE_WIDTH*SDL_ZOOM) 
	{
		pixelarray[i] = offs;
	}

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

		SDL_UpdateTexture(texture, NULL, pixeldata, CAVE_WIDTH*SDL_ZOOM * sizeof(Uint32));//update only the updated rects
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
	free(pixelarray);
	SDL_Quit();
	return 0;
}



