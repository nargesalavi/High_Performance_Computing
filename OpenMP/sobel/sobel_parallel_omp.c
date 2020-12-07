#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp> //
//#include <iostream>
#include <stdio.h>
//#include <ipp.h>
#include "cv.h"
#include "omp.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <algorithm>
// #include <chrono>

int main()
{
  	CvCapture* capture = 0;
  	IplImage *image = 0;
  	IplImage *img = 0;
  	IplImage *img_edg = 0;
  	IplImage *img_edg2 = 0;
  	int step,i,j,m,n,sum,p2,p;
  	int sum_y,sum_x;
  	uchar* data2;
  	uchar* data_dst,*data_dst2;
  	double  time1, time2;
  	int s;
  	int dx[3][3] = {{1,0,-1},{2,0,-2},{1,0,-1}};
  	int dy[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
  	int cols;

  	//Ipp64u start, end, start2, end2;
  	double start,start2,end;

	const char* filename = (char*)"lena.png";
    image = cvLoadImage( filename, 1 );

    if (!image)
      printf( "can not read the image file\n");
    else
	{
	    //cvShowImage( "Original image", image );
	  	img=cvCreateImage( cvSize(image->width,image->height), 8, 1 );
	  	if ( image->nChannels!=1)cvCvtColor( image, img, CV_BGR2GRAY );
	  	else cvCopy( image, img);
	  	img_edg = cvCreateImage( cvSize(image->width,image->height), 8, 1 );
	  	img_edg2 = cvCreateImage( cvSize(image->width,image->height), 8, 1 );
    }
    step = img->widthStep/sizeof(uchar);
  	data2 = (uchar *)img->imageData;
  	data_dst = (uchar *)img_edg->imageData;
  	data_dst2 = (uchar *)img_edg2->imageData;




	
    //start2 = ippGetCpuClocks();
    start2 = omp_get_wtime();
    for (int i=1; i < img->height-2; i++)
		for (int j=1; j < img->width-2; j++)
		{
		      
		    int sum_x=0;
		    for(int m=-1; m<=1; m++)
		    for(int n=-1; n<=1; n++)
		    {
		        s=data2[(i+m)*step+j+n]; 
		        sum_x+=s*dx[m+1][n+1];
		    }
		      
		    int sum_y=0;
		    for(int m=-1; m<=1; m++)
		    for(int n=-1; n<=1; n++)
		    {
		        s=data2[(i+m)*step+j+n]; 
		        sum_y+=s*dy[m+1][n+1];
		    }

		    int sum=sqrt(pow(sum_x,2)+pow(sum_y,2));
		    if (sum>255)
		    	sum=255;
		    data_dst2[i*step+j]=sum;
		}
	time2 = omp_get_wtime() - start2;
    // end2 = ippGetCpuClocks();
    // time2 = (float)(end2-start2);






	for (p = 2; p < 50; p+=2)
	{
		time1=0;
		for (p2 = 0; p2 < 5;)
		{
			//start = ippGetCpuClocks();
			start = omp_get_wtime();

			#pragma omp parallel for num_threads(p) private(i,j,m,n,s,sum_x,sum_y,sum) shared(data_dst,data2,step,dy,dx)
			#pragma for schedule( dynamic,1 )
			for (i=1; i < img->height-2; i++)
			for (j=1; j < img->width-2; j++)
			{
				sum_x=0;
				for(m=-1; m<=1; m++)
				for(n=-1; n<=1; n++)
				{
			        s=data2[(i+m)*step+j+n]; 
					sum_x+=s*dx[m+1][n+1];
				}
					      
				sum_y=0;
				for(m=-1; m<=1; m++)
				for(n=-1; n<=1; n++)
				{
					s=data2[(i+m)*step+j+n]; 
					sum_y+=s*dy[m+1][n+1];
				}

				sum=sqrt(pow(sum_x,2)+pow(sum_y,2));
				if (sum>255)
					sum=255;
				data_dst[i*step+j]=sum;
			}
			end = omp_get_wtime();

			//end = ippGetCpuClocks();
			if((end-start)<time2 ){
				time1 +=end-start;
				p2++;
			}
		}
		time1=time1/p2;
		printf(" %.16g  %.16g ",time1,time2);
	    printf ("speedup =  %.16g \n", (time2/time1));
	}






  	//cvNamedWindow( "Original image", 1 );
  	//cvShowImage( "output1", img_edg );
  	//cvShowImage( "output2", img_edg2 );
  
  	cvReleaseImage( &img );
  	cvReleaseImage( &img_edg );
  	cvReleaseImage( &img_edg2 );
  	cvWaitKey(0);

  }