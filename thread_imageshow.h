#ifndef THREAD_IMAGESHOW_H
#define THREAD_IMAGESHOW_H

#include <QThread>
#include <qmutex.h>
#include <opencv2/opencv.hpp>
//#include <wiringPi.h>
#include <QEventLoop>
#include <QTimer>

class Thread_Image_Show : public QThread
{
    Q_OBJECT
public:

    int Adaptive_thr = 21;//自适应局部二值化参数
    int mean_Gray_thr = 150;//均值参数
    bool fix_thr = 1;//修正参数
    bool show_thr = 0;//是否输出处理工程的数值与图像

    float distance=0;
    float angle=0;
    float result = 0;
    int amount_points = 0;
    float proportion = 5.000 / 1094;//实际距离毫米与像素的比例

    //当电机运动是设置为1，结束运动设置为0
    //作用：将结果传入到折线图的标志
    int MotorIsRunFlag = 0;

public:
    explicit Thread_Image_Show();
    ~Thread_Image_Show();
    void run();
    void stopimmedqiately();

    int   avgGray(cv::Mat img);
    float count(cv::Mat src);
    void  AdaptiveThereshold(cv::Mat src, cv::Mat &dst);
    void  CountAdaptive(cv::Mat frame, cv::Mat& binaryImg, cv::Mat& inBinaryImg, float& result1, float& result2);
    void  CountOtsu(cv::Mat frame, cv::Mat& binaryImg, float& result);
    void  filtrate(cv::Mat frame, cv::Mat& result_binary, float& result);
    void  count_distance_angle(cv::Mat src, float binary_result, float &mean_distance, float &mean_angle);
signals:
    void DisplayContinuImageSignal(float result);

private:
    cv::Mat Image;
    QMutex m_lock;
    bool m_isCanRun;

};


#endif // THREAD_IMAGESHOW_H
