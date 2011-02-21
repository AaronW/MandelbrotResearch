/**
 * Senior Research Project: Mandelbrot Dispatched
 * Programmer: Aaron Weinberger
 * Technical Advisor: Dr. Hastings
 * February 11, 2011
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dispatch/dispatch.h>
#include "main.h"                   // The #defines needed for this program to run.

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
int countDispatch[IMAGEWIDTH][IMAGEHEIGHT];                 // the libdispatch result array
int countStride[IMAGEWIDTH][IMAGEHEIGHT];                   // The libdispatch.h + striding array

// Parallel implementation using a work pool.    TODO Should probably rename this 'cal_Row()' or calRowPThread() or something equally informative
void *cal_pixel(void *threadarg) {          // Note: Used to have to be static, in case it breaks again.
    int y;                                  // The current row
    for(;;) {                               // Loop forever till thread exits itself below
        pthread_mutex_lock(&y_mutex);       // So only one thread picks a specific y-value
        y = y_pick;                         // Store locally which row we're working on currently
        ++y_pick;                           // Increment our column for the next thread.
        pthread_mutex_unlock(&y_mutex);     // Unlock the mutex so the other threads can access it now
        double c_im = MaxIm - y*Im_factor;  // TODO THIS LINE MIGHT BE CAUSING SLOW DOWN! Does it run too many times compared to single thread?
        if(y>=IMAGEHEIGHT) {
            pthread_exit(NULL);             // No more rows, exit the thread.
        }
        for(int x=0; x<IMAGEWIDTH; ++x) {           // Left to right across the current row
            double c_re = MinRe + x*Re_factor;
            double Z_re = c_re, Z_im = c_im;
            int n;                                  // Need declared outside of for() loop so we can store iteration count!
            for(n=0; n<MAXITER; ++n) {              // Count those iterations!
                double Z_re2 = Z_re*Z_re, Z_im2 = Z_im*Z_im;
                if(Z_re2 + Z_im2 > 4) {
                    break;                          // Escapes! NOT in the M-Set!
                }
                Z_im = 2*Z_re*Z_im + c_im;
                Z_re = Z_re2 - Z_im2 + c_re;
            }
            countPThread[x][y] = n;                 // Need to store the iteration counts since plotting is not thread safe!
        }
        
    }
}

// PThread implementation using a work pool.
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
}

// The mandelbrot implementation under libdispatch.h
void mandelDispatch() {
    dispatch_queue_t aQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_apply(IMAGEHEIGHT, aQueue, ^(size_t y) {    
        double c_im = MaxIm - y*Im_factor;  // TODO THIS LINE MIGHT BE CAUSING SLOW DOWN! Does it run too many times compared to single thread?
        if(y>=IMAGEHEIGHT) {
            printf("invalid row! (dispatch)\n");                  // TODO Better error handling here for libdispatch version
        }
        for(int x=0; x<IMAGEWIDTH; ++x) {           // Left to right across the current row
            double c_re = MinRe + x*Re_factor;
            double Z_re = c_re, Z_im = c_im;        //double Z_re = c_re, Z_im = my_data->c_im;
            int n;                                  // Need declared outside of for() loop so we can store iteration count!
            for(n=0; n<MAXITER; ++n) {              // Count those iterations!
                double Z_re2 = Z_re*Z_re, Z_im2 = Z_im*Z_im;
                if(Z_re2 + Z_im2 >= 4) {
                    break;                          // Escapes! NOT in the M-Set!
                }
                Z_im = 2*Z_re*Z_im + c_im;          //Z_im = 2*Z_re*Z_im + my_data->c_im;
                Z_re = Z_re2 - Z_im2 + c_re;
            }
            countDispatch[x][y] = n;                // Need to store the iteration counts since plotting is not thread safe!
        }
    });
}

// Using libdispatch.h with striding technique
void mandelDispatchStride() {
    int stride = 100;       // TODO Arbitrary number for now
    dispatch_queue_t aQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    // Run it with striding
    dispatch_apply(IMAGEHEIGHT / stride, aQueue, ^(size_t idx){
        size_t j = idx * stride;
        size_t j_stop = j + stride;
        do {
            double c_im = MaxIm - idx*Im_factor;  // TODO THIS LINE MIGHT BE CAUSING SLOW DOWN! Does it run too many times compared to single thread?
            if(idx>=IMAGEHEIGHT) {
                printf("invalid row! (stride)\n");                  // TODO Better error handling here for libdispatch version
            }
            for(int x=0; x<IMAGEWIDTH; ++x) {           // Left to right across the current row
                double c_re = MinRe + x*Re_factor;
                double Z_re = c_re, Z_im = c_im;        //double Z_re = c_re, Z_im = my_data->c_im;
                int n;                                  // Need declared outside of for() loop so we can store iteration count!
                for(n=0; n<MAXITER; ++n) {              // Count those iterations!
                    double Z_re2 = Z_re*Z_re, Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 >= 4) {
                        break;                          // Escapes! NOT in the M-Set!
                    }
                    Z_im = 2*Z_re*Z_im + c_im;          //Z_im = 2*Z_re*Z_im + my_data->c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }
                countDispatch[x][idx] = n;                // Need to store the iteration counts since plotting is not thread safe!
            }        
        }
        while (j < j_stop);
    });

    size_t i;
    for (i = IMAGEHEIGHT - (IMAGEHEIGHT % stride); i < IMAGEHEIGHT; i++)
        printf("%u\n", (unsigned int)i);
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
    for(int i=0; i<IMAGEWIDTH; i++) {           // Check the libdispatch.h resulting array against the sequential.
        for(int j=0; j<IMAGEHEIGHT; j++) {
            if(countSingle[i][j]!=countDispatch[i][j]) {
                printf("Single[%d][%d]=%d BUT dispatch[%d][%d]=%d\n",i,j,countSingle[i][j],i,j,countDispatch[i][j]);
                diffCount++;                    // Found a difference!
            }
        }
    }
    /*for(int i=0; i<IMAGEWIDTH; i++) {
        for(int j=0; j<IMAGEHEIGHT; j++) {
            if(countSingle[i][j]!=countStride[i][j]) {
                printf("Single[%d][%d]=%d BUT stride[%d][%d]=%d\n",i,j,countSingle[i][j],i,j,countStride[i][j]);
                diffCount++;
            }
        }
    }*/
    return diffCount;                           // The number of differences between the implementations.
}

// Time the different implementations
void timer() {
    time_t t0, t1;                  // Wall clock time
    clock_t c0, c1;                 // Clock cycle time
    // TIME THE SINGLE THREADED VERSION
    t0 = time(NULL);                //Initialize the timers
    c0 = clock();
    printf("SINGLE THREAD\n");
    printf("\tbegin (wall):     %ld\n", (long) t0);
    printf("\tbegin (cpu):      %d\n", (int) c0);
    mandelSingle();                 // Call the single threaded implementation
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
    // LIBDISPATCH
    t0 = time(NULL);                // Null out timers so we can reuse
    c0 = clock();
    printf("LIBDISPATCH Threads");
    printf("\tbegin (wall):     %ld\n", (long) t0);
    printf("\tbegin (cpu):      %d\n", (int) c0);
    mandelDispatch();               // Call the libdispatch.h implementation
    t1 = time(NULL);
    c1 = clock();
    printf ("\tend (wall):              %ld\n", (long) t1);
    printf ("\tend (CPU);               %d\n", (int) c1);
    printf ("\telapsed wall clock time: %ld\n", (long) (t1 - t0));
    printf ("\telapsed CPU time:        %f\n", (float) (c1 - c0)/CLOCKS_PER_SEC);
    // LIBDISPATCH + STRIDING
    t0 = time(NULL);                // Null out timers so we can reuse
    c0 = clock();
    printf("LIBDISPATCH+STRIDING Threads");
    printf("\tbegin (wall):     %ld\n", (long) t0);
    printf("\tbegin (cpu):      %d\n", (int) c0);
    // TODO Re-enable this!
    //mandelDispatchStride();               // Call the libdispatch.h + striding implementation
    t1 = time(NULL);
    c1 = clock();
    printf ("\tend (wall):              %ld\n", (long) t1);
    printf ("\tend (CPU);               %d\n", (int) c1);
    printf ("\telapsed wall clock time: %ld\n", (long) (t1 - t0));
    printf ("\telapsed CPU time:        %f\n", (float) (c1 - c0)/CLOCKS_PER_SEC);

}

void consoleUI() {
    
}

// Print to file instead of stdout
void color(FILE *fp, int red, int green, int blue)  {
	fputc((char)red,fp);
	fputc((char)green,fp);
	fputc((char)blue,fp);
}

// Creates a .ppm file from the given implementation's results.
// Based on work by Eric R. Weeks http://www.physics.emory.edu/~weeks/software/mand.html
void ppmArray(int imageArray[IMAGEWIDTH][IMAGEHEIGHT]) {
    FILE *fp;                                               // Pointer for the file we're creating or overwriting
    fp = fopen("/Volumes/Macintosh HD/Users/aaron/Documents/MandelGL/MandelGL/mandelbrot.ppm", "w"); // NEEDS FULL PATH!!! NOT EVEN ~ WORKS!!!
	
	// Header for PPM output
	fprintf(fp,"P6\n# CREATOR: Aaron Weinberger/MandelbrotPPM based on work by Eric R. Weeks\n");
	fprintf(fp,"%d %d\n255\n",IMAGEWIDTH,IMAGEHEIGHT);
    
    for(int i=0; i<IMAGEWIDTH; i++) {
        for(int j=0; j<IMAGEHEIGHT; j++) {
            int iterationCount = imageArray[j][i];  // TODO Why is this printing seemingly backwards?
            if(iterationCount<MAXITER)              // TODO Remove magic numbers, possibly with something like #define colorOffset
                color(fp,iterationCount+COLOROFFSET*2,iterationCount+COLOROFFSET/2,iterationCount+COLOROFFSET);     // Escapes!
			else
                color(fp,0,0,0);                    // Doesn't escape
        }
    }
    fflush(fp);                     // Always flush the buffer to ensure it's all written out.
    fclose(fp);                     // Explicitly close the file now.
}

// Main driver class
int main(int argc, char** argv) {
    y_pick = 0;                     // Shootout inspired, make sure the variable starts at 0! Fixes a bug I was having, important.
    timer();                        // Call the different implementations and time them
    
    int diffs = compareCounts();    // Store it so we don't call the function twice or more
    if(diffs == 0)                  // Ensure the implementations generate equivalent output
        printf("Arrays are equal!\n");
    else {
        double succRate = (1 - (double)diffs/(double)(IMAGEWIDTH*IMAGEHEIGHT))*100;     // Calculate how wrong the implementation is
        printf("NOT EQUAL %d differences out of %d total!\n%.2f%% success rate\n", diffs, IMAGEHEIGHT*IMAGEWIDTH,succRate);
    }
    
    int choice;                                             // Whether or not to display a graphical representation of the Mandelbrot set.
    printf("Would you like the GUI displayed? (0-N 1-Y): ");
    if(scanf("%d",&choice)==0) {
        printf("Bad input, NO(0) autoselected.\n");         // Some initial error checking
        choice=0;
    }
    if(choice == 0) {
        printf("No GUI displayed, exiting now.\n");
    } else if(choice==1) {
        printf("1-Sequential,2-POSIX,3-libdispatch.h,4-libdispatch.h+striding\n");
        scanf("%d",&guiMethod);                             // TODO Need error checking!
        if(guiMethod==1)
            ppmArray(countSingle);
        else if(guiMethod==2)
            ppmArray(countPThread);
        else if(guiMethod==3)
            ppmArray(countDispatch);
        else if(guiMethod==4)
            ppmArray(countStride);
        else
            printf("Invalid choice.\n");                    // TODO Better error handling.
    }
    return EXIT_SUCCESS;
}