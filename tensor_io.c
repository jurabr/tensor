/*
 * File name: tensor_io.c
 * Date:      2015/07/06 18:23
 * Author:    Jiri Brozovsky
 
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

 */

#include "tensor.h"

FILE  *msgout = NULL ;
FILE  *fr     = NULL ;
FILE  *fw     = NULL ;
FILE  *fwe    = NULL ;

double x_siz = 0.0 ;  /* width */
double y_siz = 0.0 ;  /* height */
long   x_div = 1 ;    /* x-division (number of elements */
long   y_div = 1 ;    /* y-division */
long   n_mat = 1 ;    /* number of material types */
long  *m_fld = NULL ; /* field with element data */
long  *m_big = NULL ; /* big field with element data */
double *xi = NULL ;
double *yi = NULL ;
double *ri = NULL ;

long   lines  = 4   ;
double radius = 0.0 ;
double step   = 0.0 ;

long    ts_len = 0 ;
long   *ts_mat = NULL ;
double *ts_a   = NULL ;
double *ts_b   = NULL ;
double *ts_f   = NULL ;
double ksi, h ;
double ksi2, h2 ;

/** Reads model description from file */
int read_data (FILE *fr)
{
  long i ;

  fscanf (fr, "%lf %lf %li %li %li %li %lf %lf", 
      &x_siz, &y_siz, &x_div, &y_div, &n_mat, &lines, &radius, &step) ;

  fprintf (msgout,
      "width = %e\nheight = %e\ndivision = %lix%li\nmaterials = %li\nlines = %li\nradius = %e\nstep=%e\n", 
      x_siz, y_siz, x_div, y_div, n_mat, lines, radius, step) ;

  if ((m_fld = malloc((x_div*y_div)*sizeof(long))) == NULL)
  {
    fprintf(msgout,"Out of memory!\n");
    return(-1) ;
  }

  for (i=0; i<(x_div*y_div); i++)
  {
    fscanf(fr, "%li", &m_fld[i]);
  }
  
  return(0);
}

/** Writes results for use in GUI frontend */
int write_data (FILE *fw)
{
  long i ;

  fprintf(fw, "%li\n", ts_len) ;

  for (i=0; i<ts_len; i++)
  {
    fprintf(fw, "%li %li %e %e %e\n", i, ts_mat[i], ts_a[i], ts_b[i], ts_f[i]);
  }
    fprintf(fw, "%e %e\n", h, ksi);
  return(0);
}

/** Exports data as a FE. batch file for the uFEM */
int export_data_ufem(FILE *fw)
{
  long i, j ;
  long n_num = 0 ;
  long e_num = 0 ;

  fprintf(fw, "# len=%li, x=%e, y=%e\n", ts_len, x_siz, y_siz) ;

  for (j=0; j<=y_div; j++)
  {
    for (i=0; i<=x_div; i++)
    {
      n_num++ ;
      fprintf(fw,"n,%li,%e,%e,0\n",
              n_num,x_siz*((double)i), 
              (y_siz*((double)y_div-(double)j)) );
    }
  }

  for (j=0; j<y_div; j++)
  {
    for (i=0; i<x_div; i++)
    {
      fprintf(fw,"ep,%li,1,1,%li\n",e_num+1,ts_mat[e_num]+1);
      e_num++ ;

      fprintf(fw,"e,%li,%li,%li,%li,%li\n",
             e_num,
             (i+1) + (x_div+1)*(j+1), 
             (i+2) + (x_div+1)*(j+1),
             (i+2) + (x_div+1)*(j), 
             (i+1) + (x_div+1)*(j)
              );
    }
  }
  
  return(0);

}




/* end of tensor_io.c */
