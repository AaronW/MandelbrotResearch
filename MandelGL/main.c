/**
 * Senior Research Project: Mandelbrot Dispatched
 * Programmer: Aaron Weinberger
 * Technical Advisor: Dr. Hastings
 * February 11, 2011
 */
#include <stdlib.h>
#include <stdio.h>
#include <GLUT/glut.h>


#define ImageHeight 480
#define ImageWidth 640
#define false 0
#define true 1
#define MAXITER 256

// My fabulous putpixel() implementation from graphics.h that I wanted so badly.
void setpixel(int iterations,int x,int y) {
    if(iterations < MAXITER)
        glColor3ub(iterations*2,iterations/2,iterations);    // Need to use "ub" function on OSX for integers!
        // Currently random math applied to create cool looking colors, keep toying to see what I come up with.
    else
        glColor3f(0.0f,0.0f,0.0f);      // Never escapes! Inside the mandelbrot set
    
    glBegin(GL_POINTS);
    glVertex2i(x,y);                    // Plot the individual pixel
    glEnd();
    glFlush();                          // Always need to flush to write out
}

// Function taken from below address and modified by myself slightly to work with my code.
// http://warp.povusers.org/Mandelbrot/
void mandelbrot() {
    double MinRe = -2.0;
    double MaxRe = 1.0;
    double MinIm = -1.2;
    double MaxIm = MinIm+(MaxRe-MinRe)*ImageHeight/ImageWidth;
    double Re_factor = (MaxRe-MinRe)/(ImageWidth-1);
    double Im_factor = (MaxIm-MinIm)/(ImageHeight-1);
    
    for(unsigned y=0; y<ImageHeight; ++y) {         // Progress through the image, row by row.
        double c_im = MaxIm - y*Im_factor;
        for(unsigned x=0; x<ImageWidth; ++x) {      // Left to right across the current row
            double c_re = MinRe + x*Re_factor;
            double Z_re = c_re, Z_im = c_im;
            unsigned int n;                         // was unsigned c99 for-loop declaration in original
            for(n=0; n<MAXITER; ++n) {              // Count those iterations!
                double Z_re2 = Z_re*Z_re, Z_im2 = Z_im*Z_im;
                if(Z_re2 + Z_im2 > 4) {
                    break;                          // Escapes! NOT in the M-Set!
                }
                Z_im = 2*Z_re*Z_im + c_im;
                Z_re = Z_re2 - Z_im2 + c_re;
            }
            setpixel(n,x,y);                        // Plot the point (n is the number of iterations)
        }
    }
}

// 
// Magical display method! Call all functions that need drawn in here.
//
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mandelbrot();       // Draw the set!
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

// Main driver class
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Mandelbrot OpenGL");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMainLoop();
    return EXIT_SUCCESS;
}