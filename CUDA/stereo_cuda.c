#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <ipp.h>
#include <iostream>
#include <cmath>
#include <set>
int cols,rows;
int beta = 15;
int w = 3;
IplImage *rightImage,*leftImage,*depthImage;

inline int calculate_array_element(unsigned char *array, int i, int j, int cols, int rows) {
	if ((i >= rows) || (j >= cols)
			|| i < 0 || j < 0) {
		return 0;
	}
	else {
		return (int)(*(array + i * cols + j));
	}
}

__device__ int getGlobalIdx_2D_2D(){ 

	int blockId = blockIdx.x + blockIdx.y * gridDim.x; 
	int threadId = blockId * (blockDim.x * blockDim.y)+ (threadIdx.y * blockDim.x) + threadIdx.x; 
	return threadId; 
}

double cpuSecond() {
  struct timeval tp;
  gettimeofday(&tp,NULL);
  return ((double)tp.tv_sec + (double)tp.tv_usec*1.e-6);
}

__global__ void calc(void){
	int threadID_=getGlobalIdx_2D_2D()

	int min = 10000;
	int minK = 10000;
	int i=threadID_%cols;
	int j=threadID_/cols;
	int temp;
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


int main(int argc, const char *argv[]){
	// int beta = 15;
	// int w = 3;
	int temp2,temp3;
	uchar *right_data,*left_data,*depth_data;
	Ipp64u start, end;
	

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
    double iStart;
    double iElaps;
    // define grid and block structure
    for(int i=16;i<512;i+=64)
    	for(int j=16;j<512;j+=32){
			dim3 block(i,j);
    		dim3 grid((cols + block.x - 1) / block.x),(rows + block.y - 1) / block.y));
			iStart=cpuSecond();
			calc<<grid,block>>();
			cudaDeviceSynchronize();
			iElaps=cpuSecond()-iStart;

    	}

    
	//	}
	//}
    cvShowImage( "output Serial", depthImage );
    cvReleaseImage( &depthImage );
    cvWaitKey(0);

 }
