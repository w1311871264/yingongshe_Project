#include "global.h"

PylonCamera         *g_PylonCamera=NULL;//相机
Thread_ImageHandle  *g_thread_ImageHandle=NULL;//运动并显示线程
Thread_Image_Show   *g_thread_ImageShow=NULL; //单独显示图像线程

QQueue<cv::Mat>      image_queue;
cv::Mat              cvRawImg; //拍摄的照片
cv::Mat              cvBinImg;
float g_result = 0;

int g_number = 18;

//int Adaptive_thr = 26;
//const int mean_Gray_thr = 100;
//float fix_thr = 0.3;
//const bool show_thr = 0;

