#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "ipp.h"
#define MAX 64
#define SWAP(x,y) t = x; x = y; y = t;
 
using namespace std;

void compare();
void bitonicmerge(int, int, int);
void recbitonic(int, int, int);
void sort();
 
float data[MAX];
int up = 1;
int down = 0;
 
int main()
{
    int i;
 	Ipp64u start=0, end=0;
	Ipp64u Time;
	for (int i = 0; i < 100000; ++i)
	{
		for (int i = 0; i < 64; ++i)
	    	data[i]=(float) rand();

		start = ippGetCpuClocks();
	    sort();
	    end   = ippGetCpuClocks();
		Time += (end - start);
	}
    
	printf ("serial Run time = %d \n", (Ipp32s) (Time/100000));
    // for (i = 0;i < MAX;i++){
    //     cout<<data[i]<<" ";
    //     if(i%4==3)
    //     	printf("\n");
    // }
}

void compare(int i, int j, int dir)
{
    float t;
 
    if (dir == (data[i] > data[j]))
    {
        t=data[i];
        data[i]=data[j];
        data[j]=t;
    }
}

void bitonicmerge(int low, int c, int dir)
{
    int k, i;
 
    if (c > 1)
    {
         k = c / 2;
        for (i = low;i < low+k ;i++)
            compare(i, i+k, dir);    
        bitonicmerge(low, k, dir);
        bitonicmerge(low+k, k, dir);    
    }
}

void recbitonic(int low, int c, int dir)
{
    int k;
 
    if (c > 1)
    {
        k = c / 2;
        recbitonic(low, k, up);
        recbitonic(low + k, k, down);
        bitonicmerge(low, c, dir);
    }
}
 

void sort()
{
    recbitonic(0, MAX, up);
}