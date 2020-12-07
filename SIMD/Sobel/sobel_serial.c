#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp> //
#include <iostream>
#include "ipp.h"
#include "cv.h"


int main()
{

  CvCapture* capture = 0;
  IplImage *image = 0;
  IplImage *img = 0;
  IplImage *img_edg = 0;
  int step;
  uchar* data2;
  uchar* data_dst;
  Ipp64u start, end, time1;
  int s;
  int dx[3][3] = {{1,0,-1},{2,0,-2},{1,0,-1}};
  int dy[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};

  int cols;

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

    ///***********sobel direct access algorithm
    step = img->widthStep/sizeof(uchar);
  	data2 = (uchar *)img->imageData;
  	data_dst = (uchar *)img_edg->imageData;

  	start = ippGetCpuClocks();
    for (int i=1; i < img->height-2; i++)
    for (int j=1; j < img->width-2; j++)
    {
      // apply kernel in X direction
      int sum_x=0;
      for(int m=-1; m<=1; m++)
        for(int n=-1; n<=1; n++)
        {
          s=data2[(i+m)*step+j+n]; // get the (i,j) pixel value
          sum_x+=s*dx[m+1][n+1];
        }
      // apply kernel in Y direction
      int sum_y=0;
      for(int m=-1; m<=1; m++)
        for(int n=-1; n<=1; n++)
        {
          s=data2[(i+m)*step+j+n]; // get the (i,j) pixel value
          sum_y+=s*dy[m+1][n+1];
        }
      int sum=sqrt(pow(sum_x,2)+pow(sum_y,2));
      if (sum>255)
        sum=255;
      data_dst[i*step+j]=sum; // set the (i,j) pixel value
    }
    end = ippGetCpuClocks();
    time1 = end-start;
    printf ("Sobel_dir serial = %d \n", (Ipp32s) time1);
   // t = (double)cvGetTickCount() - t;
  	//printf( "running time using direct access = %gms\n", t/((double)cvGetTickFrequency()*1000.));
  	cvNamedWindow( "Original image", 1 );
  	//cvNamedWindow( "direct access", 1 );  
  	cvShowImage( "output serial", img_edg );
  
  	cvReleaseImage( &img );
  	cvReleaseImage( &img_edg );
  	cvWaitKey(0);

  }