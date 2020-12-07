#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp> //
#include <iostream>
#include "ipp.h"
#include "x86intrin.h"
#include "immintrin.h"
#include "emmintrin.h"
#include "smmintrin.h"
#include "tmmintrin.h"
#include "cv.h"
#include <math.h>
using namespace std;

int main()
{

  IplImage *image = 0;
  IplImage *img = 0;
  IplImage *img_edg = 0;
  unsigned char* data;
  unsigned char* data_dst;
  Ipp64u start, end, time1;
  int s;
  int cols,rows;
  

   const char* filename = (char*)"Lenna.png";
    image = cvLoadImage( filename, 1 );
    if (!image)
      printf( "can not read the image file\n");
    else
    {
      cvShowImage( "Original image", image );

    	img=cvCreateImage( cvSize(image->width,image->height), 8, 1 );
    	if ( image->nChannels!=1)
      cvCvtColor( image, img, CV_BGR2GRAY );
    	else
      cvCopy( image, img);

    	img_edg = cvCreateImage( cvSize(image->width,image->height), 8, 1 );
    	//double t = (double)cvGetTickCount();
    }

    data = (unsigned char *)img->imageData;

    ////++++++++++++++++++++++++++SIMD PARALLELISM+++++++
    cols= img->width;
    rows= img->height;
    unsigned char* data2= (unsigned char *) _mm_malloc(sizeof(unsigned char) * (cols*rows + 16), 16);
    data_dst = (unsigned char*)img_edg->imageData;
    start = ippGetCpuClocks();

    int16_t *temp = (int16_t *) _mm_malloc(sizeof(int16_t)*8, 16);

    // apply kernel in X direction
    for (int i=1; i < rows-1; i++)
    for (int j=1; j < cols-16; j+=8){

      __m128i lu1 = _mm_unpacklo_epi8(_mm_loadu_si128( (__m128i *) ((data+i*cols+j)-cols-1)), _mm_setzero_si128() );
      __m128i lu2 = _mm_unpacklo_epi8(_mm_loadu_si128( (__m128i *) ((data+i*cols+j)-cols)), _mm_setzero_si128() );
      __m128i lu3 = _mm_unpacklo_epi8(_mm_loadu_si128( (__m128i *) ((data+i*cols+j)-cols+1)), _mm_setzero_si128() );

      __m128i ld1 = _mm_unpacklo_epi8(_mm_loadu_si128( (__m128i *) ((data+i*cols+j)+cols-1)), _mm_setzero_si128() );
      __m128i ld2 = _mm_unpacklo_epi8(_mm_loadu_si128( (__m128i *) ((data+i*cols+j)+cols)), _mm_setzero_si128() );
      __m128i ld3 = _mm_unpacklo_epi8(_mm_loadu_si128( (__m128i *) ((data+i*cols+j)+cols+1)), _mm_setzero_si128() );

      lu2 =_mm_add_epi16(_mm_add_epi16(_mm_add_epi16(lu2,lu2),lu1),lu3);
      ld2 =_mm_add_epi16(_mm_add_epi16(_mm_add_epi16(ld2,ld2),ld1),ld3);

      lu2 = _mm_abs_epi16(_mm_sub_epi16(lu2,ld2));
      _mm_store_si128 ( (__m128i *) temp, lu2);

      for (int k = 0; k < 8; ++k)
                *(data_dst + i*cols + j+k) = *(temp+k);
    }


    for (int i = 0; i < rows; ++i) 
        for (int j = 0; j < cols; ++j)
            *(data2 + j*rows + i) = *(data + i*cols + j);

    // apply kernel in Y direction
    for (int i=1; i < cols-1; i++)
    for (int j=1; j < rows-16; j+=8){
      __m128i lu1 = _mm_unpacklo_epi8(_mm_loadu_si128( (__m128i *) ((data2+i*rows+j)-rows-1)), _mm_setzero_si128() );
      __m128i lu2 = _mm_unpacklo_epi8(_mm_loadu_si128( (__m128i *) ((data2+i*rows+j)-rows)), _mm_setzero_si128() );
      __m128i lu3 = _mm_unpacklo_epi8(_mm_loadu_si128( (__m128i *) ((data2+i*rows+j)-rows+1)), _mm_setzero_si128() );

      __m128i ld1 = _mm_unpacklo_epi8(_mm_loadu_si128( (__m128i *) ((data2+i*rows+j)+rows-1)), _mm_setzero_si128() );
      __m128i ld2 = _mm_unpacklo_epi8(_mm_loadu_si128( (__m128i *) ((data2+i*rows+j)+rows)), _mm_setzero_si128() );
      __m128i ld3 = _mm_unpacklo_epi8(_mm_loadu_si128( (__m128i *) ((data2+i*rows+j)+rows+1)), _mm_setzero_si128() );

      lu2 =_mm_add_epi16(_mm_add_epi16(_mm_add_epi16(lu2,lu2),lu1),lu3);
      ld2 =_mm_add_epi16(_mm_add_epi16(_mm_add_epi16(ld2,ld2),ld1),ld3);

      lu2 = _mm_abs_epi16(_mm_sub_epi16(lu2,ld2));
      _mm_store_si128 ( (__m128i *) temp, lu2);

      for (int k = 0; k < 8; ++k)
        if(*(temp+k)+ *(data_dst + (j+k)*rows + i) > 255)
                *(data_dst + (j+k)*rows + i) =255;
        else
                *(data_dst + (j+k)*rows + i) =*(temp+k)+ *(data_dst + (j+k)*rows + i);
    }
      
    
      
    end = ippGetCpuClocks();
    time1 = end-start;
    printf ("Sobel_dir Parallel = %d \n", (Ipp32s) time1);
    cvShowImage( "output Parallel", img_edg );
  
    cvReleaseImage( &img_edg );
    cvWaitKey(0);
  

  return 0;
}

