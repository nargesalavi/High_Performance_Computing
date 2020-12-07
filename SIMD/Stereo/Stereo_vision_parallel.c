#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <ipp.h>
#include <stdio.h>
#include <math.h>
//#include <set>
#include "x86intrin.h"
#include "immintrin.h"
#include "emmintrin.h"
#include "smmintrin.h"
#include "tmmintrin.h"
#include "cv.h"



int main(int argc, const char *argv[]){
    int beta = 15 ;
    int w = 3;
    int margin=w/2+1;
    int cols,rows;
    uchar *right_data,*left_data,*depth_data;
    IplImage *rightImage,*leftImage,*depthImage;
    __m128i temp2,temp3,temp,min,minK;
    int16_t array1[8];
    Ipp64u start, end,time1;
    
    rightImage = cvLoadImage( "scene_r.pgm", CV_LOAD_IMAGE_GRAYSCALE);
    leftImage  = cvLoadImage( "scene_l.pgm", CV_LOAD_IMAGE_GRAYSCALE);
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
    for (int i = margin; i < rows-margin-1; i++){
        for (int j = margin; j < cols-margin-8; j+=8){ 
            min = _mm_set1_epi16((int16_t)999999999);
            minK = _mm_set1_epi16((int16_t)(beta/2));
            for (int k=0 ; k<beta+1 ; k++){
                temp=_mm_set1_epi16((uchar)0X00);
                for (int m = i-(w-1)/2; m <= i+(w+1)/2; m++){
                    for (int n = j-(w-1)/2; n <= j+(w+1)/2; n++){
                        temp2=_mm_loadu_si128((__m128i*)(right_data + m*cols+n));
                        temp3=_mm_loadu_si128((__m128i*)(left_data  + m*cols+(n+k)));
                        temp=_mm_add_epi16(temp,_mm_abs_epi16(_mm_sub_epi16(temp2,temp3)));
                    }
                }

               minK= _mm_add_epi16(_mm_and_si128(_mm_cmpgt_epi16(min, temp),_mm_set1_epi16( (int16_t)k )) 
                                  ,_mm_and_si128(_mm_cmpgt_epi16(temp, min),minK ));
               min = _mm_min_epi16(min,temp);
            }

            //cout << "khari??\n" << endl;
            minK = _mm_mullo_epi16(minK, _mm_set1_epi16( (int16_t)(255.0 / beta)) );
            //minKhi = _mm_mulhi_epi16(minK, _mm_set1_epi16( (int16_t)(255.0 / beta)) );


            _mm_store_si128( (__m128i *) array1, minK);
            //_mm_store_si128( (__m128i *) array2, minKhi);

            for (int x= 0; x< 8; x++)
                *(depth_data + i * cols + j + x) = (uchar) array1[x];

            // _mm_store_si128((__m128i*)(depth_data + i*cols+j),
            //                  _mm_mullo_epi16(minK , _mm_set1_epi16( (int16_t)(255.0 / beta))));

        }
    }
     end = ippGetCpuClocks();
    time1 = end-start;
    printf ("Stereo Parallel = %d \n", (Ipp32s) time1);

    cvShowImage( "Output Parallel", depthImage );
    cvReleaseImage( &depthImage );
    cvWaitKey(0);
}

    