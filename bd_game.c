#include <stdlib.h>

#include "bd_game.h"

#define BD_UNCOVER_LOOP 69

extern int bd_cave_data[];
extern int bd_cave_start_idx[];

struct bd_game_struct_t* bd_game_initialize(int level,int difficulty)
{

	struct bd_game_struct_t* bd_game;

	bd_game = (struct bd_game_struct_t *)malloc(sizeof(struct bd_game_struct_t));
	
	
	struct cave_struct_t *cavedata;
	cavedata = (struct cave_struct_t*)&bd_cave_data[bd_cave_start_idx[level]]; 
	
	for(int x = 0; x < CAVE_WIDTH; x++) {
		for (int y = 0; y < CAVE_HEIGHT; y++) {
			bd_game->covered[x][y]=1;
			bd_game->cavemap[x][y]=BD_STEELWALL;
		}
	}



	bd_game->Tick=0;
	bd_game->Difficulty=difficulty;
	bd_game->DiamonValue=cavedata->DiamonValue;
	bd_game->DiamonValueBonus=cavedata->DiamonValueBonus;
	bd_game->DiamondsRequired=cavedata->DiamondsRequired[difficulty];
	bd_game->CaveTime=cavedata->CaveTime[difficulty];
	bd_game->Colors=cavedata->Colors;
	bd_game->MagicWallTime=cavedata->MagicWallTime;
	bd_game->AmoebaTime=cavedata->AmoebaTime;
	
	
	
	int RandSeed1 = 0;
	int RandSeed2 = cavedata->RandSeed[difficulty];

	for(int y = 1; y < CAVE_HEIGHT-1; y++) {
		for(int x = 0; x < CAVE_WIDTH; x++) {
			NextRandom(&RandSeed1, &RandSeed2);
			int store=BD_DIRT;
			for (int caveDataaddIndex = 0; caveDataaddIndex < 4; caveDataaddIndex++) {
				if (RandSeed1 < cavedata->RandomFill[caveDataaddIndex*2+1]) {
					store = cavedata->RandomFill[caveDataaddIndex*2];
				}
			}
			if((x>0)&&(x<CAVE_WIDTH-1))
				bd_game->cavemap[x][y]=store;
		}     
	} 

	int *drawidx = (&cavedata->DrawItems)+1;
	int offset=0;

	for(int item=0;item<cavedata->DrawItems;item++)
	{
		int type = *(drawidx+offset);

		switch(type)
		{
			case BD_DRAW_LINE:
			case BD_DRAW_FILLRECT:
				draw_fillrect(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),bd_game->cavemap);
				offset+=6;
				break;
			case BD_DRAW_FILLRECT2:
				draw_fillrect2(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),*(drawidx+offset+6),bd_game->cavemap);
				offset+=7;
				break;
			case BD_DRAW_RECTANGLE:
				draw_rect(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),bd_game->cavemap);
				offset+=6;
				break;
			case BD_DRAW_POINT:
				draw_point(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),bd_game->cavemap);
				offset+=4;
				break;
			case BD_DRAW_RASTER:
				draw_raster(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),*(drawidx+offset+6),*(drawidx+offset+7),bd_game->cavemap);
				offset+=8;
				break;
		}

	}

	


	return bd_game;
}

void bd_game_process(struct bd_game_struct_t* bd_game)
{
	int tick = bd_game->Tick++;


	int move_tick = tick%15;

	int uncovered = 1;

	//printf("%i %i %i\n",tick,BD_UNCOVER_LOOP,move_tick);


	if(tick < BD_UNCOVER_LOOP)
	{
		for(int line=0;line < CAVE_HEIGHT;line++)
		{
			int pos = random()%CAVE_WIDTH;
			bd_game->covered[pos][line] = 0;
		}
		uncovered=0;
	}

	int new_cavemap[CAVE_WIDTH][CAVE_HEIGHT];
	for(int y = 1; y < CAVE_HEIGHT-1; y++) 
	{
		for(int x = 0; x < CAVE_WIDTH; x++) 
		{
			new_cavemap[x][y]=bd_game->cavemap[x][y];
		}
	}


	for(int y = 1; y < CAVE_HEIGHT-1; y++) 
	{
		for(int x = 0; x < CAVE_WIDTH; x++) 
		{
			switch(bd_game->cavemap[x][y])
			{
				case BD_FIREFLYr:
					if(move_tick==0)
					{
						if((y>1)&&(bd_game->cavemap[x][y-1] == BD_SPACE))
						{
							new_cavemap[x][y-1]=BD_FIREFLYt;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if((x<CAVE_WIDTH-1)&&(bd_game->cavemap[x+1][y] == BD_SPACE))
						{
							new_cavemap[x+1][y]=BD_FIREFLYr;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if((y<CAVE_HEIGHT-1)&&(bd_game->cavemap[x][y+1] == BD_SPACE))
						{
							new_cavemap[x][y+1]=BD_FIREFLYd;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if((x>1)&&(bd_game->cavemap[x-1][y] == BD_SPACE))
						{
							new_cavemap[x-1][y]=BD_FIREFLYl;
							new_cavemap[x][y]=BD_SPACE;
						}
					};
					break;
				case BD_FIREFLYl:
					if(move_tick==0)
					{
						if((y<CAVE_HEIGHT-1)&&(bd_game->cavemap[x][y+1] == BD_SPACE))
						{
							new_cavemap[x][y+1]=BD_FIREFLYd;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if((x>1)&&(bd_game->cavemap[x-1][y] == BD_SPACE))
						{
							new_cavemap[x-1][y]=BD_FIREFLYl;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if((y>1)&&(bd_game->cavemap[x][y-1] == BD_SPACE))
						{
							new_cavemap[x][y-1]=BD_FIREFLYt;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if((x<CAVE_WIDTH-1)&&(bd_game->cavemap[x+1][y] == BD_SPACE))
						{
							new_cavemap[x+1][y]=BD_FIREFLYr;
							new_cavemap[x][y]=BD_SPACE;
						}
					};
					break;
				case BD_FIREFLYd:
					if(move_tick==0)
					{
						if((x<CAVE_WIDTH-1)&&(bd_game->cavemap[x+1][y] == BD_SPACE))
						{
							new_cavemap[x+1][y]=BD_FIREFLYr;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if((y<CAVE_HEIGHT-1)&&(bd_game->cavemap[x][y+1] == BD_SPACE))
						{
							new_cavemap[x][y+1]=BD_FIREFLYd;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if((x>1)&&(bd_game->cavemap[x-1][y] == BD_SPACE))
						{
							new_cavemap[x-1][y]=BD_FIREFLYl;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if((y>1)&&(bd_game->cavemap[x][y-1] == BD_SPACE))
						{
							new_cavemap[x][y-1]=BD_FIREFLYt;
							new_cavemap[x][y]=BD_SPACE;
						}
					};
					break;
				case BD_FIREFLYt:
					if(move_tick==0)
					{
						if((x>1)&&(bd_game->cavemap[x-1][y] == BD_SPACE))
						{
							new_cavemap[x-1][y]=BD_FIREFLYl;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if((y>1)&&(bd_game->cavemap[x][y-1] == BD_SPACE))
						{
							new_cavemap[x][y-1]=BD_FIREFLYt;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if((x<CAVE_WIDTH-1)&&(bd_game->cavemap[x+1][y] == BD_SPACE))
						{
							new_cavemap[x+1][y]=BD_FIREFLYr;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if((y<CAVE_HEIGHT-1)&&(bd_game->cavemap[x][y+1] == BD_SPACE))
						{
							new_cavemap[x][y+1]=BD_FIREFLYd;
							new_cavemap[x][y]=BD_SPACE;
						}
					};
					break;
			}
		}
	}






	for(int y = 1; y < CAVE_HEIGHT-1; y++) 
	{
		for(int x = 0; x < CAVE_WIDTH; x++) 
		{
			bd_game->cavemap[x][y] = new_cavemap[x][y];
		}
	}


}

void bd_game_render(struct bd_game_struct_t* bd_game,char display[CAVE_WIDTH][CAVE_HEIGHT])
{
	for(int y = 1; y < CAVE_HEIGHT-1; y++) 
	{
		for(int x = 0; x < CAVE_WIDTH; x++) 
		{
			if(bd_game->Tick < BD_UNCOVER_LOOP)
			{
				if(bd_game->covered[x][y])
				{
					display[x][y]=BD_STEELWALL;
				}
				else
				{
					display[x][y]=bd_game->cavemap[x][y];
				}
			}
			else
			{
				display[x][y]=bd_game->cavemap[x][y];
			}
		}
	}
}

