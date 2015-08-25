/*
   File name: gen.c
   Date:      2015/08/25 15:18
   Author:    Jiri Brozovsky

   Copyright (C) 2015 Jiri Brozovsky

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   in a file called COPYING along with this program; if not, write to
   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
   02139, USA.


	 Generates large input file for tensor.tcl/tensor.c with actual sizes
	 of bricks and mortar layers
*/

#include <stdio.h>
#include <stdlib.h>

int cihly[300][300];

#if 0
int cih_x = 29 ;
int cih_y = 7  ;
int malt  = 1  ;

int num_x = 10 ;
int num_y = 18 ;

int sizex =  0 ;
int sizey =  0 ;
#else
int cih_x = 29 ;
int cih_y = 7  ;
int malt  = 1  ;

int num_x = 2 ;
int num_y = 3 ;

int sizex =  0 ;
int sizey =  0 ;
#endif

double unit_size = 0.01 ;

void comp_ssizexy(void)
{
	sizex = (cih_x+malt)*num_x ;
	sizey = 2*(cih_y+malt)*num_y ;
}

/* data cleaning */
void prep_data(int sizex, int sizey)
{
	int i, j;

	for (i=0; i<sizex; i++)
	{
		for (j=0; j<sizey; j++)
		{
			cihly[i][j] = 0 ;
		}
	}
}

void set_mortar(int sizex, int sizey)
{
	int i, j, k ;

	/* horizontal layers */
	for (j=0; j<(2*num_y); j++)
	{
		for (i=0; i<sizex; i++) 
		{ 
			cihly[i][(cih_y+malt)*j] = 1 ; 
		}
	}

	/* odd vertical layers */
	for (i=0; i<num_x; i++)
	{
		for (j=0; j<num_y; j++) 
		{ 
			for (k=0; k<cih_y; k++) 
			{
				cihly[i*(cih_x+malt)][(j*(2*(cih_y+malt)))+k+1] = 1 ; 
			}
		}
	}

	/* even vertical layers */
	for (i=0; i<num_x; i++)
	{
		for (j=0; j<num_y; j++) 
		{ 
			for (k=0; k<cih_y; k++) 
			{
				cihly[(i*(cih_x+malt))+((int)((cih_x+malt)/2))][(j*2*(cih_y+malt))+k+(cih_y+malt)+1] = 1 ; 
			}
		}
	}
}

int write_data(FILE *fw, int sizex, int sizey)
{
	int i, j;

  fprintf (fw,"%e %e\n%i %i\n2\n12\n%e\n%e\n",
			(double)sizex*unit_size, (double)sizey*unit_size, 
			sizex, sizey,
      (double)(cih_x+1)*unit_size,unit_size/2);
	for (j=0; j<sizey; j++)
	{
		for (i=0; i<sizex; i++)
		{
			fprintf(fw,"%i ",cihly[i][j]);
		}
		fprintf(fw,"\n");
	}
  fprintf (fw,"0\n 0.0 0.0\n");
	return(0);
}

int main(int argc, char *argv[])
{
	comp_ssizexy() ;
	prep_data(sizex, sizey) ;
	set_mortar(sizex, sizey) ;
	write_data(stdout, sizex, sizey) ;
	return(0);
}


/* end of gen.c */
