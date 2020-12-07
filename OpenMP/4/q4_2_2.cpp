// Example Chapter 3 OpenMP Program
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <Windows.h>
#pragma comment(lib, "Winmm.lib")

const long int VERYBIG = 100000;

int main(void)
{
	int i;
	long int j, k, sum;
	double sumx, sumy, total, z;
	int NUM_OF_TEST=6
	int NUM_OF_THR 4
	double starttime, elapsedtime[NUM_OF_TEST][NUM_OF_THR];
	double execution_time[4] = { 0, 0, 0, 0 };
	

	printf("OpenMP Parallel Timings for %ld iterations \n\n", VERYBIG);

	// repeat experiment several times
	for (i = 0; i < NUM_OF_TEST; i++)
	{
		// get starting time
		starttime = timeGetTime();
		// reset check sum and total
		sum = 0;
		total = 0.0;
		int tid;
		// Work loop, do some work by looping VERYBIG times
#pragma omp parallel num_threads (4) private(tid)
		{
#pragma omp for	private(sumx, sumy, k) reduction(+: sum, total) schedule(static) nowait
			for (int j = 0; j < VERYBIG; j++)
			{
				// increment check sum
				sum += 1;

				// Calculate first arithmetic series
				sumx = 0.0;
				for (k = 0; k < j; k++)
					sumx = sumx + (double)k;

				// Calculate second arithmetic series
				sumy = 0.0;
				for (k = j; k > 0; k--)
					sumy = sumy + (double)k;

				if (sumx > 0.0)total = total + 1.0 / sqrt(sumx);
				if (sumy > 0.0)total = total + 1.0 / sqrt(sumy);
			}
			tid = omp_get_thread_num();
			elapsedtime[i][tid] = timeGetTime() - starttime;
			printf("thread num %d test iteration %d time: %.0lf ms\n", tid, i, elapsedtime[i][tid]);
			execution_time[tid] += elapsedtime[i][tid];
		}

	}
	for (size_t i = 0; i < NUM_OF_THR; i++)
	{
		printf("Thread %d average execution time is %.0lf ms\n", i, execution_time[i] / NUM_OF_TEST);
	}

	// return integer as required by function header
	return 0;
}
// **********************************************************************
