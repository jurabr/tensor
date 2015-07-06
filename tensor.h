/*
   File name: tensor.h
   Date:      2015/07/06 18:24
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
*/


#ifndef __TENSOR_H__
#define __TENSOR_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846

extern FILE  *msgout ;
extern FILE  *fr     ;
extern FILE  *fw     ;
extern FILE  *fwe    ;

extern double x_siz ;  /* width */
extern double y_siz ;  /* height */
extern long   x_div ;    /* x-division (number of elements */
extern long   y_div ;    /* y-division */
extern long   n_mat ;    /* number of material types */
extern long  *m_fld ; /* field with element data */
extern long  *m_big ; /* big field with element data */
extern double *xi ;
extern double *yi ;
extern double *ri ;

extern long   lines  ;
extern double radius ;
extern double step   ;

extern long    ts_len ;
extern long   *ts_mat ;
extern double *ts_a   ;
extern double *ts_b   ;
extern double *ts_f   ;
extern double ksi, h ;
extern double ksi2, h2 ;


extern int read_data (FILE *fr);
extern int write_data (FILE *fw);
extern int export_data_ufem(FILE *fw);

#endif

/* end of tensor.h */
