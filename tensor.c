/*
   File name: tensor.c
   Date:      2005/09/17 11:57
   Author:    

   Copyright (C) 2005 Jiri Brozovsky

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


/* computes element position */
long  m_pos(long x, long y) 
{
  return(((y-1)*x_div) + x - 1 );
}

long  m_posbig(long x, long y) 
{
  return(((y-1)*(x_div*3)) + x - 1 );
}


int alloc_big_data (void)
{
  long i,j;
  double val ;
  
  if ((m_big = malloc((x_div*3*y_div*3)*sizeof(long))) == NULL)
  {
    fprintf(msgout,"Out of memory!\n");
    return(-1) ;
  }

  for (j=1; j<= y_div; j++)
  {
    for (i=1; i<=x_div; i++)
    {
      val = m_fld[m_pos(i,j)] ;

      m_big[m_posbig(i,j)] = val ;
      m_big[m_posbig(i+x_div,j)] = val ;
      m_big[m_posbig(i+2*x_div,j)] = val ;

      m_big[m_posbig(i,j+y_div)] = val ;
      m_big[m_posbig(i+x_div,j+y_div)] = val ;
      m_big[m_posbig(i+2*x_div,j+y_div)] = val ;

      m_big[m_posbig(i,j+2*y_div)] = val ;
      m_big[m_posbig(i+x_div,j+2*y_div)] = val ;
      m_big[m_posbig(i+2*x_div,j+2*y_div)] = val ;
    }
  }

#if 0
  fprintf(msgout,"\nbig data:\n");
  for (j=1; j<= y_div*3; j++)
  {
    for (i=1; i<=x_div*3; i++)
    {
#if 1
      fprintf(msgout," %li", m_big[m_posbig(i,j)]);
#else
      fprintf(msgout," %li", m_posbig(i,j));
#endif
    }
    fprintf(msgout,"\n");
  }
#endif
  
  return(0);
}

int alloc_line_coords(void)
{
  if ((xi = malloc((lines)*sizeof(double))) == NULL)
  {
    fprintf(msgout,"Out of memory!\n");
    return(-1) ;
  }
  if ((yi = malloc((lines)*sizeof(double))) == NULL)
  {
    fprintf(msgout,"Out of memory!\n");
    return(-1) ;
  }
  if ((ri = malloc((lines)*sizeof(double))) == NULL)
  {
    fprintf(msgout,"Out of memory!\n");
    return(-1) ;
  }
  return(0);
}

int alloc_results(void)
{
  ts_len = x_div * y_div ;

  if ((ts_mat = malloc((ts_len)*sizeof(long))) == NULL)
  {
    fprintf(msgout,"Out of memory!\n");
    return(-1) ;
  }

  if ((ts_a = malloc((ts_len)*sizeof(double))) == NULL)
  {
    fprintf(msgout,"Out of memory!\n");
    return(-1) ;
  }

  if ((ts_b = malloc((ts_len)*sizeof(double))) == NULL)
  {
    fprintf(msgout,"Out of memory!\n");
    return(-1) ;
  }

  if ((ts_f = malloc((ts_len)*sizeof(double))) == NULL)
  {
    fprintf(msgout,"Out of memory!\n");
    return(-1) ;
  }

  return(0);
}

long get_mat_type(double x, double y)
{
  long i ;
  double dx, dy ;
  long  mx = 0 ;
  long  my = 0  ;
  long  mat ;

  dx = x_siz / x_div ;
  dy = y_siz / y_div ;

  /* find row (y) */
  for (i=0; i<=(3*y_div); i++)
  {
    if ( ( y >=  (i*dy) )&&( y <= ((i+1)*dy) ) )
    {
      my = i ;
    }
  }

  /* find collum (x) */
  for (i=0; i<=(3*x_div); i++)
  {
    if ( ( x >=  (i*dx) )&&( x <= ((i+1)*dx) ) )
    {
      mx = i ;
    }
  }

  /* get material type: */
  mat = m_big[m_posbig(mx+1, my+1)] ;
  
#if 0
  fprintf(msgout, "material: %li\n", mat);
#endif

  return(mat);
}

int get_cyl_dir(double r, double angle, double x0, double y0, double *x, double *y) 
{
  *x = x0 + r*cos(angle) ;
  *y = y0 + r*sin(angle) ;
  return(0);
}

int compute_abf(double x, double y, long *mat, double *a, double *b, double *f)
{
  long i, j, substeps, type0, half ;
  double angle ;
  double x1, y1 ;
  double m11,m22,m12, m21, lambda_small, lambda, lambda1, lambda2 ;
  double rs1, rs2, q1, q2, n11, n12, n22, r1, r2 ;
  double x4   = 0.0 ;
	double y4   = 0.0 ;
	double x2   = 0.0 ;
	double y2   = 0.0 ;
	double x2y2 = 0.0 ;
	double a_down, a_top ;
	double b_down, b_top ;
	double tx, ty ;

  type0 = get_mat_type(x, y) ;
  *mat = type0;

  angle = (2*PI) / ((double) lines) ;
  substeps = (radius / step) + 1 ; 

#if 0
  fprintf (msgout,"angle = %e substeps = %li (%e|%e)\n", angle, substeps, radius, step);
#endif

  for  (i=0; i<lines; i++) 
  {
    for  (j=0; j<substeps; j++) 
    {
      get_cyl_dir(step*((double)j), ((double) i)*angle, x, y, &x1, &y1) ;

      xi[i] = x1 ;
      yi[i] = y1 ;
      ri[i] = step*((double)j) ;

      /*printf("[%e, %e] => [%e, %e]\n",step*((double)j), ((double)i)*angle, x1, y1);*/

      if (get_mat_type(x1, y1) != type0) 
      { 
        break ; 
      }
    }
  }

  half = (long) (lines/2) ;

  for  (i=0; i<half; i++) 
  {
    if (ri[i] >= ri[i+half])
    {
      ri[i] = ri[i+half] ;
      get_cyl_dir(ri[i], angle*(double)(i),x,y,&xi[i],&yi[i]) ;
    }
    else 
    {
      ri[i+half] = ri[i] ;
      get_cyl_dir(ri[i], angle*(double)(i+half),x,y,&xi[i+half],&yi[i+half]) ;
    }
  }

#if 0
  for  (i=0; i<lines; i++) { printf("%e %e\n",xi[i], yi[i]); }
#endif
#if 0
  for  (i=0; i<lines; i++) { printf("%li %e \n",i, ri[i]); }
#endif


  m11 = 0.0 ;
  m22 = 0.0 ;
  m12 = 0.0 ;
  m21 = 0.0 ;

  for (i=0; i<lines; i++) 
  {
    m11 += pow ( (xi[i] - x), 2 ) ;
    m22 += pow ( (yi[i] - y), 2 ) ;

    m12 +=  pow((xi[i] - x),2) * pow((yi[i] - y),2) ;
  }

  m11 = m11 / lines ;
  m22 = m22 / lines ;
  m12 = m12 / lines ;
  m21 = m12 ;

  q1 = (m11+m22) ;
	q2 = sqrt( pow(m11+m22, 2) + 4.0*(m12*m21-m11*m22) );

	lambda1 = 0.5 * (q1 + q2 ) ;
	lambda2 = 0.5 * (q1 - q2 ) ;


	/* getting larger one: */
	if (lambda2 > lambda1) 
	{ 
		lambda       = lambda2 ; 
		lambda_small = lambda1 ; 
	}
	else                   
	{ 
		lambda       = lambda1 ; 
		lambda_small = lambda2 ; 
	}

#if 0
	/* eigenvector for larger eigenvalue */
	n11 = m11 - lambda ;
	n22 = m22 - lambda ;
	n12 = m12 ;

	r1 = (n11*n22)/n12 - n12 ;
	r2 = (-1.0)* (n11/n12) ;

	/* eigenvector for smaller eigenvalue */
	n11 = m11 - lambda_small ;
	n22 = m22 - lambda_small ;
	n12 = m12 ;

	rs1 = 1 ;
	rs2 = 0 ;
	/*
	rs1 = (n11*n22)/n12 - n12 ;
	rs2 = (-1.0)* (n11/n12) ;
	*/
#else
		r1 = lambda -m22 ;
		r2 = m12 ;
		rs1 = 1 ;
		rs2 = 0 ;
		/*
		rs1 = lambda_small -m22 ;
		rs2 =  m12 ;
		*/

		/*
		rs1 = -m22 ;
		rs2 = m11 - lambda_small ;
		*/
#endif

	if ( ( sqrt(r1*r1 + r2*r2) * sqrt(rs1*rs1 + rs2*rs2) ) == 0.0 )
	{
		*f = 0.0 ;
	}
	else 
	{
		*f = acos (
		 	(r1*rs1 + r2*rs2) / ( sqrt(r1*r1 + r2*r2) * sqrt(rs1*rs1 + rs2*rs2) )
			) ;
	}


  /* ################################ */
  *a = 0.0 ;
	*b = 0.0 ;

	/* transform coordinates: */
	for (i=0; i<lines; i++) 
	{
		xi[i] -= x ;
		yi[i] -= y ;
	}

	for (i=0; i<lines; i++) 
	{
		tx = xi[i] ;
		ty = yi[i] ;
		
		xi[i] = tx * cos ((*f)) + ty * sin ((*f)) ;
		yi[i] = -tx * sin ((*f)) + ty * cos ((*f)) ;
	}

	for (i=0; i<lines; i++)
	{
		x2   += pow(xi[i],2) ;
		x4   += pow(xi[i],4) ;

		y2   += pow(yi[i],2) ;
		y4   += pow(yi[i],4) ;

		x2y2 += (pow(xi[i],2)*pow(yi[i],2)) ;
	}

	a_top  = (x2y2*x2y2) - (x4*y4) ;
	a_down = (x2y2*y2)   - (x2*y4) ;

	if ((a_down == 0.0))
	{
		fprintf(msgout,"[E] %s: a =%f / %f!\n",
				("Computation of tensor scale failed"),
			 	a_top, a_down);
    *a = 0 ;
	}
  else
  {
    if ((a_top/a_down) > 0)
    {
	    *a = sqrt ( (a_top / a_down) ) ;
    }
    else
    {
      *a = 0 ;
    }
  }

	b_top  = (x2y2*x2y2) - (x4*y4) ;
	b_down = (x2*x2y2)   - (x4*y2) ;

	if ((b_down == 0.0))
	{
		fprintf(msgout,"[E] %s: b =%f / %f!\n",
				("Computation of tensor scale failed"),
			 	b_top, b_down);
    *b = 0 ;
	}
  else
  {
    if ((b_top/b_down) > 0)
    {
	    *b = sqrt ( (b_top / b_down) ) ;
    }
    else
    {
      *b = 0 ;
    }
  }

  return(0);
}

/* computes sums of a, b, f divided by numbers */
int compute_g_abf(FILE *fw)
{
  double a0 = 0.0 ; 
  double a1 = 0.0 ; 
  double b0 = 0.0 ;
  double b1 = 0.0 ;
  double f0 = 0.0 ;
  double f1 = 0.0 ;
  double num_0 = 0 ;
  double num_1 = 0 ;
  double sum = 0 ;
  long i, ii, j ;

  for (ii=0; ii<y_div; ii++)
  {
    for (j=0; j<x_div; j++)
    {
      i = ii*x_div + j ; /* Excludes borders: */
      if ((ii==0)||(ii==(y_div-1))||(j==0)||(j==(x_div-1))) {continue;}
      if (ts_mat[i] == 0)
      {
        a0 += ts_a[i] ;
        b0 += ts_b[i] ;
        f0 += ts_f[i] ;
        num_0 += 1 ;
      }
      else /* mat 1 */
      {
        a1 += ts_a[i] ;
        b1 += ts_b[i] ;
        f1 += ts_f[i] ;
        num_1 += 1 ;
      }
    }
  }
  sum = num_0+num_1 ;

  fprintf(fw,"\nWeighted tensor scale:\n");
  fprintf(fw," a0 =%f, b0= %f, fi0= %f, ratio0 = %f\n",
      a0/num_0,b0/num_0,f0/num_0,num_0/sum);
  fprintf(fw," a1 =%f, b1= %f, fi1= %f, ratio1 = %f\n",
      a1/num_1,b1/num_1,f1/num_1,num_1/sum);

  return(0);
}

int comp_global(double *ksi, double *h, long mat)
{
  long i ;
  double ksi_i, h_i ;

  *h = 0 ;
  *ksi = 0 ;
  
  for (i=0; i<ts_len; i++)
  {
    if (ts_mat[i] == mat)
    {
      if ((ts_a[i] != 0)&&(ts_b[i] != 0))
      {
        ksi_i = sqrt(
          pow(ts_a[i],2) * pow(ts_b[i],2) /
          (
           pow(ts_b[i],2)*sin(ts_f[i]) + 
           pow(ts_a[i],2)*cos(ts_f[i])
           )
          ) ;
      }
      else
      {
        ksi_i = 0 ;
      }

      if (ts_a[i] != 0.0) { h_i = ksi_i / ts_a[i] ; }
      else { h_i = 0 ; }
      
      *ksi += ksi_i ;
      *h   += h_i ;
    }
  }

  *ksi /= (double)ts_len ;
  *h   /= (double)ts_len ;

  return(0);
}

int main (int argc, char *argv[]) 
{
  int i, j, pos ;
  double a,b,f, dx,dy ;
  long mat ;

  msgout = stderr ;

  fr = stdin ;

  if (argc > 1) 
  {
    if ((fr = fopen(argv[1],"r")) == NULL) { exit (-1) ; }
  }


  if (read_data (fr) != 0) { exit (-1); }

  fprintf(msgout,"\ninput data:\n");
  for (j=0; j<y_div; j++)
  {
    for (i=0; i<x_div; i++)
    {
#if 1
      fprintf(msgout," %li", m_fld[m_pos(i+1,j+1)]);
#else
      fprintf(msgout," %li", m_pos(i+1,j+1));
#endif
    }
    fprintf(msgout,"\n");
  }

  if (alloc_big_data ()  != 0) { exit (-1); }
  if (alloc_line_coords ()  != 0) { exit (-1); }
  if (alloc_results ()  != 0) { exit (-1); }

  dx = x_siz / ((double) x_div) ;
  dy = y_siz / ((double) y_div) ;

  pos = 0 ;
  for (j=0; j<y_div; j++)
  {
    for  (i=0; i<x_div; i++)
    {
      compute_abf(
          x_siz + 0.5*dx + ((double)i)*dx,
          y_siz + 0.5*dy + ((double)j)*dy,
          &ts_mat[pos], &ts_a[pos], &ts_b[pos], &ts_f[pos]);
      pos++ ;
    }
  }

  compute_abf(3*x_siz/2, 3*y_siz/2, &mat, &a, &b, &f);

  fprintf(msgout," A=%e B=%e Angle=%e\n",a,b,f);

  
  if (argc > 2) 
  {
    if ((fw = fopen(argv[2],"w")) == NULL) { fw = stdout ; }
    printf("[[%s]] (%li)\n", argv[2], ts_len);
  }
  else
  {
    fw = stdout ;
  }

  comp_global(&ksi, &h, 0);
  printf("ksi  = %e. h =%e\n", ksi, h);

  write_data(fw) ; 

  if (argc > 3) 
  {
    if ((fwe = fopen(argv[3],"w")) == NULL) { fwe = stdout ; }
    export_data_ufem(fwe);
    fclose(fwe);
  }

 printf("--------------------\n");
  compute_g_abf(stdout) ;

  fclose(fw);
  fclose(fr);

  return(0);
}

/* end of tensor.c */
