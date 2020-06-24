#include "thread_imagehandle.h"
#include <global.h>
#include <QDebug>

Thread_ImageHandle::Thread_ImageHandle(){
    m_isCanRun = false;

//    wiringPiSetup();
//    pinMode(23,OUTPUT);
//    pinMode(24,OUTPUT);
//    pinMode(7,INPUT);
    num = 18;
}

Thread_ImageHandle::~Thread_ImageHandle(){

}

void Thread_ImageHandle::run()
{
    int count=1;
    bool Image_flag = false;
    Mat local_frame;
    Mat result_binary;
    int working_mode = 0;
    float result = 0;
    m_isCanRun = true;
    while(1){
        {
            QMutexLocker locker(&m_lock);
            if(!m_isCanRun)
            {
                qDebug() << ">>Quit the thread of Image Handle.";
                return;
            }
        }
        Image_flag = false;
        if(count <= num)
        {
            while((count!=1))//&&check_point(7));
            while(!Image_flag)
            {
                cvRawImg = g_PylonCamera->GetCameraImage(g_PylonCamera->camera, g_PylonCamera->ptrGrabResult2D, g_PylonCamera->PyImg2D);
                if(!cvRawImg.empty()){
                    cvtColor(cvRawImg, cvRawImg, COLOR_BGR2GRAY);
                    local_frame = cvRawImg(Rect(cvRawImg.cols/3, cvRawImg.rows/3, cvRawImg.cols/3, cvRawImg.rows/3));
                    filtrate(local_frame, result_binary, result);
                    Image_flag = true;
                    emit DisplayImageSignal(result);
                    //move_signal(24);
                    Delay_Ms(800);
                }
                else{
                    qDebug() << "Image is empty.";
                }
                count++;
            }
        }
        else{
            stopimmedqiately();
            qDebug() << "Fiinning!";
        }
//        cvRawImg = g_PylonCamera->GetCameraImage(g_PylonCamera->camera, g_PylonCamera->ptrGrabResult2D, g_PylonCamera->PyImg2D);
//        if(!cvRawImg.empty()){
//            cvtColor(cvRawImg, cvRawImg, COLOR_BGR2GRAY);
//            local_frame = cvRawImg(Rect(cvRawImg.cols/3, cvRawImg.rows/3, cvRawImg.cols/3, cvRawImg.rows/3));
//            filtrate(local_frame, result_binary, result);
//            //qDebug() << "result = "<<result;
//            emit DisplayImageSignal();
//        }
//        else{
//            qDebug() << "Image is empty.";
//        }
    }
}

//void Thread_ImageHandle::reset_signal(int mgpio){
//    digitalWrite(mgpio,LOW);
//    Delay_Ms(100);
//    digitalWrite(mgpio,HIGH);
//}

//void Thread_ImageHandle::move_signal(int mgpio){
//    digitalWrite(mgpio,LOW);
//    Delay_Ms(100);
//    digitalWrite(mgpio,HIGH);
//}

//int Thread_ImageHandle::check_point(int mgpio){
//    return digitalRead(mgpio);
//}

//void Thread_ImageHandle::RecvMoveSignal(){
//    move_signal(24);
//}

//void Thread_ImageHandle::RecvScanSignal(){
//    this->start();
//}

//void Thread_ImageHandle::RecvResetSignal(){
//    reset_signal(23);
//}

void Thread_ImageHandle::stopimmedqiately(){
    QMutexLocker locker(&m_lock);
    m_isCanRun = false;
    qDebug() << ">> stopimmedqiately.";
}

void Thread_ImageHandle::Delay_Ms(unsigned int msec){
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    loop.exec();
}

int Thread_ImageHandle::avgGray(cv::Mat img)
{
    int pxl = 0;
    int sum = 0;
    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            sum = sum + img.at<uchar>(i, j);
            pxl++;
        }
    }
    int gray = sum / pxl;
    return gray;
}

float Thread_ImageHandle::count(cv::Mat src)
{
    int black = 0;
    int sum = 0;
    float result = 0;
    for (int i = 0; i < src.rows; i++){
        for (int j = 0; j < src.cols; j++){
            sum++;
            if (src.ptr<uchar>(i)[j] == 0){
                black++;
            }
        }
    }
    result = black*1.0000 / sum;
    return result;
}

void Thread_ImageHandle::AdaptiveThereshold(cv::Mat src, cv::Mat &dst)
{
    //cvtColor(src, dst, CV_BGR2GRAY);
    src.copyTo(dst);
    int x1, y1, x2, y2;
    int count = 0;
    long long sum = 0;
    int S = src.rows >> 3;  //Ṡ®ṖÖÇøÓòṁÄṀóŴḂS*S
    int T = Adaptive_thr;         /*ḞÙṖÖḟÈ£ỲÓÃÀṀṪîẃóÓëãŴÖṁṁÄḟÈẄÏḂ£Ô­ÎÄ£ẃIf the value of the current pixel is t percent less than this average
                        then it is set to black, otherwise it is set to white.*/
    int W = dst.cols;
    int H = dst.rows;
    long long **Argv;
    Argv = new long long*[dst.rows];
    for (int ii = 0; ii<dst.rows; ii++){
        Argv[ii] = new long long[dst.cols];
    }

    for (int i = 0; i<W; i++){
        sum = 0;
        for (int j = 0; j<H; j++){
            sum += dst.at<uchar>(j, i);
            if (i == 0)
                Argv[j][i] = sum;
            else
                Argv[j][i] = Argv[j][i - 1] + sum;
        }
    }

    for (int i = 0; i<W; i++){
        for (int j = 0; j<H; j++){
            x1 = i - S / 2;
            x2 = i + S / 2;
            y1 = j - S / 2;
            y2 = j + S / 2;
            if (x1<0)
                x1 = 0;
            if (x2 >= W)
                x2 = W - 1;
            if (y1<0)
                y1 = 0;
            if (y2 >= H)
                y2 = H - 1;
            count = (x2 - x1)*(y2 - y1);
            sum = Argv[y2][x2] - Argv[y1][x2] - Argv[y2][x1] + Argv[y1][x1];


            if ((long long)(dst.at<uchar>(j, i)*count)<(long long)sum*(100 - T) / 100)
                dst.at<uchar>(j, i) = 0;
            else
                dst.at<uchar>(j, i) = 255;
        }
    }
    for (int i = 0; i < dst.rows; ++i){
        delete[] Argv[i];
    }
    delete[] Argv;
}

void Thread_ImageHandle::CountAdaptive(cv::Mat frame, cv::Mat& binaryImg, cv::Mat& inBinaryImg, float& result1, float& result2)
{
    //frame = frame(Rect(frame.cols * 2 / 5, frame.rows * 2 / 5, frame.cols * 1 / 5, frame.rows * 1 / 5));//ẄØÈḂROI

    AdaptiveThereshold(frame, binaryImg);//ẅÖĠṡ¶ŷÖṁ
    inBinaryImg = 255 - frame;
    AdaptiveThereshold(inBinaryImg, inBinaryImg);//ẅÖĠṡ¶ŷÖṁ
    inBinaryImg = 255 - inBinaryImg;

    result1 = count(binaryImg) * 100;
    result2 = count(inBinaryImg) * 100;
}

void Thread_ImageHandle::CountOtsu(cv::Mat frame, cv::Mat& binaryImg, float& result)
{
    //frame = frame(Rect(frame.cols * 2 / 5, frame.rows * 2 / 5, frame.cols * 1 / 5, frame.rows * 1 / 5));//ẄØÈḂROI
    threshold(frame, binaryImg, 0, 255, cv::THRESH_OTSU);//otsu¶ŷÖṁ
    result = count(binaryImg) * 100;
}

void Thread_ImageHandle::filtrate(cv::Mat frame, cv::Mat& result_binary, float& result)
{
        qDebug()<<"handle_filtrate";
        float result_Otsu = 0;
        float result_Adaptive1 = 0;
        float result_Adaptive2 = 0;
        //float result_CVAdaptive1 = 0;
        //float result_CVAdaptive2 = 0;
        Mat OtsuImg;
        Mat AdaptiveImg1;
        Mat AdaptiveImg2;
        //Mat CVAdaptiveImg1;
        //Mat CVAdaptiveImg2;
        int average_gray = avgGray(frame);

        CountOtsu(frame, OtsuImg, result_Otsu);
        CountAdaptive(frame, AdaptiveImg1, AdaptiveImg2, result_Adaptive1, result_Adaptive2);
        //CountCVAdaptive(frame, CVAdaptiveImg1, CVAdaptiveImg2, result_CVAdaptive1, result_CVAdaptive2);
        //输出过程
        if (show_thr == 1)
        {
            imshow("OtsuImg", OtsuImg);
            imshow("AdaptiveImg1", AdaptiveImg1);
            imshow("AdaptiveImg2", AdaptiveImg2);
            //imshow("CVAdaptiveImg1", CVAdaptiveImg1);
            //imshow("CVAdaptiveImg2", CVAdaptiveImg2);
            cout << "average_gray:" << average_gray << endl;
            cout << "ResultOtsu:" << result_Otsu << "%" << endl;
            cout << "ResultAdaptive1:" << result_Adaptive1 << "%" << endl;
            cout << "ResultAdaptive2:" << result_Adaptive2 << "%" << endl;
            //cout << "ResultCVAdaptive1:" << result_CVAdaptive1 << "%" << endl;
            //cout << "ResultCVAdaptive2:" << result_CVAdaptive2 << "%" << endl;
        }

        /***************筛选输出结果******************/
        if (abs(result_Adaptive1 - result_Otsu) < 10 && abs(result_Adaptive2 - result_Otsu) >= 10)
        {
            if (average_gray >= mean_Gray_thr && result_Adaptive1 < 13)
            {
                result_binary = AdaptiveImg1;
                result = result_Adaptive1;
            }
            else
            {
                result_binary = OtsuImg;
                result = result_Otsu;
            }
        }
        else if (abs(result_Adaptive2 - result_Otsu) < 10 && abs(result_Adaptive1 - result_Otsu) >= 10)
        {
            if (average_gray < mean_Gray_thr && result_Adaptive2 > 94)
            {
                result_binary = AdaptiveImg2;
                result = result_Adaptive2;
            }
            else
            {
                result_binary = OtsuImg;
                result = result_Otsu;
            }
        }
        else
        {
            if (average_gray < mean_Gray_thr && result_Adaptive2 > 94)
            {
                result_binary = AdaptiveImg2;
                result = result_Adaptive2;
            }
            else if (average_gray >= mean_Gray_thr && result_Adaptive1 < 13)
            {
                result_binary = AdaptiveImg1;
                result = result_Adaptive1;
            }
            else
            {
                result_binary = OtsuImg;
                result = result_Otsu;
            }
        }

        /*补偿结果*/
        if (fix_thr == 1)
        {
            if (result >= 13 && result <= 50)
            {
                result = result_Otsu + 0.00176*(result_Otsu - 20.77)*(result_Otsu - 20.77) - 2.2;
            }
            else if (result > 50 && result <= 70)
            {
                result = result_Otsu -0.65;
            }
            else if (result > 70 && result <= 90)
            {
                result = result_Otsu + 0.00157*(result_Otsu - 69.7)*(result_Otsu - 69.7) - 0.647;
            }

        }
}

//计算网点间距与角度
void Thread_ImageHandle::count_distance_angle(Mat src, float binary_result, float &mean_distance, float &mean_angle)
{
    /***************************************提取轮廓**********************************************/
    Mat result_binary2;
    if (binary_result < 50)
    {
        result_binary2 = 255 - src;
    }
    else
    {
        result_binary2 = src;
    }
    vector<vector<Point>>contours;
    vector<Vec4i>hierarchy;
    vector<Point>center_contours;
    findContours(result_binary2, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
    Mat dst = Mat::zeros(result_binary2.size(), CV_8UC3);
    RNG rng(1);
    for (int i = 0; i < contours.size(); i++) {
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        drawContours(dst, contours, i, color, 1, 8, hierarchy, 0, Point(0, 0));
    }

    /***************************************提取质心**********************************************/
    vector<Moments> mu(contours.size());
    for (int i = 0; i < contours.size(); i++)
    {
        mu[i] = moments(contours[i], false);
    }
    //计算轮廓的质心
    vector<Point2f> mc(contours.size());
    for (int i = 0; i < contours.size(); i++)
    {
        mc[i] = Point2d(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);

    }
    //画轮廓及其质心并显示
    Mat drawing = Mat::zeros(result_binary2.size(), CV_8UC3);
    for (int i = 0; i < contours.size(); i++)
    {
        circle(drawing, mc[i], 0, Scalar(255, 255, 255), -1, 8, 0);

    }

    //imshow("drawing", drawing);
    /***************************************计算点间距均值**********************************************/
    //计算所有点与邻域最近四点距离与角度并存入向量
    vector<float> v_angle;
    vector<float> v_distance;
    for (int i = 0; i < contours.size(); i++)
    {
        float neihbor_distance[4] = { 999,999,999,999 };
        float neihbor_angle[4] = { 0,0,0,0 };

        for (int j = 0; j < contours.size(); j++)
        {
            if (j != i)
            {
                float distance = sqrt((mc[i].x - mc[j].x)*(mc[i].x - mc[j].x) + (mc[i].y - mc[j].y)*(mc[i].y - mc[j].y));
                float diff = 0;
                int number = -1;
                for (int x = 0; x < 4; x++)
                {
                    if (neihbor_distance[x] - distance > diff)
                    {
                        number = x;
                        diff = neihbor_distance[x] - distance;
                    }

                }
                if (number >= 0)
                {
                    neihbor_distance[number] = distance;
                    if (mc[i].x - mc[j].x == 0)
                    {
                        neihbor_angle[number] = 90;
                    }
                    else
                    {
                        neihbor_angle[number] = atan(-((mc[i].y - mc[j].y) / (mc[i].x - mc[j].x))) * 180 / 3.14;
                    }
                    if (neihbor_angle[number] < -85)
                    {
                        neihbor_angle[number] += 180;
                    }
                }

            }

        }
        for (int x = 0; x < 4; x++)
        {
            v_distance.push_back(neihbor_distance[x]);
            v_angle.push_back(neihbor_angle[x]);
        }

    }

    //对所有距离聚类
    vector<vector<float>> distance_cluster;
    vector<float> sum_distance_cluster;
    for (int i = 0; i < v_distance.size(); i++)
    {
        //cout << v_distance[i] << endl;
        if (distance_cluster.size() == 0)
        {
            vector<float>aaa;
            aaa.push_back(v_distance[i]);
            sum_distance_cluster.push_back(v_distance[i]);
            distance_cluster.push_back(aaa);
        }
        else
        {
            bool new_v = 1;
            for (int x = 0; x < sum_distance_cluster.size(); x++)
            {
                if (abs(v_distance[i] - (sum_distance_cluster[x] / distance_cluster[x].size())) < 1)
                {
                    distance_cluster[x].push_back(v_distance[i]);
                    new_v = 0;
                    sum_distance_cluster[x] += v_distance[i];
                    break;
                }
            }
            if (new_v == 1)
            {
                vector<float>aaa;
                aaa.push_back(v_distance[i]);
                sum_distance_cluster.push_back(v_distance[i]);
                distance_cluster.push_back(aaa);
            }
        }

    }

    ////打印所有聚类情况
    //for (int i = 0; i < distance_cluster.size(); i++)
    //{
    //	cout << distance_cluster[i].size() << endl;
    //	for (int j = 0; j < distance_cluster[i].size(); j++)
    //	{
    //		cout << distance_cluster[i][j] << endl;
    //	}
    //	cout << endl;
    //}

    //对所有角度聚类
    vector<vector<float>> angle_cluster;
    vector<float> sum_angle_cluster;
    for (int i = 0; i < v_angle.size(); i++)
    {
        //cout << v_angle[i] << endl;
        if (angle_cluster.size() == 0)
        {
            vector<float>aaa;
            aaa.push_back(v_angle[i]);
            sum_angle_cluster.push_back(v_angle[i]);
            angle_cluster.push_back(aaa);
        }
        else
        {
            bool new_v = 1;
            for (int x = 0; x < sum_angle_cluster.size(); x++)
            {
                if (abs(v_angle[i] - (sum_angle_cluster[x] / angle_cluster[x].size())) < 3)
                {
                    angle_cluster[x].push_back(v_angle[i]);
                    new_v = 0;
                    sum_angle_cluster[x] += v_angle[i];
                    break;
                }
            }
            if (new_v == 1)
            {
                vector<float>aaa;
                aaa.push_back(v_angle[i]);
                sum_angle_cluster.push_back(v_angle[i]);
                angle_cluster.push_back(aaa);
            }
        }

    }

    ////打印所有聚类情况
    //for (int i = 0; i < angle_cluster.size(); i++)
    //{
    //	cout << angle_cluster[i].size() << endl;
    //	for (int j = 0; j < angle_cluster[i].size(); j++)
    //	{
    //		cout << angle_cluster[i][j] << endl;
    //	}
    //	cout << endl;
    //}


    //取值最多的距离聚类向量求平均值
    int num_max_size_distance = 0;
    int i_max_size_distance = 0;
    for (int i = 0; i < distance_cluster.size(); i++)
    {
        if (distance_cluster[i].size() > num_max_size_distance)
        {
            i_max_size_distance = i;
            num_max_size_distance = distance_cluster[i].size();
        }

    }

    if (distance_cluster.size() > 0) {
        mean_distance = sum_distance_cluster[i_max_size_distance] / distance_cluster[i_max_size_distance].size();
    }
    if (mean_distance == 999)
    {
        mean_distance = 0;
    }

    //取值最多的角度聚类向量求平均值
    int num_max_size_angle = 0;
    int i_max_size_angle = 0;
    for (int i = 0; i < angle_cluster.size(); i++)
    {
        if (angle_cluster[i].size() > num_max_size_angle && sum_angle_cluster[i] / angle_cluster[i].size() >= 0 && sum_angle_cluster[i] / angle_cluster[i].size() <= 90)
        {
            i_max_size_angle = i;
            num_max_size_angle = angle_cluster[i].size();
        }

    }

    if (angle_cluster.size() > 0)
    {
        mean_angle = sum_angle_cluster[i_max_size_angle] / angle_cluster[i_max_size_angle].size();
    }

    if (show_thr == 1)
    {
        imshow("contours", dst);
    }

}



