#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bd_caves.h"
#include "bd_game.h"


#define BD_UNCOVER_LOOP 9
#define BD_START_DELAY 12

extern const int bd_cave_data[];
extern const int bd_cave_start_idx[];

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

	bd_game->Won=0;
	bd_game->Lost=0;
	bd_game->Tick=0;
	bd_game->SlimeSeed1=0;
	bd_game->SlimeSeed2=0;
	bd_game->DiamondsCollected=0;
	bd_game->Player_X=0;
	bd_game->Cave=level;
	bd_game->Difficulty=difficulty;
	bd_game->DiamonValue=cavedata->DiamonValue;
	bd_game->DiamonValueBonus=cavedata->DiamonValueBonus;
	bd_game->DiamondsRequired=cavedata->DiamondsRequired[difficulty];
	bd_game->CaveTime=cavedata->CaveTime[difficulty];
	bd_game->Colors=cavedata->Colors;
	bd_game->MagicWallTime=cavedata->MagicWallTime;
	bd_game->MagicWallActive=0;
	bd_game->AmoebaTime=cavedata->AmoebaTime;
	bd_game->AmoebaSpace=1;
	bd_game->Intermission=0;

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

	if(level==4)
	{
		bd_game->Intermission=1;
		for(int y = 1; y < CAVE_HEIGHT-1; y++) {
			for(int x = 0; x < CAVE_WIDTH; x++) {
				if((x>19)||(y>10))
					bd_game->cavemap[x][y]=BD_STEELWALL;
			}     
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
				bd_draw_fillrect(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),bd_game->cavemap);
				offset+=6;
				break;
			case BD_DRAW_FILLRECT2:
				bd_draw_fillrect2(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),*(drawidx+offset+6),bd_game->cavemap);
				offset+=7;
				break;
			case BD_DRAW_RECTANGLE:
				bd_draw_rect(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),bd_game->cavemap);
				offset+=6;
				break;
			case BD_DRAW_POINT:
				bd_draw_point(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),bd_game->cavemap);
				offset+=4;
				break;
			case BD_DRAW_RASTER:
				bd_draw_raster(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),*(drawidx+offset+6),*(drawidx+offset+7),bd_game->cavemap);
				offset+=8;
				break;
		}

	}




	return bd_game;
}

static void explode(int map[CAVE_WIDTH][CAVE_HEIGHT],int x,int y)
{

	for(int dy=-1;dy<2;dy++)
		for(int dx=-1;dx<2;dx++)
			if(map[x+dx][y+dy]!=BD_STEELWALL)
				map[x+dx][y+dy]=BD_EXPLOSION1;


}

void bd_game_process(struct bd_game_struct_t** bd_game_ptr,int getkey(int))
{
	struct bd_game_struct_t* bd_game = *bd_game_ptr;
	bd_game->Tick++;
	int tick = bd_game->Tick;



	int move_tick = 0;
	int fall_tick = 0;
	int expl_tick = 0;

	if(getkey(6))
	{
		bd_game->Won=1;
	}

	if(bd_game->Won > 0)
	{
		move_tick=1;
		fall_tick=1;

		bd_game->Won++;


		if(bd_game->Won == 25)
		{
			int old_cave = bd_game->Cave;
			int old_difficulty = bd_game->Difficulty;

			old_cave++;
			if(old_cave==7)
			{
				old_cave=0;
				old_difficulty++;
			}
			if(old_difficulty==5)
			{
				old_cave=0;
				old_difficulty=0;
			}

			free(bd_game);
			bd_game = bd_game_initialize(old_cave,old_difficulty); 
			*bd_game_ptr = bd_game;
			return;
		}
	}

	if(bd_game->Lost > 0)
	{
		bd_game->Lost++;

		if(bd_game->Lost == 25)
		{
			int old_cave = bd_game->Cave;
			int old_difficulty = bd_game->Difficulty;

			free(bd_game);
			bd_game = bd_game_initialize(old_cave,old_difficulty); 
			*bd_game_ptr = bd_game;
			return;
		}
	}



	int new_cavemap[CAVE_WIDTH][CAVE_HEIGHT];
	for(int y = 0; y < CAVE_HEIGHT; y++) 
	{
		for(int x = 0; x < CAVE_WIDTH; x++) 
		{
			new_cavemap[x][y]=bd_game->cavemap[x][y];
		}
	}

	int amoeba_possible=0;

	for(int y =0; y < CAVE_HEIGHT ; y++) 
	{
		for(int x = 0; x < CAVE_WIDTH; x++) 
		{
			int curr_type = bd_game->cavemap[x][y];
			if(bd_game->cavemap[x][y] != new_cavemap[x][y])
			{
				continue;
			};
			int fall=1;
			switch(curr_type)
			{
				case BD_AMOEBA:
					if(bd_game->AmoebaSpace==0)
					{
						new_cavemap[x][y]=BD_DIAMOND;
					}
					else
						for(int dir = 1;dir<5;dir++)
						{
							if(
									(new_cavemap[x+move_x(dir)][y+move_y(dir)] == BD_SPACE)||
									(new_cavemap[x+move_x(dir)][y+move_y(dir)] == BD_DIRT)
							  )
							{
								amoeba_possible++;
								if(((bd_game->AmoebaTime*8) < tick)&&(rand()%15==0))
								{
									new_cavemap[x+move_x(dir)][y+move_y(dir)]=BD_AMOEBA;
								}
								else if((bd_game->Tick>BD_START_DELAY)&&(rand()%100==0))
								{
									new_cavemap[x+move_x(dir)][y+move_y(dir)]=BD_AMOEBA;
								}
							}
						}
					break;
				case BD_OUTBOX:
					if(bd_game->DiamondsCollected >= bd_game->DiamondsRequired)
					{
						new_cavemap[x][y]=BD_OUTBOXactive;
					}
					break;
				case BD_EXPLOSION1:
				case BD_EXPLOSION2:
				case BD_EXPLOSION3:
				case BD_EXPLOSION4:
					if(expl_tick ==0)
					{
						new_cavemap[x][y]=curr_type+1;
					}
					break;
				case BD_EXPLOSION5:
					if(expl_tick ==0)
					{
						new_cavemap[x][y]=BD_SPACE;
					}
					break;
				case BD_INBOX:
					bd_game->Player_X=x;
					if(bd_game->Tick == BD_START_DELAY)
						new_cavemap[x][y]=BD_ROCKFORD;

					break;
				case BD_ROCKFORDgrab:
				case BD_ROCKFORD:
					if(move_tick ==0)
					{

						int direction=0;

						if(getkey(5))
						{
							bd_game->Lost=1;
							explode(new_cavemap,x,y);
							break;
						}
						if((bd_game->Won==0)&&((bd_game->CaveTime - (bd_game->Tick / 8)) < 1))
						{
							bd_game->Lost=1;
							explode(new_cavemap,x,y);
							break;
						}

						if(getkey(1))
						{
							direction=2;
						} 
						else if(getkey(2))
						{
							direction=3;
						}
						else if(getkey(3))
						{
							direction=4;
						}
						else if(getkey(0))
						{
							direction=1;
						};

						if(direction != 0)
						{
							if(
									(new_cavemap[x+move_x(direction)][y+move_y(direction)] == BD_SPACE)||
									(new_cavemap[x+move_x(direction)][y+move_y(direction)] == BD_DIRT)
							  )
							{
								new_cavemap[x+move_x(direction)][y+move_y(direction)]=BD_ROCKFORD;
								new_cavemap[x][y]=BD_SPACE;
								bd_game->Player_X = x+move_x(direction);
							}
							if(
									new_cavemap[x+move_x(direction)][y+move_y(direction)] == BD_OUTBOXactive
							  )
							{
								new_cavemap[x+move_x(direction)][y+move_y(direction)]=BD_ROCKFORD;
								new_cavemap[x][y]=BD_SPACE;
								bd_game->Player_X = x+move_x(direction);
								bd_game->Won=1;
							}
							else if(
									(new_cavemap[x+move_x(direction)][y+move_y(direction)] == BD_DIAMOND)||
									(new_cavemap[x+move_x(direction)][y+move_y(direction)] == BD_DIAMONDfall)
								   )
							{
								new_cavemap[x+move_x(direction)][y+move_y(direction)] = BD_ROCKFORD;
								new_cavemap[x][y] = BD_SPACE;
								bd_game->Player_X = x+move_x(direction);
								bd_game->DiamondsCollected++;
							}
							else if(
									(new_cavemap[x+move_x(direction)][y] == BD_BOULDER)&&
									(move_y(direction)==0)
								   )
							{
								if(curr_type == BD_ROCKFORDgrab)
								{
									if(new_cavemap[x+move_x(direction)*2][y] == BD_SPACE)
									{
										new_cavemap[x+move_x(direction)*2][y] = BD_BOULDER;
										new_cavemap[x+move_x(direction)][y] = BD_ROCKFORD;
										bd_game->Player_X = x+move_x(direction);
										new_cavemap[x][y]=BD_SPACE;
									}
								}
								else
								{
									new_cavemap[x][y]=BD_ROCKFORDgrab;
								}
							}

						}
					}
					break;
				case BD_BOULDERfall:
				case BD_DIAMONDfall:
					if(fall_tick == 0)
					{
						if(
								(new_cavemap[x][y+1] == BD_FIREFLYt)||
								(new_cavemap[x][y+1] == BD_FIREFLYl)||
								(new_cavemap[x][y+1] == BD_FIREFLYr)||
								(new_cavemap[x][y+1] == BD_FIREFLYd)||
								(new_cavemap[x][y+1] == BD_ROCKFORD)
						  )
						{
							if(new_cavemap[x][y+1] == BD_ROCKFORD)	
								bd_game->Lost=1;
							explode(new_cavemap,x,y+1);

						}
						else if(
								(curr_type == BD_BOULDERfall)&&
								(new_cavemap[x][y+1] == BD_MAGICWALL)
							   )
						{
						}
						else if(
								(new_cavemap[x][y+1] != BD_SPACE)&&
								(new_cavemap[x][y+1] != BD_MAGICWALLactive)
							   )
						{
							new_cavemap[x][y]-=1;
						}
						fall=0;
					}
				case BD_BOULDER:
				case BD_DIAMOND:
					if(fall_tick == 0)
					{
						if(
								((new_cavemap[x][y] == BD_BOULDER)||(new_cavemap[x][y] == BD_DIAMOND))&&
								(new_cavemap[x][y+1] == BD_SPACE)
						  )
						{
							new_cavemap[x][y+1]=curr_type+fall;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if(
								new_cavemap[x][y+1] == BD_SPACE
							   )
						{
							new_cavemap[x][y] = BD_SPACE;
							new_cavemap[x][y+1]=curr_type+fall;
						}
						else if(
								((new_cavemap[x][y+1] == BD_BOULDER)||(new_cavemap[x][y+1] == BD_DIAMOND))&&
								(new_cavemap[x-1][y+1] == BD_SPACE)&&
								(new_cavemap[x-1][y] == BD_SPACE)

							   )
						{
							new_cavemap[x-1][y]=curr_type+fall;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if(
								((new_cavemap[x][y+1] == BD_BOULDER)||(new_cavemap[x][y+1] == BD_DIAMOND))&&
								(new_cavemap[x+1][y+1] == BD_SPACE)&&
								(new_cavemap[x+1][y] == BD_SPACE)

							   )
						{
							new_cavemap[x+1][y]=curr_type+fall;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if(
								(new_cavemap[x][y+1] == BD_MAGICWALL)&&
								(new_cavemap[x][y] == BD_BOULDERfall)
							   )
						{
							bd_game->MagicWallActive=1;
						}
						else if(
								(new_cavemap[x][y+1] == BD_MAGICWALLactive)&&
								((new_cavemap[x][y] == BD_DIAMONDfall)||
								 (new_cavemap[x][y] == BD_BOULDERfall))&&
								(new_cavemap[x][y+2] == BD_SPACE)
							   )
						{
							new_cavemap[x][y+2]=BD_DIAMONDfall;
							new_cavemap[x][y]=BD_SPACE;
						}
						else if(
								(new_cavemap[x][y+1] == BD_MAGICWALLactive)&&
								((new_cavemap[x][y] == BD_DIAMONDfall)||
								 (new_cavemap[x][y] == BD_BOULDERfall))&&
								(new_cavemap[x][y+2] != BD_SPACE)
							   )
						{
							new_cavemap[x][y]=BD_SPACE;
						}
						else if(
								(new_cavemap[x][y+1] == BD_SLIME)&&
								(new_cavemap[x][y] == BD_DIAMOND)&&
								(new_cavemap[x][y+2] == BD_SPACE)
							   )
						{
							NextRandom(&bd_game->SlimeSeed1,&bd_game->SlimeSeed2);
							if(bd_game->SlimeSeed1 < 10)
							{
								new_cavemap[x][y+2]=BD_DIAMONDfall;
								new_cavemap[x][y]=BD_SPACE;
							}
						}
						else if(
								(new_cavemap[x][y+1] == BD_SLIME)&&
								(new_cavemap[x][y] == BD_BOULDER)&&
								(new_cavemap[x][y+2] == BD_SPACE)
							   )
						{
							NextRandom(&bd_game->SlimeSeed1,&bd_game->SlimeSeed2);
							if(bd_game->SlimeSeed1 < 10)
							{
								new_cavemap[x][y+2]=BD_BOULDERfall;
								new_cavemap[x][y]=BD_SPACE;
							}
						}
					}
					break;
				case BD_MAGICWALL:
					if(bd_game->MagicWallActive == 1)
					{
						new_cavemap[x][y]=BD_MAGICWALLactive;
					}
					break;
				case BD_FIREFLYr:
				case BD_FIREFLYl:
				case BD_FIREFLYt:
				case BD_FIREFLYd:
					if(move_tick ==0)
					{
						int dir = firefly_left(curr_type);

						for(int di = 0; di<4; di++)
						{
							if(
									(new_cavemap[x+move_x(di)][y+move_y(di)] == BD_ROCKFORD)||
									(new_cavemap[x+move_x(di)][y+move_y(di)] == BD_ROCKFORDgrab)
							  )
							{
								explode(new_cavemap,x,y);
								bd_game->Lost=1;
							}
							if(new_cavemap[x+move_x(di)][y+move_y(di)] == BD_AMOEBA)
							{
								explode(new_cavemap,x,y);
							}
						}

						if(new_cavemap[x][y] != BD_EXPLOSION1)
						{
							if(new_cavemap[x+firefly_x(dir)][y+firefly_y(dir)] == BD_SPACE)
							{
								new_cavemap[x+firefly_x(dir)][y+firefly_y(dir)]=dir;
								new_cavemap[x][y]=BD_SPACE;
							}
							else if( new_cavemap[x+firefly_x(curr_type)][y+firefly_y(curr_type)] == BD_SPACE)
							{
								new_cavemap[x+firefly_x(curr_type)][y+firefly_y(curr_type)]=curr_type;
								new_cavemap[x][y]=BD_SPACE;
							}
							else
							{
								new_cavemap[x][y]=firefly_right(curr_type);
							}
						}
					}
					break;
					//there must still be a bug, oberserved a resting diamond on top a freestanding boulder
			}
		}
	}


	bd_game->AmoebaSpace=amoeba_possible;




	for(int y = 1; y < CAVE_HEIGHT-1; y++) 
	{
		for(int x = 0; x < CAVE_WIDTH; x++) 
		{
			bd_game->cavemap[x][y] = new_cavemap[x][y];
		}
	}


}



static unsigned int font5x3[] = {32319,17393,24253,32437,31879,30391,29343,31905,32447,31911};

void render_digit_5x3(int x,int y, int digit,int typea, int typeb,char display[CAVE_WIDTH][(INFO_HEIGHT+CAVE_HEIGHT)])
{
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<5;j++)
		{
			if(digit == -1)
				display[x+i][y+j] =typeb;
			else
			{
				if(font5x3[digit] & (1<<(i*5+j)))
				{
					display[x+i][y+j] =typea;
				}
				else
				{
					display[x+i][y+j] =typeb;
				}

			}
		}
	}
}

void render_digits_5x3(int x, int y, const char *text, int typea, int typeb,char display[CAVE_WIDTH][(INFO_HEIGHT+CAVE_HEIGHT)])
{
	while (*text)
	{
		render_digit_5x3(x,y,(*text)-48,typea,typeb,display);
		x+=4;
		text++;
	}

}

static void render_num(int number,int x,int y,int length,int pad, int typea, int typeb,char display[CAVE_WIDTH][(INFO_HEIGHT+CAVE_HEIGHT)])
{

	char s[10];
	snprintf(s,10, "%i", (int)number);

	int len = strlen(s);

	if (length < len) {
		int i;
		for (i = 0; i < length; i++) {
			render_digit_5x3(x, y, -1, typea,typeb,display);
			x+=4;
		}
		return;
	}
	int i;
	for (i = 0; i < length - len; i++) {
		render_digit_5x3(x, y, pad, typea,typeb,display);
		x+=4;
	}
	
	render_digits_5x3(x, y, (char*)s, typea,typeb,display);

}


int rendertick;

void bd_game_render(struct bd_game_struct_t* bd_game,unsigned int* pixelbuffer,int zoom)
{
	rendertick++;

	if(bd_game->Tick < BD_UNCOVER_LOOP)
	{
		for(int line=0;line < CAVE_HEIGHT;line++)
		{
			int pos = rand()%CAVE_WIDTH;
			bd_game->covered[pos][line] = 0;
			pos = rand()%CAVE_WIDTH;
			bd_game->covered[pos][line] = 0;
			pos = rand()%CAVE_WIDTH;
			bd_game->covered[pos][line] = 0;
			pos = rand()%CAVE_WIDTH;
			bd_game->covered[pos][line] = 0;
		}
	}
	char display[CAVE_WIDTH][(INFO_HEIGHT+CAVE_HEIGHT)];
	memset(display,0,(INFO_HEIGHT+CAVE_HEIGHT) * CAVE_WIDTH);
	
	//cave part:

	for(int y = 0; y < CAVE_HEIGHT; y++) 
	{
		for(int x = 0; x < CAVE_WIDTH; x++) 
		{
			int field = bd_game->cavemap[x][y];

			if(bd_game->Tick < BD_START_DELAY)
				if(field == BD_INBOX)
					if(bd_game->Tick % 20 < 10)
						field = BD_STEELWALL;


			if(bd_game->Tick < BD_UNCOVER_LOOP)
			{
				if(bd_game->covered[x][y])
				{
					field=BD_STEELWALL;
				}
			}

			display[x][y]=field;
		}
	}

	//info part:

	for(int y = CAVE_HEIGHT; y < (CAVE_HEIGHT+INFO_HEIGHT); y++) 
	{
		for(int x = 0; x < CAVE_WIDTH; x++) 
		{
			display[x][y]=BD_STEELWALL;
		}
	}

	if((bd_game->DiamondsRequired - bd_game->DiamondsCollected) > 0)
		render_num(bd_game->DiamondsRequired - bd_game->DiamondsCollected,1,22,3,0,BD_COLOR_PURPLE,BD_BOULDER,display);

	if((bd_game->CaveTime - (bd_game->Tick / 8)) > 0)
		render_num(bd_game->CaveTime - (bd_game->Tick / 8),28,22,3,0,BD_MAGICWALL,BD_BOULDER,display);
	


	// render to pixelbuffer
		for(int y = 0; y < (INFO_HEIGHT+CAVE_HEIGHT); y++) 
		{
			for(int x = 0; x < CAVE_WIDTH; x++) 
			{
				int colors[3];	
				get_colors(display[x][y],rendertick,colors);

				unsigned int col = (colors[0]<<16)+(colors[1]<<8)+colors[2];

				if(pixelbuffer[((y*zoom)*CAVE_WIDTH*zoom)+x*zoom] != col)
					for(int a = 0; a < zoom;a++)
					{
						for(int b = 0;b < zoom;b++)
						{
							pixelbuffer[((y*zoom+a)*CAVE_WIDTH*zoom)+x*zoom+b] = col;
						}
					}
			}
		}
}

