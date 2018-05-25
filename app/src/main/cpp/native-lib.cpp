#include <jni.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/aruco.hpp>
#include <android/log.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;

int dictionaryId = 16;
float markerLength = 0.033;
float cam[9] = {3.7912298852212243e+03, 0., 2.1272262488243086e+03, 0.,
                3.7912298852212243e+03, 1.5298569688578571e+03, 0., 0., 1. };
float dist[5] = {3.4790498057714760e-02, 1.4058937642379707e-01,
                 -3.7512327058658646e-03, 1.0158828596242453e-03,
                 -2.3169471161601113e-01};

Mat camMatrix = Mat(3, 3, CV_32F, cam);
Mat distCoeffs = Mat(5, 1, CV_32F, dist);
Ptr<aruco::DetectorParameters> detectorParams;
Ptr<aruco::Dictionary> dictionary;

static void readDetectorParameters(Ptr<aruco::DetectorParameters> &params);
bool isRotationMatrix(Mat &R);
Vec3f rotationMatrixToEulerAngles(Mat &R);

extern "C" {
    JNIEXPORT jstring
    #define  LOG_TAG    "NativeClass_elif"
    #define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
    #define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


    JNICALL
    Java_com_example_esalman17_aruco_1demo_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */) {
        std::string hello = "Hello from C++";
        return env->NewStringUTF(hello.c_str());
    }

    jfloatArray JNICALL Java_com_example_esalman17_aruco_1demo_MainActivity_detectMarker(JNIEnv *env, jobject instance, jlong matAddrInput) {
        Mat &image = *(Mat *) matAddrInput;
        cvtColor(image, image, cv::COLOR_BGRA2BGR);
        vector< int > ids;
        vector< vector< Point2f > > corners, rejected;
        vector< Vec3d > rvecs, tvecs;

        aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejected);
        aruco::drawDetectedMarkers(image, corners, ids);
        if(ids.size() > 0) {
            aruco::estimatePoseSingleMarkers(corners, markerLength, camMatrix, distCoeffs, rvecs, tvecs);
        }
        Mat rotM(3,3,CV_64FC1,0.0);
        float res[7]={};
        for(unsigned int i = 0; i < ids.size(); i++){
            if(ids[i] == 0 || ids[i] == 1023) continue;
            Rodrigues(rvecs[i],rotM);
            Vec3f vec = rotationMatrixToEulerAngles(rotM);
            //LOGD("id=%d\tdistance=%.1fcm\troll,pitch,yaw=%.1f,%.1f,%.1f",ids[i],tvecs[i][2]*100,vec[0],vec[1],vec[2]);
            res[0] = ids[i];
            res[1] = tvecs[i][0];
            res[2] = tvecs[i][1];
            res[3] = tvecs[i][2];
            res[4] = vec[0];
            res[5] = vec[1];
            res[6] = vec[2];
            break; // Take only 1 marker
        }

        jfloatArray result = env->NewFloatArray(7);
        env->SetFloatArrayRegion(result, 0, 7, res);
        return result;
    }

    void JNICALL Java_com_example_esalman17_aruco_1demo_MainActivity_initializeDetector(JNIEnv *env, jobject instance, jint width, jint height) {
        detectorParams = aruco::DetectorParameters::create();
        detectorParams->cornerRefinementMethod = aruco::CORNER_REFINE_SUBPIX; // do corner refinement in markers
        dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
        float scalex = (float)width / 4208;
        float scaley = (float)height / 3120;
        camMatrix.at<float>(0,0) *= scalex;
        camMatrix.at<float>(0,2) *= scalex;
        camMatrix.at<float>(1,1) *= scaley;
        camMatrix.at<float>(1,2) *= scaley;
        LOGD("Detector initialized");
    }

}



static void readDetectorParameters(Ptr<aruco::DetectorParameters> &params){
    params->adaptiveThreshWinSizeMin = 3;
    params->adaptiveThreshWinSizeMax = 23;
    params->adaptiveThreshWinSizeStep = 10;
    params->adaptiveThreshConstant=7;
    params->minMarkerPerimeterRate=0.03;
    params->maxMarkerPerimeterRate=4.0;
    params->polygonalApproxAccuracyRate=0.05;
    params->minCornerDistanceRate=10.0;
    params->minDistanceToBorder=3;
    params->minMarkerDistanceRate=10.0;
    params->minMarkerDistanceRate=0.05;
    params->cornerRefinementMethod=0;
    params->cornerRefinementWinSize=5;
    params->cornerRefinementMaxIterations=30;
    params->cornerRefinementMinAccuracy=0.1;
    params->markerBorderBits=1;
    params->perspectiveRemovePixelPerCell=4;
    params->perspectiveRemoveIgnoredMarginPerCell=0.13;
    params->maxErroneousBitsInBorderRate=0.04;
    params->minOtsuStdDev=5.0;
    params->errorCorrectionRate=0.6;
}

// Checks if a matrix is a valid rotation matrix.
bool isRotationMatrix(Mat &R)
{
    Mat Rt;
    transpose(R, Rt);
    Mat shouldBeIdentity = Rt * R;
    Mat I = Mat::eye(3,3, shouldBeIdentity.type());

    return  norm(I, shouldBeIdentity) < 1e-6;

}

// Calculates rotation matrix to euler angles
// The result is the same as MATLAB except the order
// of the euler angles ( x and z are swapped ).
Vec3f rotationMatrixToEulerAngles(Mat &R)
{
    bool b = isRotationMatrix(R);
    if(!b) LOGE("Not a rotation matrix");
    assert(b);

    float sy = sqrt(R.at<double>(0,0) * R.at<double>(0,0) +  R.at<double>(1,0) * R.at<double>(1,0) );

    bool singular = sy < 1e-6; // If
    float pi = 3.14159265358979323846;

    float x, y, z;
    if (!singular)
    {
        x = atan2(R.at<double>(2,1) , R.at<double>(2,2));
        y = atan2(-R.at<double>(2,0), sy);
        z = atan2(R.at<double>(1,0), R.at<double>(0,0));
    }
    else
    {
        x = atan2(-R.at<double>(1,2), R.at<double>(1,1));
        y = atan2(-R.at<double>(2,0), sy);
        z = 0;
    }
    return Vec3f(x/pi*180, y/pi*180, z/pi*180);


}

