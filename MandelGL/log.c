/**
 * Senior Research Project: Mandelbrot Dispatched
 * Programmer: Aaron Weinberger
 * Technical Advisor: Dr. Hastings
 * February 11, 2011
 */
#include "log.h"
#include "main.h"
#include <time.h>
#include <stdio.h>

// Driver function for this file, handles the repetitions and writing the header.
void logger() {
    FILE *fp;                                               // Pointer for the file we're creating or overwriting
    fp = fopen("/Volumes/Macintosh HD/Users/aaron/Documents/MandelGL/MandelGL/logfile.txt", "a"); // NEEDS FULL PATH!!! NOT EVEN ~ WORKS!!!
    printHeader(fp);
    for(int i=0; i<RUNCOUNT; i++)                           // Run the set of implementations RUNCOUNT times
        timer(fp);
    fprintf(fp,"***END***\n");                              // Footer so I know whether the machine finished all the runs or not.
    fflush(fp);                                             // Ensure everything has been written out
    fclose(fp);                                             // Explicitly close the file
}

// Time the different implementations, might make too many fflush() calls but I do not want to lose costly data!
void timer(FILE *fp) {
    time_t t0, t1;                  // Wall clock time
    clock_t c0, c1;                 // Clock cycle time
    // TIME THE SINGLE THREADED VERSION
    t0 = time(NULL);                //Initialize the timers
    c0 = clock();
    mandelSingle();                 // Call the single threaded implementation
    t1 = time(NULL);
    c1 = clock();
    fprintf(fp,"sequential_wall=%ld\n",(long)(t1-t0));
    fprintf(fp,"sequential_CPU=%f\n",(float)(c1-c0)/CLOCKS_PER_SEC);
    fflush(fp);
    // TIME THE PTHREADED IMPLEMENTATION
    t0 = time(NULL);                // Null out timers so we can reuse
    c0 = clock();
    mandelPthread();                // Call the POSIX-Threaded Implementation
    t1 = time(NULL);
    c1 = clock();
    fprintf(fp,"pthread_wall=%ld\n",(long)(t1-t0));
    fprintf(fp,"pthread_CPU=%f\n",(float)(c1-c0)/CLOCKS_PER_SEC);
    fflush(fp);
    // TIME LIBDISPATCH IMPLEMENTATION
    t0 = time(NULL);                // Null out timers so we can reuse
    c0 = clock();
    mandelDispatch();               // Call the libdispatch.h implementation
    t1 = time(NULL);
    c1 = clock();
    fprintf(fp,"dispatch_wall=%ld\n",(long)(t1-t0));
    fprintf(fp,"dispatch_CPU=%f\n",(float)(c1-c0)/CLOCKS_PER_SEC);
    fflush(fp);                     // Flush the buffer so this trial run's data is output to the file.
}

// Print the parse header for this manual run.
void printHeader(FILE *fp) {
    fprintf(fp,"MAXITER=%d NUM_THREADS=%d IMAGEWIDTH=%d IMAGEHEIGHT=%d\n",MAXITER,NUM_THREADS,IMAGEWIDTH,IMAGEHEIGHT);
    fprintf(fp,"cpu_scale=seconds RUNCOUNT=%d\n",RUNCOUNT);
    fprintf(fp,"***START***\n");
    fflush(fp);
}
