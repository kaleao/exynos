#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

// Compile with gcc -lrt -lm -o test_clock test_clock.c

#define CLOCK CLOCK_MONOTONIC

int main(int argc, char** argv) {
    char *p;
    double temp, elapsed;
    int j;
    struct timespec requestStart, requestEnd, req;
    unsigned int value = 1000;

    if (argc!=2)
    {
	printf("value\n");
	return 0;
    }
    value = strtol( argv[1],&p, 10);
    printf("value = %u\n", value);

    // Pseudo-sleep
  //  clock_gettime(CLOCK, &requestStart);
    while(1)
    {
    temp = 0;
    for(j=0; j < value; j++)
        temp += j;
//    clock_gettime(CLOCK, &requestEnd);
//    elapsed = ( requestEnd.tv_sec - requestStart.tv_sec ) / 1e-6 + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / 1e3;
//    printf("Elapsed: %lf us\n", elapsed);

    // Nanosleep
//    clock_gettime(CLOCK, &requestStart);
    req.tv_nsec = 1000;
    req.tv_sec = 0;
    clock_nanosleep(CLOCK, 0, &req, NULL);
	
    //usleep(1);

//    clock_gettime(CLOCK, &requestEnd);
//    elapsed = ( requestEnd.tv_sec - requestStart.tv_sec ) / 1e-6 + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / 1e3;
    }
    printf("Elapsed: %lf us\n", elapsed);
    return 0;
}
