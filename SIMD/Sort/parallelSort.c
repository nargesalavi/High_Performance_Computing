#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include "ipp.h"
#include "x86intrin.h"
#include "stdlib.h"
#include <emmintrin.h>
#include <limits>

using namespace std;

float* min(float* in)
{
	for (int i = 0; i < 4; ++i)
	{	
		float t=numeric_limits<int>::max();int t2=-1;
		for (int j = i; j < 4; ++j)
		{
			if (in[j]<t)
			{
				t=in[j];
				t2=j;
			}
		}
		t=in[t2];
		in[t2]=in[i];
		in[i]=t;
	}
	return in;
}

void merge_4x4_32bit(__m128i &vA, __m128i &vB, __m128i &vMin, __m128i &vMax) 
{
	__m128i vTmp;
	vTmp = _mm_min_epu32(vA, vB);
	vMax = _mm_max_epu32(vA, vB);
	vTmp = _mm_alignr_epi8(vTmp, vTmp, 4);
	vMin = _mm_min_epu32(vTmp, vMax);
	vMax = _mm_max_epu32(vTmp, vMax);
	vTmp = _mm_alignr_epi8(vMin, vMin, 4);
	vMin = _mm_min_epu32(vTmp, vMax);
	vMax = _mm_max_epu32(vTmp, vMax);
	vTmp = _mm_alignr_epi8(vMin, vMin, 4);
	vMin = _mm_min_epu32(vTmp, vMax);
	vMax = _mm_max_epu32(vTmp, vMax);
	vMin = _mm_alignr_epi8(vMin, vMin, 4);
}


void merge_8x8_32bit(__m128i &vA0, __m128i &vA1, __m128i &vB0, __m128i &vB1, __m128i &vMin0, __m128i &vMin1, __m128i &vMax0, __m128i &vMax1) 
{
	merge_4x4_32bit(vA0,vB0,vMin0,vMax0);
	merge_4x4_32bit(vA1,vB1,vMin1,vMax1);

	merge_4x4_32bit(vMax0,vMin1,vMin1,vMax0);
}

void merge_16x16_32bit(	__m128i &vA0,	__m128i &vA1, 	__m128i &vA2, 	__m128i &vA3, 	__m128i &vB0, 	__m128i &vB1, 	__m128i &vB2, 	__m128i &vB3,
						__m128i &vMin0, __m128i &vMin1, __m128i &vMin2, __m128i &vMin3, __m128i &vMax0, __m128i &vMax1, __m128i &vMax2, __m128i &vMax3)
{
	merge_4x4_32bit(vA0,vB0,vMin0,vMax0);
	merge_4x4_32bit(vA1,vB1,vMin1,vMax1);
	merge_4x4_32bit(vA2,vB2,vMin2,vMax2);
	merge_4x4_32bit(vA3,vB3,vMin3,vMax3);

	merge_4x4_32bit(vMax0,vMin1,vMin1,vMax0);
	merge_4x4_32bit(vMax1,vMin2,vMin2,vMax1);
	merge_4x4_32bit(vMax2,vMin3,vMin3,vMax2);

	merge_4x4_32bit(vMax0,vMin2,vMin2,vMax0);
	merge_4x4_32bit(vMax1,vMin3,vMin3,vMax1);

	merge_4x4_32bit(vMax0,vMin3,vMin3,vMax0);
}
void merge_32x32_32bit(	__m128i &vA0, 	__m128i &vA1, 	__m128i &vA2, 	__m128i &vA3, 	__m128i &vA4, 	__m128i &vA5, 	__m128i &vA6, 	__m128i &vA7,
						__m128i &vB0, 	__m128i &vB1, 	__m128i &vB2, 	__m128i &vB3, 	__m128i &vB4, 	__m128i &vB5, 	__m128i &vB6, 	__m128i &vB7,
						__m128i &vMin0, __m128i &vMin1, __m128i &vMin2, __m128i &vMin3, __m128i &vMin4, __m128i &vMin5, __m128i &vMin6, __m128i &vMin7,
						__m128i &vMax0, __m128i &vMax1, __m128i &vMax2, __m128i &vMax3, __m128i &vMax4, __m128i &vMax5, __m128i &vMax6, __m128i &vMax7)
{
	merge_4x4_32bit(vA0,vB0,vMin0,vMax0);
	merge_4x4_32bit(vA1,vB1,vMin1,vMax1);
	merge_4x4_32bit(vA2,vB2,vMin2,vMax2);
	merge_4x4_32bit(vA3,vB3,vMin3,vMax3);
	merge_4x4_32bit(vA4,vB4,vMin4,vMax4);
	merge_4x4_32bit(vA5,vB5,vMin5,vMax5);
	merge_4x4_32bit(vA6,vB6,vMin6,vMax6);
	merge_4x4_32bit(vA7,vB7,vMin7,vMax7);

	merge_4x4_32bit(vMax0,vMin1,vMin1,vMax0);
	merge_4x4_32bit(vMax1,vMin2,vMin2,vMax1);
	merge_4x4_32bit(vMax2,vMin3,vMin3,vMax2);
	merge_4x4_32bit(vMax3,vMin4,vMin4,vMax3);
	merge_4x4_32bit(vMax4,vMin5,vMin5,vMax4);
	merge_4x4_32bit(vMax5,vMin6,vMin6,vMax5);
	merge_4x4_32bit(vMax6,vMin7,vMin7,vMax6);

	merge_4x4_32bit(vMax0,vMin2,vMin2,vMax0);
	merge_4x4_32bit(vMax1,vMin3,vMin3,vMax1);
	merge_4x4_32bit(vMax2,vMin4,vMin4,vMax2);
	merge_4x4_32bit(vMax3,vMin5,vMin5,vMax3);
	merge_4x4_32bit(vMax4,vMin6,vMin6,vMax4);
	merge_4x4_32bit(vMax5,vMin7,vMin7,vMax5);

	merge_4x4_32bit(vMax0,vMin3,vMin3,vMax0);
	merge_4x4_32bit(vMax1,vMin4,vMin4,vMax1);
	merge_4x4_32bit(vMax2,vMin5,vMin5,vMax2);
	merge_4x4_32bit(vMax3,vMin6,vMin6,vMax3);
	merge_4x4_32bit(vMax4,vMin7,vMin7,vMax4);

	merge_4x4_32bit(vMax0,vMin4,vMin4,vMax0);
	merge_4x4_32bit(vMax1,vMin5,vMin5,vMax1);
	merge_4x4_32bit(vMax2,vMin6,vMin6,vMax2);
	merge_4x4_32bit(vMax3,vMin7,vMin7,vMax3);

	merge_4x4_32bit(vMax0,vMin5,vMin5,vMax0);
	merge_4x4_32bit(vMax1,vMin6,vMin6,vMax1);
	merge_4x4_32bit(vMax2,vMin7,vMin7,vMax2);

	merge_4x4_32bit(vMax0,vMin6,vMin6,vMax0);
	merge_4x4_32bit(vMax1,vMin7,vMin7,vMax1);

	merge_4x4_32bit(vMax0,vMin7,vMin7,vMax0);
}

int main(int argc, char const *argv[])
{
	__m128i v[16],v2[16];
	Ipp64u start1, end1, start2, end2;
	Ipp64u time1=0, time2=0;
	float **x,**y;

	x = new float* [16];
	y = new float* [16];

	for (int i = 0; i < 16; i++){
		x[i] = new float[4];
		y[i] = new float[4];
	}
	for (int i = 0; i < 100000; ++i)
	{
		for (int i = 0; i < 16; i++)
			for (int j = 0; j < 4; j++)
				y[i][j] = (float) rand();

		start1 = ippGetCpuClocks();
		for (int i = 0; i < 16; i++)
			x[i] =min(y[i]);
		end1   = ippGetCpuClocks();
		time1 += (end1 - start1);

		for (int i = 0; i < 16; i++)
			v[i] = _mm_load_si128((__m128i *)x[i]);

		start2 = ippGetCpuClocks();
		merge_4x4_32bit(v[0],  v[1],  v2[0],  v2[1]);
		merge_4x4_32bit(v[2],  v[3],  v2[2],  v2[3]);
		merge_4x4_32bit(v[4],  v[5],  v2[4],  v2[5]);
		merge_4x4_32bit(v[6],  v[7],  v2[6],  v2[7]);
		merge_4x4_32bit(v[8],  v[9],  v2[8],  v2[9]);
		merge_4x4_32bit(v[10], v[11], v2[10], v2[11]);
		merge_4x4_32bit(v[12], v[13], v2[12], v2[13]);
		merge_4x4_32bit(v[14], v[15], v2[14], v2[15]);

		merge_8x8_32bit(v2[0],  v2[1],  v2[2],  v2[3],  v[0],  v[1],  v[2],  v[3]);
		merge_8x8_32bit(v2[4],  v2[5],  v2[6],  v2[7],  v[4],  v[5],  v[6],  v[7]);
		merge_8x8_32bit(v2[8],  v2[9],  v2[10], v2[11], v[8],  v[9],  v[10], v[11]);
		merge_8x8_32bit(v2[12], v2[13], v2[14], v2[15], v[12], v[13], v[14], v[15]);

		merge_16x16_32bit(v[0], v[1], v[2],  v[3],  v[4],  v[5],  v[6],  v[7],  v2[0], v2[1], v2[2],  v2[3],  v2[4],  v2[5],  v2[6],  v2[7]);
		merge_16x16_32bit(v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v2[8], v2[9], v2[10], v2[11], v2[12], v2[13], v2[14], v2[15]);

		merge_32x32_32bit(v2[0], v2[1], v2[2], v2[3], v2[4], v2[5], v2[6], v2[7], v2[8], v2[9], v2[10], v2[11], v2[12], v2[13], v2[14], v2[15],
						  v [0],  v[1],  v[2],  v[3],  v[4],  v[5],  v[6],  v[7],  v[8],  v[9],  v[10],  v[11],  v[12],  v[13],  v[14],  v[15]);
		end2   = ippGetCpuClocks();
		time2 += (end2 - start2);
	}
	
	printf ("Parallel Run time = %d \n", (Ipp32s) ((time2+time1)/100000));

	for (int i = 0; i < 16; i++)
		_mm_store_si128((__m128i *)y[i], v[i]);

	// cout<<endl;
	// for (int i = 0; i < 16; ++i)
	// 	cout<<y[i][0]<<" "<<y[i][1]<<" "<<y[i][2]<<" "<<y[i][3]<<endl;
	// cout<<endl;
	
	return 0;
}