#ifndef THREAD_IMAGEHANDLE_H
#define THREAD_IMAGEHANDLE_H

#include <QThread>
#include <qmutex.h>
#include <opencv2/opencv.hpp>
//#include <wiringPi.h>
#include <QEventLoop>
#include <QTimer>

class Thread_ImageHandle : public QThread
{
    Q_OBJECT
public:
    int num;
public:
    explicit Thread_ImageHandle();
    ~Thread_ImageHandle();
    void run();
    void stopimmedqiately();
    void Delay_Ms(unsigned int msec);

    void reset_signal(int mgpio);
    int check_point(int mgpio);
    void move_signal(int mgpio);

    int   avgGray(cv::Mat img);
    float count(cv::Mat src);
    void  AdaptiveThereshold(cv::Mat src, cv::Mat &dst);
    void  CountAdaptive(cv::Mat frame, cv::Mat& binaryImg, cv::Mat& inBinaryImg, float& result1, float& result2);
    void  CountOtsu(cv::Mat frame, cv::Mat& binaryImg, float& result);
    void  filtrate(cv::Mat frame, cv::Mat& result_binary, float& result);
    void  count_distance_angle(cv::Mat src, float binary_result, float &mean_distance, float &mean_angle);
public slots:
//    void RecvMoveSignal();
//    void RecvScanSignal();
//    void RecvResetSignal();
signals:
    void DisplayImageSignal(float result);

private:
    cv::Mat Image;
    QMutex m_lock;
    bool m_isCanRun;

public:
    int Adaptive_thr = 21;//自适应局部二值化参数
    int mean_Gray_thr = 150;//均值参数
    bool fix_thr = 1;//修正参数
    bool show_thr = 0;//是否输出处理工程的数值与图像    

};



#endif // THREADSHOWCAMERA_H
