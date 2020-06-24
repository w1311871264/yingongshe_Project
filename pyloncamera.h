#ifndef PYLONCAMERA_H
#define PYLONCAMERA_H
#include <pylon/PylonIncludes.h>
#include <opencv2/opencv.hpp>
#include <QDebug>
#include <QImage>
#include <pylon/gige/BaslerGigEInstantCamera.h>

typedef Pylon::CBaslerGigEInstantCamera Camera_t;
using namespace Basler_GigECameraParams;

using namespace Pylon;
using namespace cv;
using namespace std;

class PylonCamera
{
public:
    Mat cvRaw_2D;
    bool Camera2D_status;
    Camera_t camera;
    CGrabResultPtr ptrGrabResult2D;
    CPylonImage PyImg2D;

    PylonCamera();

    int Camera2DExposureTime = 2700;
    int CameraPacketSize = 1500;
    int CameraPacketDelay = 2300;

public:
    bool camera_init();
    void SetupCamera(Camera_t &camera);

    Mat  GetCameraImage(Camera_t& camera, CGrabResultPtr& ptrGrabResult,CPylonImage& PyImg);
    bool checkCamera();


    QImage Mat2QImage(Mat const& src);
    Mat  QImage2Mat(QImage const& src);

private:
    CInstantCameraArray cameras;
    CGrabResultPtr ptrGrabResultLaser;
    CImageFormatConverter FormatConverter2D;

    int Camera2D_num;

};

#endif // PYLONCAMERA_H
