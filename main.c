
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "bd_caves.h"
#include "bd_lib.h"
#include "bd_game.h"

#include <SDL/SDL.h>

static SDL_Surface* screen;

#define SDL_ZOOM 30

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


	screen = SDL_SetVideoMode(CAVE_WIDTH*SDL_ZOOM,CAVE_HEIGHT*SDL_ZOOM,32, SDL_SWSURFACE | SDL_DOUBLEBUF);

	struct bd_game_struct_t* bd_game = bd_game_initialize(0,0);

/*
	int idxtest=0;
	for(int cave=0;cave<bd_caves;cave++)
	{
		int level=0;

		if(idxtest != bd_cave_start_idx[cave])
		{
			printf("%i %i\n",idxtest,bd_cave_start_idx[cave]);
			return 1;
		}

		idxtest+=sizeof(struct cave_struct_t)/sizeof(int);

		int field[CAVE_WIDTH][CAVE_HEIGHT];
		
		idxtest+=render_field(cave,level,field);

		for(int y = 0; y < CAVE_HEIGHT; y++) {
			for(int x = 0; x < CAVE_WIDTH; x++) {

				switch(field[x][y])
				{
					case 0:
						printf("E");
						break;
					case BD_BOULDER:
						printf("\033[1;30m*\033[0m");
						break;
					case BD_SPACE:
						printf(" ");
						break;
					case BD_FIREFLYl:
					case BD_FIREFLYr:
						printf("\033[1;33m@\033[0m");
						break;
					case	BD_DIAMOND:
						printf("\033[1;36m$\033[0m");
						break;
					case BD_INBOX:
						printf("X");
						break;
					case BD_OUTBOX:
						printf("H");
						break;
					case BD_WALL:
						printf("W");
						break;
					case BD_STEELWALL:
						printf("\033[0;34m#\033[0m");
						break;
					case BD_DIRT:
						printf(".");
						break;
					case BD_MAGICWALL:
						printf("\033[1;37mM\033[0m");
						break;
					case BD_AMOEBA:
						printf("\033[0;32mA\033[0m");
						break;
					default:
						printf("(%i)",field[x][y]);
						break;
				}
			}     
			printf("\n");
		}  
		printf("\n\n");

	}
	
*/

	int running = 1;


	int direction=0;

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

		bd_game_process(bd_game,direction);
	
		char display[CAVE_WIDTH][CAVE_HEIGHT];

		bd_game_render(bd_game,display);
	
		for(int y = 0; y < CAVE_HEIGHT; y++) 
		{
			for(int x = 0; x < CAVE_WIDTH; x++) 
			{
				int colors[3];	
				get_colors(display[x][y],bd_game->Tick,colors);
				
				
				
				SDL_Rect rect = { SDL_ZOOM*x, SDL_ZOOM*y, SDL_ZOOM,SDL_ZOOM};
				SDL_FillRect(
					screen, 
					&rect, 
					SDL_MapRGB(screen->format, colors[0],colors[1],colors[2])
				);
			}
		}
		
		SDL_Flip(screen);
		SDL_Delay(16);


	}


	SDL_Quit();
	return 0;

}



