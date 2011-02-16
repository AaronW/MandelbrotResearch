/**
 * Senior Research Project: Mandelbrot Dispatched
 * Programmer: Aaron Weinberger
 * Technical Advisor: Dr. Hastings
 * February 11, 2011
 */
#include <stdlib.h>
#include <stdio.h>
#include <GLUT/glut.h>
#include <pthread.h>
#include "mandel_graphics.h"        // OpenGL Graphic functions I wrote
#include "main.h"                   // The #defines needed for this program to run.

int DEMO = FALSE;                   // Quick demo mode to plot the OpenGL Singlethread mandelbrot
const double MinRe = -2.0;          // Setup the necessary variables, should I take time to remove globals and pass them around?
const double MaxRe = 1.0;           // CONSTANTS, except for DEMO above.
const double MinIm = -1.2;
const double MaxIm = MinIm+(MaxRe-MinRe)*IMAGEHEIGHT/IMAGEWIDTH;
const double Re_factor = (MaxRe-MinRe)/(IMAGEWIDTH-1);
const double Im_factor = (MaxIm-MinIm)/(IMAGEHEIGHT-1);

pthread_mutex_t y_mutex = PTHREAD_MUTEX_INITIALIZER;        // Inspired by shootout code
int y_pick;                                                 // Inspired by shootout code
// TODO IS THE MUTEX ABOVE LOCKING MY ARRAYS TOO!?
int countPThread[IMAGEWIDTH][IMAGEHEIGHT];                  // The iteration counts, countPThread[x][y]
int countSingle[IMAGEWIDTH][IMAGEHEIGHT];                   // Store iteration counts of single threaded implementation

// Parallel implementation
// TODO Should probably rename this "cal_Row()" or calRowPThread() or something equally informative...
void *cal_pixel(void *threadarg) {          // Note: Used to have to be static, in case it breaks again.
    int y;                                  // The current row
    //double c_im = MaxIm - y*Im_factor;

    for(;;) {                               // Loop forever till thread exits itself below
        pthread_mutex_lock(&y_mutex);       // So only one thread picks a specific y-value
        y = y_pick;                         // Store locally which row we're working on currently
        //y_pick++;                           // Increment the global y_pick mutex so the next thread works on next row
        ++y_pick;       // Was this backwards?
        pthread_mutex_unlock(&y_mutex);     // Unlock the mutex so the other threads can access it now
        double c_im = MaxIm - y*Im_factor;  // TODO THIS LINE MIGHT BE CAUSING SLOW DOWN! Does it run too many times compared to single thread?
        if(y>=IMAGEHEIGHT) {
            pthread_exit(NULL);                     // return NULL;
        }
        for(int x=0; x<IMAGEWIDTH; ++x) {           // Left to right across the current row
            double c_re = MinRe + x*Re_factor;
            double Z_re = c_re, Z_im = c_im;        //double Z_re = c_re, Z_im = my_data->c_im;
            int n;                         // Need declared outside of for() loop so we can store iteration count!
            for(n=0; n<MAXITER; ++n) {              // Count those iterations!
                double Z_re2 = Z_re*Z_re, Z_im2 = Z_im*Z_im;
                if(Z_re2 + Z_im2 > 4) {
                    break;                          // Escapes! NOT in the M-Set!
                }
                Z_im = 2*Z_re*Z_im + c_im;          //Z_im = 2*Z_re*Z_im + my_data->c_im;
                Z_re = Z_re2 - Z_im2 + c_re;
            }
            //setpixel(n,x,y);             // Plot the point (n is the number of iterations)
            countPThread[x][y] = n;                 // Need to store the iteration counts since plotting is not thread safe!
        }
        
    }
}

// PThread implementation
void mandelPthread() {
    int rc, t;                                      // For pthreads
    pthread_t threads[NUM_THREADS];                 // The pthreads array
    
    for(t=0; t<NUM_THREADS; t++) {
        rc = pthread_create(&threads[t], NULL, cal_pixel, (void *) t);
        if (rc) {                                   // Basic error checking.
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    for(int k=0; k<NUM_THREADS; k++) {
        pthread_join(threads[k], NULL);     // Wait for them all to finish and join()!
    }
    pthread_mutex_destroy(&y_mutex);        // Mutex no longer needed, destroy it.
    
    // TODO Print out the whole image now
}

// Function taken from below address and modified by myself slightly to work with my code.
// http://warp.povusers.org/Mandelbrot/
void mandelSingle() {
    for(int y=0; y<IMAGEHEIGHT; ++y) {         // Progress through the image, row by row.
        double c_im = MaxIm - y*Im_factor;
        for(int x=0; x<IMAGEWIDTH; ++x) {      // Left to right across the current row
            double c_re = MinRe + x*Re_factor;
            double Z_re = c_re, Z_im = c_im;
            int n;                         // was unsigned c99 for-loop declaration in original
            for(n=0; n<MAXITER; ++n) {              // Count those iterations!
                double Z_re2 = Z_re*Z_re, Z_im2 = Z_im*Z_im;
                if(Z_re2 + Z_im2 > 4) {
                    break;                          // Escapes! NOT in the M-Set!
                }
                Z_im = 2*Z_re*Z_im + c_im;
                Z_re = Z_re2 - Z_im2 + c_re;
            }
            if(DEMO==1)
                setpixel(n,x,y);                        // DEMO mode
            //setpixel(n,x,y);                        // Plot the point (n is the number of iterations)
            countSingle[x][y] = n;                      // Store the iteration counts
        }
    }
}

// Compare the resulting arrays to make sure implementations are equivalent
int compareCounts() {
    int diffCount = 0;
    for(int i=0; i<IMAGEWIDTH; i++) {           // Move through X-Axis
        for(int j=0; j<IMAGEHEIGHT; j++) {      // Move through Y-Axis
            if(countPThread[i][j] != countSingle[i][j]) {
                printf("Single[%d][%d]=%d BUT POSIX[%d][%d]=%d\n",i,j,countSingle[i][j],i,j,countPThread[i][j]);
                diffCount++;                    // Found a difference!
            }
        }
    }
    return diffCount;                           // The number of differences between the implementations.
}

// Print the given array from a particular Mandelbrot implementation.
void printArray() {
    if(guiMethod==1) {                          // SingleThreaded
        for(int i=0; i<IMAGEWIDTH; i++) {
            for(int j=0;j<IMAGEHEIGHT; j++) {
                setpixel(i,j,countSingle[i][j]);
            }
        }
    } else if(guiMethod==2) {                   // POSIX Threads
        for(int i=0; i<IMAGEWIDTH; i++) {
            for(int j=0;j<IMAGEHEIGHT; j++) {
                setpixel(i,j,countPThread[i][j]);
            }
        }
    } else {
        printf("Invalid in printArray()\n"); 
    }

}

// Time the different implementations
void timer() {
    time_t t0, t1;
    clock_t c0, c1;
    // TIME THE SINGLE THREADED VERSION
    t0 = time(NULL);                 //Initialize the timers
    c0 = clock();
    printf("SINGLE THREAD\n");
    printf("\tbegin (wall):     %ld\n", (long) t0);
    printf("\tbegin (cpu):      %d\n", (int) c0);
    mandelSingle();                  // Call the single threaded implementation
    t1 = time(NULL);
    c1 = clock();
    printf ("\tend (wall):              %ld\n", (long) t1);
    printf ("\tend (CPU);               %d\n", (int) c1);
    printf ("\telapsed wall clock time: %ld\n", (long) (t1 - t0));
    printf ("\telapsed CPU time:        %f\n", (float) (c1 - c0)/CLOCKS_PER_SEC);
    // TIME THE PTHREADED IMPLEMENTATION
    t0 = time(NULL);                // Null out timers so we can reuse
    c0 = clock();
    printf("POSIX Threads");
    printf("\tbegin (wall):     %ld\n", (long) t0);
    printf("\tbegin (cpu):      %d\n", (int) c0);
    mandelPthread();                // Call the POSIX-Threaded Implementation
    t1 = time(NULL);
    c1 = clock();
    printf ("\tend (wall):              %ld\n", (long) t1);
    printf ("\tend (CPU);               %d\n", (int) c1);
    printf ("\telapsed wall clock time: %ld\n", (long) (t1 - t0));
    printf ("\telapsed CPU time:        %f\n", (float) (c1 - c0)/CLOCKS_PER_SEC);
}

void consoleUI() {
    
}

// Main driver class
int main(int argc, char** argv) {
    y_pick = 0;                     // Shootout inspired, make sure the variable starts at 0!
    timer();                        // Call the different implementations and time them
    
    int diffs = compareCounts();
    if(diffs == 0)                  // Ensure the implementations generate equivalent output
        printf("Arrays are equal!\n");
    else {
        double succRate = (1 - (double)diffs/(double)(IMAGEWIDTH*IMAGEHEIGHT))*100;
        printf("NOT EQUAL %d differences out of %d total!\n%.2f%% success rate\n", diffs, IMAGEHEIGHT*IMAGEWIDTH,succRate);
    }
    
    int choice;                                             // Whether or not to display a GUI representation of the Mandelbrot set.
    printf("Would you like the GUI displayed? (0-N 1-Y 9-DEMO MODE): ");
    if(scanf("%d",&choice)==0) {
        printf("Bad input, NO(0) autoselected.\n");         // Some initial error checking
        choice=0;
    }
    if(choice == 0) {
        printf("No GUI displayed, exiting now.\n");
    } else if(choice==1) {
        printf("1 for Single Threaded, 2 for POSIX:\n");
        scanf("%d",&guiMethod);                             // TODO Need error checking!
        // GUI DISPLAY CODE
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
        glutInitWindowSize(IMAGEWIDTH, IMAGEHEIGHT);
        glutCreateWindow("Mandelbrot OpenGL");
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);
        glutIdleFunc(idle);
        glutMainLoop();
    }
    return EXIT_SUCCESS;
}