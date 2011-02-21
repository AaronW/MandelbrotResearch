/**
 * Senior Research Project: Mandelbrot Dispatched
 * Programmer: Aaron Weinberger
 * Technical Advisor: Dr. Hastings
 * February 11, 2011
 */
#define IMAGEHEIGHT 1000
#define IMAGEWIDTH 1000
#define COLOROFFSET 25          // The minimum color for points not in the set.
#define FALSE 0
#define TRUE 1
#define MAXITER 5000            // Maximum iteration count before point is determined to not escape.
#define NUM_THREADS 4           // Number of threads for my POSIX Threaded implementation.
#define RUNCOUNT 10             // Number of times to run each implementation for benchmarking purposes.
int guiMethod;                  // Method to output the graphical representation of the Mandelbrot set.
// Necessary function prototypes
void mandelPthread();
void mandelSingle();
void printArray();