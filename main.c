
#include <stdio.h>

#include "bd_caves.h"
#include "bd_lib.h"


int main()
{
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



}



