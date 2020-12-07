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
  double start1,start2,end2;
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
	



  start1 = omp_get_wtime();

  for (int i=0; i < rows; i++)
    for (int j=0; j < cols; j++)
      data_dst[i*cols+j] = data1[i*cols+j]-data2[i*cols+j];

  time1 = omp_get_wtime() - start1;
  printf ("sub_img Serial = %.16g \n", time1);
  cvShowImage( "output Serial", img_sub );




  int p,p2;
  for (p = 2; p < 50; p+=2)
  {
    time2=0;
    for (p2 = 0; p2 < 5;)
    {
      start2 = omp_get_wtime();

      #pragma omp parallel for num_threads(p) shared(data_dst,data2,data1)
      #pragma for schedule( dynamic,1 )
      for (int i=0; i < rows; i++)
        for (int j=0; j < cols; j++)
          data_dst[i*cols+j] = data1[i*cols+j]-data2[i*cols+j];

      end2 = omp_get_wtime();

      //end = ippGetCpuClocks();
      if((end2-start2)<time1 ){
        time2 +=end2-start2;
        p2++;
      }
    }
    time2=time2/p2;
    printf ("sub_img Parallel = %.16g ", time2);
    printf ("Speedup = %.16g \n", time1/time2);
  }
  cvShowImage( "output Parallel", img_sub );
  



  cvReleaseImage( &img_sub );
  cvWaitKey(0);
  

  return 0;
}