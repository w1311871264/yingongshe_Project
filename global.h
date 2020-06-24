#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <pyloncamera.h>
#include <opencv2/opencv.hpp>
#include <thread_imagehandle.h>
#include <thread_imageshow.h>
#include <QQueue>

extern PylonCamera *g_PylonCamera;
extern Thread_ImageHandle  *g_thread_ImageHandle;
extern Thread_Image_Show *g_thread_ImageShow;


extern QQueue<cv::Mat> image_queue;
extern cv::Mat cvRawImg;
extern cv::Mat cvBinImg;
extern float g_result;

extern int g_number;

//extern int Adaptive_thr;
//extern const int mean_Gray_thr;
//extern float fix_thr;
//extern const bool show_thr;

#endif
