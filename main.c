
#include <stdio.h>
#include <sys/time.h>

#include "bd_caves.h"
#include "bd_lib.h"
#include "bd_game.h"

#include <SDL/SDL.h>

static SDL_Surface* screen;

#define SDL_ZOOM 15

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
					break;
				case SDL_KEYDOWN:
					switch(ev.key.keysym.sym) 
					{
						case SDLK_ESCAPE:
							running = 0;
							break;
						default: break;
					}
				default: break;
			}
		}

		bd_game_process(bd_game);
	
		char display[CAVE_WIDTH][CAVE_HEIGHT];

		bd_game_render(bd_game,display);
	
		for(int y = 1; y < CAVE_HEIGHT-1; y++) 
		{
			for(int x = 0; x < CAVE_WIDTH; x++) 
			{
				
				
				
				
				
				SDL_Rect rect = { SDL_ZOOM*x, SDL_ZOOM*y, SDL_ZOOM,SDL_ZOOM};
				SDL_FillRect(
					screen, 
					&rect, 
					SDL_MapRGB(screen->format, display[x][y]*16,0,0)
				);
			}
		}
		
		SDL_Flip(screen);
		SDL_Delay(10);

		/*if( now < animations[current_animation].timing )
		{
			SDL_Delay(animations[current_animation].timing - now);
		}
		lastFrame = SDL_GetTicks();
*/
		

	}


	SDL_Quit();
	return 0;

}



