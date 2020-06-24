#include "pyloncamera.h"

PylonCamera::PylonCamera()
{
    //   qDebug() << "camera_init: PylonInitialize." ;
       Camera2D_status = 0;
       Camera2D_num = 0;
       FormatConverter2D.OutputPixelFormat = PixelType_BGR8packed;
}

bool PylonCamera::camera_init()
{
    qDebug() << "camera_init: PylonInitialize." ;

    int exitCode = 0;
    PylonInitialize();
    try
    {
        // Only look for cameras supported by Camera_t.
        CDeviceInfo info;
        info.SetDeviceClass( Camera_t::DeviceClass());

        // Create an instant camera object with the first found camera device that matches the specified device class.
        camera.Attach(CTlFactory::GetInstance().CreateFirstDevice( info));

        // Print the name of the used camera.
        cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;

        // Register the standard event handler for configuring single frame acquisition.
        // This overrides the default configuration as all event handlers are removed by setting the registration mode to RegistrationMode_ReplaceAll.
        // Please note that the camera device auto functions do not require grabbing by single frame acquisition.
        // All available acquisition modes can be used.
        //camera.RegisterConfiguration( new CAcquireSingleFrameConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);

        // Open the camera.
        camera.Open();
        // Turn test image off.
        camera.TestImageSelector = TestImageSelector_Off;

        qDebug() << "检测到2D相机.";
        //设置相机参数
        SetupCamera(camera);
        //启动拍照
        camera.StartGrabbing(GrabStrategy_LatestImageOnly);
        qDebug() << "Camer Init is successful.";
    }
    catch (const TimeoutException &e)
    {
        // Auto functions did not finish in time.
        // Maybe the cap on the lens is still on or there is not enough light.
        cerr << "A timeout has occurred." << endl
             << e.GetDescription() << endl;
        cerr << "Please make sure you remove the cap from the camera lens before running this sample." << endl;
        exitCode = 0;
        return false;
    }
    catch (const GenericException &e)
    {
        // Error handling.
        cerr << "An exception occurred." << endl
             << e.GetDescription() << endl;
        exitCode = 1;
        return false;
    }
    return true;
}

void PylonCamera::SetupCamera(Camera_t &camera)
{
    int i = 0;
    try{
        qDebug() << ">> before set camera.GevSCBWA.GetValue : "<< camera.GevSCBWR.GetValue();
        camera.ExposureTimeAbs.SetValue(Camera2DExposureTime);
        camera.GevSCPSPacketSize.SetValue(CameraPacketSize);
        camera.GevSCPD.SetValue(CameraPacketDelay);
        camera.GevSCBWR.SetValue(20);
        camera.GevSCBWRA.SetValue(1);
        qDebug() << "camera.ExposureTimeAbs : "<< camera.ExposureTimeAbs.GetValue();
        qDebug() << ">> after set camera.GevSCBWA.GetValue : "<< camera.GevSCBWR.GetValue();
        qDebug() << "camera.GevSCPD : " <<camera.GevSCPD.GetValue();
        qDebug() << "camera.GevSCPSPacketSize : " <<camera.GevSCPSPacketSize.GetValue();
    }
    catch(GenericException &e){
        qDebug()<<"set config error";
    }
}

Mat PylonCamera::GetCameraImage(Camera_t &camera, CGrabResultPtr &ptrGrabResult, CPylonImage &PyImg)
{
    Mat cvImg;
    if (camera.IsGrabbing()) {
        //qDebug() << "is Grabbing";
        camera.RetrieveResult(3000, ptrGrabResult, TimeoutHandling_ThrowException);
        if (ptrGrabResult->GrabSucceeded()) {
            //qDebug() << "grab_succed";
            FormatConverter2D.Convert(PyImg, ptrGrabResult);
            cvImg = Mat((int)(ptrGrabResult->GetHeight()), (int)ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)PyImg.GetBuffer());
        }
        else {
            qDebug() << "Grab the image failed.";
        }
    }
    return cvImg;
}

QImage PylonCamera::Mat2QImage(const Mat &src)
{
    Mat temp;
    if(!src.empty())
        cvtColor(src, temp,COLOR_BGR2RGB); // cvtColor Makes a copt, that what i need
    QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888); //temp.setp()没有时，会导致有些图片在转换后倾斜
    dest.bits(); // enforce deep copy, see documentation
    // of QImage::QImage ( const uchar * data, int width, int height, Format format )
    return dest;
}

Mat PylonCamera::QImage2Mat(const QImage &src)
{
    cv::Mat tmp(src.height(),src.width(),CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
    cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
    if(!tmp.empty())
        cvtColor(tmp, result,COLOR_BGR2RGB);
    return result;
}





