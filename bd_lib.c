#include "bd_lib.h"
#include <stdlib.h>

extern int bd_cave_data[];
extern int bd_cave_start_idx[];

void NextRandom(int *RandSeed1, int *RandSeed2)
{
	short TempRand1;
	short TempRand2;
	short carry;
	short result;


	TempRand1 = (*RandSeed1 & 0x0001) * 0x0080;
	TempRand2 = (*RandSeed2 >> 1) & 0x007F;
	result = (*RandSeed2) + (*RandSeed2 & 0x0001) * 0x0080;
	carry = (result > 0x00FF);
	result = result & 0x00FF;
	result = result + carry + 0x13;
	carry = (result > 0x00FF);
	*RandSeed2 = result & 0x00FF;
	result = *RandSeed1 + carry + TempRand1;
	carry = (result > 0x00FF);
	result = result & 0x00FF;
	result = result + carry + TempRand2;
	*RandSeed1 = result & 0x00FF;
}
			
void bd_draw_point(int x,int y,int item,int field[CAVE_WIDTH][CAVE_HEIGHT])
{
	field[x][y]=item;
}
void bd_draw_fillrect(int x1,int y1,int x2, int y2,int item,int field[CAVE_WIDTH][CAVE_HEIGHT])
{
	for(int x=x1;x<=(x2);x++)
	{
		for(int y=y1;y<=(y2);y++)
		{
			field[x][y]=item;
		}
	}
}
void bd_draw_rect(int x1,int y1,int x2, int y2,int item,int field[CAVE_WIDTH][CAVE_HEIGHT])
{
	for(int x=x1;x<=x2;x++)
	{
		for(int y=y1;y<=y2;y++)
		{
			if((x==x1)||(x==x2)||(y==y1)||(y==y2))
				field[x][y]=item;
		}
	}
}
void bd_draw_fillrect2(int x1,int y1,int x2, int y2,int item,int item2,int field[CAVE_WIDTH][CAVE_HEIGHT])
{
	for(int x=x1;x<=x2;x++)
	{
		for(int y=y1;y<=y2;y++)
		{
			if((x==x1)||(x==x2)||(y==y1)||(y==y2))
				field[x][y]=item;
			else
				field[x][y]=item2;
		}
	}
}
void bd_draw_raster(int x1,int y1,int count_x, int count_y,int offset_x,int offset_y,int item,int field[CAVE_WIDTH][CAVE_HEIGHT])
{
	for(int x=0;x<count_x;x++)
	{
		for(int y=0;y<count_y;y++)
		{
			field[x1+(x*offset_x)][y1+(y*offset_y)]=item;
		}
	}
}

int render_field(int cave,int level,int field[CAVE_WIDTH][CAVE_HEIGHT])
{
	struct cave_struct_t *cavedata;

	cavedata = (struct cave_struct_t*)&bd_cave_data[bd_cave_start_idx[cave]]; 

	int RandSeed1 = 0;
	int RandSeed2 = cavedata->RandSeed[level];

	for(int x = 0; x < CAVE_WIDTH; x++) {
		for (int y = 0; y < CAVE_HEIGHT; y++) {
			field[x][y]=BD_STEELWALL;
		}
	}

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
				field[x][y]=store;
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
				bd_draw_fillrect(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),field);
				offset+=6;
				break;
			case BD_DRAW_FILLRECT2:
				bd_draw_fillrect2(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),*(drawidx+offset+6),field);
				offset+=7;
				break;
			case BD_DRAW_RECTANGLE:
				bd_draw_rect(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),field);
				offset+=6;
				break;
			case BD_DRAW_POINT:
				bd_draw_point(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),field);
				offset+=4;
				break;
			case BD_DRAW_RASTER:
				bd_draw_raster(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),*(drawidx+offset+6),*(drawidx+offset+7),field);
				offset+=8;
				break;
		}

	}
	return offset;
}

int firefly_left(int firefly)
{
	if(firefly == BD_FIREFLYr) return BD_FIREFLYt;
	if(firefly == BD_FIREFLYt) return BD_FIREFLYl;
	if(firefly == BD_FIREFLYl) return BD_FIREFLYd;
	return BD_FIREFLYr;
}
int firefly_right(int firefly)
{
	if(firefly == BD_FIREFLYr) return BD_FIREFLYd;
	if(firefly == BD_FIREFLYd) return BD_FIREFLYl;
	if(firefly == BD_FIREFLYl) return BD_FIREFLYt;
	return BD_FIREFLYr;
}
int firefly_x(int firefly)
{
	if(firefly == BD_FIREFLYr) return 1;
	if(firefly == BD_FIREFLYd) return 0;
	if(firefly == BD_FIREFLYl) return -1;
	return 0;
}
int firefly_y(int firefly)
{
	if(firefly == BD_FIREFLYr) return 0;
	if(firefly == BD_FIREFLYd) return 1;
	if(firefly == BD_FIREFLYl) return 0;
	return -1;
}

int move_x(int direction)
{
	if(direction == 1) return 0;
	if(direction == 2) return 1;
	if(direction == 3) return 0;
	if(direction == 4) return -1;
	return 0;
}
int move_y(int direction)
{
	if(direction == 1) return -1;
	if(direction == 2) return 0;
	if(direction == 3) return 1;
	if(direction == 4) return 0;
	return 0;
}

static const int sinlut64[] = { 16,18,19,21,22,24,25,26,27,28,29,30,31,31,32,32,32,32,32,31,31,30,29,28,27,26,25,24,22,21,19,18,16,14,13,11,10,8,7,6,5,4,3,2,1,1,0,0,0,0,0,1,1,2,3,4,5,6,7,8,10,11,13,14,16};
static const int sinlut32[] = { 5,6,7,8,9,9,10,10,10,10,10,9,9,8,7,6,5,4,3,2,1,1,0,0,0,0,0,1,1,2,3,4,5};

void get_colors(int type,int tick,int* colors)
{
	colors[0]=0;
	colors[1]=0;
	colors[2]=0;

	switch(type)
	{
		case BD_SLIME:
			tick = (rand()&7);
			colors[0]=6;
			colors[1]=6;
			colors[2]=150+(5*sinlut32[tick&63]);
			break;
		case BD_AMOEBA:
			tick = (rand()&7);
			colors[0]=6;
			colors[1]=150+(5*sinlut32[tick&63]);
			colors[2]=6;
			break;
		case BD_DIRT:
			colors[0]=94;
			colors[1]=68;
			colors[2]=36;
			break;
		case BD_BOULDER:
		case BD_BOULDERfall:
			colors[0]=17;
			colors[1]=99;
			colors[2]=90;
			break;
		case BD_OUTBOXactive:
			colors[0]=93+(2*sinlut64[tick&63]);
			colors[1]=93+(2*sinlut64[tick&63]);
			colors[2]=93+(2*sinlut64[tick&63]);
			break;
		case BD_OUTBOX:
		case BD_STEELWALL:
			colors[0]=93;
			colors[1]=93;
			colors[2]=93;
			break;
		case BD_WALL:
			colors[0]=126;
			colors[1]=126;
			colors[2]=126;
			break;
		case BD_MAGICWALL:
			colors[0]=146;
			colors[1]=146;
			colors[2]=146;
			break;
		case BD_MAGICWALLactive:
			colors[0]=100+(4*sinlut64[tick&63]);
			colors[1]=100+(4*sinlut64[tick&63]);
			colors[2]=100+(4*sinlut64[tick&63]);
			break;
		case BD_DIAMOND:
		case BD_DIAMONDfall:
			colors[0]=86+(2*sinlut64[tick&63]);
			colors[1]=6;
			colors[2]=86+(2*sinlut64[(tick+5)&63]);
			break;
		case BD_COLOR_PURPLE:
			colors[0]=86+10;
			colors[1]=6;
			colors[2]=86+10;
			break;
		case BD_ROCKFORD:
		case BD_ROCKFORDgrab:
			colors[0]=158;
			colors[1]=90+(5*sinlut64[tick&63]);
			colors[2]=53;
			break;
		case BD_FIREFLYr:
		case BD_FIREFLYl:
		case BD_FIREFLYt:
		case BD_FIREFLYd:
			colors[0]=32+(20*sinlut32[tick&31]);
			colors[1]=32+(20*sinlut32[tick&31]);
			colors[2]=32+(20*sinlut32[tick&31]);
			break;
		case BD_EXPLOSION1:
		case BD_EXPLOSION2:
		case BD_EXPLOSION3:
		case BD_EXPLOSION4:
		case BD_EXPLOSION5:
			colors[0]=255;
			colors[1]=((BD_EXPLOSION5-type)*50);
			colors[2]=0;
	}
}



