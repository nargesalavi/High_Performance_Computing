#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp> //
#include <iostream>
#include "x86intrin.h"
#include "immintrin.h"
#include "emmintrin.h"
#include "smmintrin.h"
#include "tmmintrin.h"
#include "cv.h"
#include "omp.h"
#include <math.h>
using namespace std;

int main()
{

  IplImage *image1 = 0;
  IplImage *image2 = 0;
  
  IplImage *img1 = 0;
  IplImage *img2 = 0;
  
  IplImage *img_sub = 0;
  
  unsigned char* data1;
  unsigned char* data2;
  
  unsigned char* data_dst;
  
  // Ipp64u start, end, time1;
  double start1,start2,end;
  double  time1, time2;

  int cols,rows;
  

	const char* filename = (char*)"lena.png";
  image1 = cvLoadImage( filename, 1 );
  if (!image1)
	{
    printf( "can not read the image1 file\n");
	  return 0;
	}
  else
  {
		cvShowImage( "Original image1", image1 );

    img1=cvCreateImage( cvSize(image1->width,image1->height), 8, 1 );
    if ( image1->nChannels!=1)
			cvCvtColor( image1, img1, CV_BGR2GRAY );
		else
			cvCopy( image1, img1);
		data1 = (unsigned char *)img1->imageData;
  }
	
	
	filename = (char*)"lena.png";
  image2 = cvLoadImage( filename, 1 );
  if (!image2)
	{
    printf( "can not read the image2 file\n");
	  return 0;
	}
  else
  {
	 cvShowImage( "Original image2", image2 );

    img2=cvCreateImage( cvSize(image2->width,image2->height), 8, 1 );
  	if ( image2->nChannels!=1)
			cvCvtColor( image2, img2, CV_BGR2GRAY );
		else
			cvCopy( image2, img2);
		data2 = (unsigned char *)img2->imageData;
  }

	
  img_sub = cvCreateImage( cvSize(image2->width,image2->height), 8, 1 );
  data_dst = (unsigned char*)img_sub->imageData;

    ////++++++++++++++++++++++++++SIMD PARALLELISM+++++++
  cols= img1->width;
  rows= img1->height;
	
 
  start2 = omp_get_wtime();

	
	
	
	
	
  int16_t *temp = (int16_t *) _mm_malloc(sizeof(int16_t)*8, 16);

  for (int i=0; i < rows; i++)
		for (int j=0; j < cols/16; j+=16){

			//__m128i lu1 = _mm_unpacklo_epi8(_mm_loadu_si128( (__m128i *) (data1+i*cols+j)), _mm_setzero_si128() );
			//__m128i lu2 = _mm_unpacklo_epi8(_mm_loadu_si128( (__m128i *) (data2+i*cols+j)), _mm_setzero_si128() );
			__m128i lu1 = _mm_loadu_si128( (__m128i *) (data1+i*cols+j));
			__m128i lu2 = _mm_loadu_si128( (__m128i *) (data2+i*cols+j));
			
			//lu2 = _mm_abs_epi16(_mm_sub_epi16(lu1,lu2));
			lu2 =_mm_sub_epi16(lu1,lu2);

			_mm_store_si128 ( (__m128i *) temp, lu2);

			for (int k = 0; k < 8; ++k)
				*(data_dst + i*cols + j+k) = *(temp+k);
		}





  time2 = omp_get_wtime() - start2;
  printf ("sub_img Parallel = %.16g \n", time2);
  cvShowImage( "output Parallel", img_sub );
  




  start1 = omp_get_wtime();

  for (int i=0; i < rows; i++)
    for (int j=0; j < cols; j++)
      data_dst[i*cols+j] = data1[i*cols+j]-data2[i*cols+j];

  time1 = omp_get_wtime() - start1;
  printf ("sub_img Serial = %.16g \n", time1);
  cvShowImage( "output Serial", img_sub );


  printf ("Speedup = %.16g \n", time1/time2);

  cvReleaseImage( &img_sub );
  cvWaitKey(0);
  

  return 0;
}