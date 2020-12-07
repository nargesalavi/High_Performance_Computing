#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <ipp.h>
#include <iostream>
#include <cmath>
#include <set>

inline int calculate_array_element(unsigned char *array, int i, int j, int cols, int rows) {
	if ((i >= rows) || (j >= cols)
			|| i < 0 || j < 0) {
		return 0;
	}
	else {
		return (int)(*(array + i * cols + j));
	}
}

int main(int argc, const char *argv[]){
	int beta = 15;
	int w = 3;
	int cols,rows,min,minK,temp,temp2,temp3;
	uchar *right_data,*left_data,*depth_data;
	Ipp64u start, end;
	IplImage *rightImage,*leftImage,*depthImage;

    rightImage = cvLoadImage("scene_r.pgm", 1 );
    leftImage  = cvLoadImage( "scene_l.pgm", 1 );
  	depthImage = cvCreateImage( cvGetSize(rightImage), IPL_DEPTH_8U, 1 );


    if (!rightImage || !leftImage){
        	printf( "can not read the image file\n");
        	return -1;
    }


  	right_data = (uchar *)rightImage->imageData;
  	left_data = (uchar *)leftImage->imageData;
  	depth_data = (uchar *)depthImage->imageData;
  	//cvShowImage( "output Serial", leftImage );
    //cvReleaseImage( &leftImage );
  	cols= rightImage->width;
    rows= rightImage->height;

    for (int i = ((w - 1) / 2); i < rows- ((w - 1) / 2); i++){
		for (int j = ((w - 1) / 2); j < cols-16- ((w - 1) / 2); j++){	min = 10000;minK = 10000;
			for (int k=0 ; k<beta+1 ; k++){ temp=0;
				for (int m = -((w - 1) / 2); m <= ((w - 1) / 2); m++)
					for (int n = -((w - 1) / 2); n <= ((w - 1) / 2); n++)
						temp+=std::abs((int)(left_data[(m+i)*cols+(n+j)]-right_data[(m+i)*cols+(n+j+k)]));
				
				if (temp < min) {
					min = temp;
					minK = k;
				}
			}
			depth_data[i*cols+j] =(unsigned char) (minK * 4);
		}
	}
    cvShowImage( "output Serial", depthImage );
    cvReleaseImage( &depthImage );
    cvWaitKey(0);

 }
