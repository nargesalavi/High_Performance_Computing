#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <ipp.h>
#include <stdio.h>
#include <cmath>
#include <set>


int main(int argc, const char *argv[]){
    int beta = 15 ;
    int w = 3;
    int cols,rows,min,minK,temp,temp2,temp3;
    uchar *right_data,*left_data,*depth_data;
    IplImage *rightImage,*leftImage,*depthImage;
    Ipp64u start, end,time1;
    
    // rightImage = cvLoadImage( "im0.png", CV_LOAD_IMAGE_GRAYSCALE);
    // leftImage  = cvLoadImage( "im1.png", CV_LOAD_IMAGE_GRAYSCALE);
    rightImage = cvLoadImage( "scene_r.pgm", CV_LOAD_IMAGE_GRAYSCALE);
    leftImage  = cvLoadImage( "scene_l.pgm", CV_LOAD_IMAGE_GRAYSCALE);
   \
    depthImage = cvCreateImage( cvGetSize(rightImage), IPL_DEPTH_8U, 1 );
    
    if (!rightImage || !leftImage){
        printf( "can not read the image file\n");
        return -1;
    }
    
    right_data = (uchar *)rightImage->imageData;
    left_data = (uchar *)leftImage->imageData;
    depth_data = (uchar *)depthImage->imageData;
    
    cols= rightImage->width;
    rows= rightImage->height;

    start = ippGetCpuClocks();
    
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){	min = 999999999;minK = beta/2;
            for (int k=0 ; k<beta+1 ; k++){ temp=0;
                for (int m = i-(w-1)/2; m <= i+(w+1)/2; m++){
                    for (int n = j-(w-1)/2; n <= j+(w+1)/2; n++){
                        
                        if( (m >= rows) || (n >= cols) || (m < 0) || (n < 0) ) temp2=0;
                        else  temp2=(int)(*(right_data + i*cols+j));
                        if( (m >= rows) || (n+k >= cols) || (m < 0) || (n+k < 0) ) temp3=0;
                        else  temp3=(int)(*(left_data  + i*cols+(j+k)));
                        
                        temp+=std::abs(temp2-temp3);
                    }
                }
                if (temp <= min) {
                    min = temp;
                    minK = k;
                }
            }
            (*(depth_data + i*cols+j)) =(uchar) (minK * 255.0 / beta);
        }
    }
    end = ippGetCpuClocks();
    time1 = end-start;
    printf ("Stereo Serial = %d \n", (Ipp32s) time1);
    //cvSaveImage( "Serial.png", depthImage );
    //cv::namedWindow( "output Serial", CV_WINDOW_AUTOSIZE );
    cvShowImage( "Output Serial", depthImage );
    cvReleaseImage( &depthImage );
    cvWaitKey(0);
    
}