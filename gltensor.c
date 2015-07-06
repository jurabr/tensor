/*
 * File name: gltensor.c
 * Date:      2015/07/06 13:53
 * Author:    Jiri Brozovsky
 
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
#include <GL/glut.h>

/* GLUT/view: */
float newX=0, newY=0, newZ=0;               /* soucasna pozice, ze ktere se pocitaji rotace a posuny */
float newmX=0, newmY=0; 
int   oldX=0, oldY=0;                       /* soucasna pozice, ze ktere se pocitaji rotace a posuny */
int   rotX=0, rotY=0;                       /* soucasna pozice, ze ktere se pocitaji rotace a posuny */
float oldZ=0;                               /* minula pozice, ze ktere se pocitaji posuny */
float totZ=500;
int   mouseX=0, mouseY=0, mouseZ=0;         /* body, ve kterych se nachazi kurzor mysi */
int   mouseStatus=0;                        /* stav tlacitek mysi */


/** Plots 2D grid of points */
void plot_structure()
{
  /* TODO */
}

void plot_stuff(void)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); /* vymazani barvoveho bufferu */
    glMatrixMode(GL_MODELVIEW);             /* bude se menit modelova matice */
    glLoadIdentity();                       /* nahrat jednotkovou matici */

    glTranslatef(newmX/180, -newmY/180, 0);        /* posun sipky */
    glTranslatef(0.0f, 0.0f, -2.0f);       /* posun modelu dale od kamery */
    glTranslatef(0.0f, 0.0f, (newZ-oldZ)/totZ*50.0);

#if 0
    plot_structure() ;
#endif
    glBegin(GL_TRIANGLES);
      glColor3f(0.6, 0.6, 0.0);                 

      glVertex3f(-0.5,-0.5,0.0);
      glVertex3f(0.5,0.0,0.0);
      glVertex3f(0.0,0.5,0.0);
    glEnd();

    glFlush(); 
    glutSwapBuffers(); 
}


void onDisplay(void)
{
  plot_stuff();
}

void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);       /* barva pro mazani color-bufferu */
    glShadeModel(GL_SMOOTH);                /* nastaveni Gouraudova stinovani */
    glClearDepth(1.0f);                     /* barva pro mazani Z-bufferu */
    glEnable(GL_DEPTH_TEST);                /* nastaveni funkce pro testovani hodnot v Z-bufferu */
    glDepthFunc(GL_LESS);                   /* nastaveni porovnavaci funkce pro Z-buffer */
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); /* stav vykresleni vyplnenych polygonu */
    glPointSize(3.0);                       /* velikost vykreslovanych bodu */
    glLineWidth(1.0);                       /* sirka vykreslovanych car */
    glEnable(GL_POINT_SMOOTH);              /* povoleni antialiasingu bodu */
    glEnable(GL_LINE_SMOOTH);               /* povoleni antialiasingu car */
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0);
}

void onResize(int w, int h)
{
    totZ = (float)h ;
    glViewport(0, 0, w, h);                 /* viditelna oblast */
    glMatrixMode(GL_PROJECTION);            /* zmena projekcni matice */
    glLoadIdentity();                       /* nastaveni perspektivni projekce */
    gluPerspective(45.0, (double)w/(double)h, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);             /* zmena modelove matice */
}

void resetPos(void)
{
  newX=0;
  newY=0;
  newZ=0;       

  newmX=0;
  newmY=0;
  
  oldX=0;
  oldY=0;
  oldZ=0;

  rotX = 0 ;
  rotY = 0 ;
  
  mouseX=0;
  mouseY=0;
  mouseZ=0;

  mouseStatus=0;

  glutPostRedisplay();
}


void onKeyboard(unsigned char key, int x, int y)
{
    key=(key>'A' && key<='Z') ? key+'a'-'A':key;
    
    switch (key) {
        case 27:                            /* klavesa Escape */
        case 'q':                           
            exit(0);                        
            break;

        case 'r':
            resetPos();
            break;
				case 'p':
            /*swith_ecolres();*/
            glutPostRedisplay();
            break;

        default:
            break;
    }
}

void onMouseDrag(int x, int y)
{
    if (mouseStatus==1) {                   /* provadi se presun */
        newZ=oldZ+y-mouseZ;                 /* vypocitat novou pozici */
        glutPostRedisplay();                /* a prekreslit scenu */
    }

    if (mouseStatus==3) {                   /* provadi se presun */
        newmX = (x-oldX) + rotX ;            /* zapamatovat si pozici kurzoru */
        newmY = (y-oldY) + rotY ;            /* pro otaceni */
        glutPostRedisplay();
    }
}

void onMouseClick(int button, int state, int x, int y)
{
    if (button==GLUT_MIDDLE_BUTTON) {        /* prave tlacitko aktivuje posun od pozorovatele */
        if (state==GLUT_DOWN) {             /* pri stlaceni */
            mouseStatus=1;                  /* nastaveni pro funkci motion */
            mouseZ=y;                       /* zapamatovat pozici kurzoru mysi */
        }
        else {
            mouseStatus=0;
            oldZ=newZ;                      /* zapamatovat novy pocatek */
        }
    }

    if (button==GLUT_LEFT_BUTTON) {        /* prave tlacitko aktivuje posun od pozorovatele */
        if (state==GLUT_DOWN) {             /* pri stlaceni */
            mouseStatus=3;                  /* nastaveni pro funkci motion */

          oldX=x;              
          oldY=y;             
          rotX = newmX;              
          rotY = newmY;             
        }
        else {
            mouseStatus=0;
        }
    }

    glutPostRedisplay();
}


int main(int argc, char *argv[])
{
  msgout = stderr ;

  if (argc > 1)
  {
    if ((fr=fopen(argv[1],"r"))==NULL)
    {
      fprintf(msgout,"Invalid file: %s!\n", argv[1]);
      return(-2);
    }

    if (read_data(fr)!=0)
    {
      fprintf(msgout,"Reading of data failed!\n");
      fclose(fr);
      return(-1);
    }

    fclose(fr);
  }
  else
  {
    fprintf(msgout,"Use: %s FILENAME\n",argv[0]);
    return(-1);
  }

  fprintf(msgout," Tensor scale visualizer (%s).\n",argv[0]);

  glutInit(&argc, argv);                  
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(500, 500);           
  glutInitWindowPosition(10, 10); 
  glutCreateWindow("Tensor Scale 2D (OpenGL renderer)");

  glutReshapeWindow(700, 500);            
  glutDisplayFunc(onDisplay);             
  glutReshapeFunc(onResize);              
  glutKeyboardFunc(onKeyboard);           
  glutMouseFunc(onMouseClick);            
  glutMotionFunc(onMouseDrag);            

  init(); 

  /* createMenu(); */

  glutMainLoop();                         
  return(0);
}

/* end of gltensor.c */
