//
//  mandel_graphics.c
//  MandelGL
//
//  Created by Aaron Weinberger on 2/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include <GLUT/glut.h>
#include "main.h"                   // Include my defines like MAXITER
#include "mandel_graphics.h"

// My fabulous putpixel() implementation from graphics.h that I wanted so badly.
void setpixel(int iterations,int x,int y) {
    unsigned uIterations = (unsigned)iterations;        // Trying to deal with unsigned needs of OpenGL on OSX
    if(iterations < MAXITER)
        glColor3ub(uIterations*2,uIterations/2,uIterations);    // Currently random math applied to create cool looking colors, keep toying to see what I come up with.
    //glColor3ub(iterations*2,iterations/2,iterations);    // Need to use "ub" function on OSX for integers!
    else
        glColor3f(0.0f,0.0f,0.0f);      // Never escapes! Inside the mandelbrot set
    
    glBegin(GL_POINTS);
    glVertex2i(x,y);                    // Plot the individual pixel
    glEnd();
    glFlush();                          // Always need to flush to write out
}

// Magical display method! Call all functions that need drawn in here.
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //mandelbrot();       // Draw the set!
    //mandelPthread();       // Draw it faster!
    if(guiMethod==1) {
        printArray();
    }
    if(guiMethod==9) {
        //printArray();
        mandelSingle();
    }
    glutSwapBuffers();  // Swap background buffer with foreground to display to user
}

/**
 *http://blog.onesadcookie.com/2007/12/xcodeglut-tutorial.html
 * for 2d plotting ease?
 */
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
}

void idle(void) {
    glutPostRedisplay();
}