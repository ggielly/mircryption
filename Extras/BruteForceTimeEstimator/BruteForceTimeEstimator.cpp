// BruteForceTimeEstimator.cpp : Defines the entry point for the console application.
// version 2 - function had some bugs in estimate time

#include "stdafx.h"
#include <time.h>
#include <math.h>
#include <malloc.h>



int main(int argc, char* argv[])
{
	// estimate bruteforce time
	clock_t time_start;
	clock_t time_end;
	clock_t elapsed_clocks;
	long double elapsed_seconds;
	long double checkspersec,checksperday,checksperyear;
	long double dayestimate;
	long double secsperday=60*60*24;
	long double searchsize;
	//char *mem[50];

	printf("BruteForcer Time Estimator Utility v1.0\n");
	printf("This utility assumes that the checking of a potential key can be done\n");
	printf(" at the cpu cost of only a few opcodes. This is insanely optimistice\n");
	printf(" and most decryption routines will take many orders of magnitude longer\n");
	printf(" than this to evaluate a potential key, so the time esimtates given here\n");
	printf(" are very optimistic and you should expect to multiply estimates by\n");
	printf(" 10 to 1000 times for even the most efficient key check implementations.\n");
	printf("However, you must also account for faster computers, and parallel cpus\n");
	printf("  would likely be used on a bruteforce attack, which might suggest you\n");
	printf("  divide these numbers by as much as a million (for a major government.\n");
	printf("These are very rough estimates!  These times assume a truely random password,\n");
	printf(" so if your password is composed entirely of valid dictionary words, it\n");
	printf(" may be crackable in a matter of hours.\n");

	// get current time
	time_start=clock();

	// estimate how long to crack a pass - note we dont even waste any time actually trying passwords, we assume it can be done instantaneously
	printf("Please wait - estimating speed.");
	long double counter1=0;
	long double counter2=0;
	int mcounter=0;

	while (counter1<40)
	{
		// for test of memory leak detection - should not be left in on final version
		// mem[mcounter++]=(char *)malloc(10000);
		++counter1;
		counter2=0;
		printf(".");
		while (counter2<50000000)
		{
			++counter2;
		}
	}
	printf("\n");

	// get current time
	time_end=clock();

	// calculate elapsed time
	elapsed_clocks=time_end-time_start;
	// how many seconds have elapsed (The time in seconds is the value returned by the clock function, divided by CLOCKS_PER_SEC.)
	elapsed_seconds=elapsed_clocks/CLOCKS_PER_SEC;
	checkspersec=(counter1*counter2)/elapsed_seconds;
	checksperday=checkspersec*secsperday;
	checksperyear=checksperday*365;

	// display estimates
	printf("- Estimated Bruteforcing Time -----------------\n");
	printf("Checks per second: %.0Lf (%.0Lf / %0.0Lf)\n",(long double)checkspersec,(long double)(counter1*counter2),(long double)elapsed_seconds);
	//printf("Checks per day: %.0Lf\n",checksperday);
	searchsize=208827064576;
	dayestimate=searchsize/(checksperday)/2;
	printf(" 8 character pass using only a-z:  %.3Lf hours.\n",dayestimate*24);
	searchsize=6.45099747032972E19;
	dayestimate=searchsize/(checksperyear)/2;
	printf("14 character pass using only a-z:  %.0Lf years.\n",dayestimate);
	searchsize=1.99281488952094E28;
	dayestimate=searchsize/(checksperyear)/2;
	printf("20 character pass using only a-z:  %.0Lf years.\n",dayestimate);
	searchsize=7.04423425546998E35;
	dayestimate=searchsize/(checksperyear)/2;
	printf("20 character pass using a-zA-Z0-9: %.0Lf years.\n",dayestimate);
	searchsize=1.73183888392163E79;
	dayestimate=searchsize/(checksperyear)/2;
	printf("56 character pass using only a-z:  %.0Lf years.\n",dayestimate);
	searchsize=2.36556350750549E100;
	dayestimate=searchsize/(checksperyear)/2;
	printf("56 character pass using a-zA-Z0-9: %.0Lf years.\n",dayestimate);
	printf("-----------------------------------------------\n\n");

	// all done
	return 0;
}

