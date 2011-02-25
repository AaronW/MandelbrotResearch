//
//  Senior Research Project: Mandelbrot Dispatched
//  Programmer: Aaron Weinberger
//  Technical Advisor: Dr. Hastings
//  February 11, 2011
//
#define IMAGEHEIGHT 4000
#define IMAGEWIDTH 4000
#define COLOROFFSET 35          // The minimum color for points not in the set.
#define FALSE 0
#define TRUE 1
#define MAXITER 50000            // Maximum iteration count before point is determined to not escape.
#define NUM_THREADS 4           // Number of threads for my POSIX Threaded implementation.
#define RUNCOUNT 40             // Number of times to run each implementation for benchmarking purposes.
int guiMethod;                  // Method to output the graphical representation of the Mandelbrot set.
void mandelPthread();           // Necessary function prototypes
void mandelSingle();
void mandelDispatch();
void printArray();