#include <jni.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/aruco.hpp>
#include <android/log.h>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

Ptr<aruco::DetectorParameters> detectorParams;
Ptr<aruco::Dictionary> dictionary;

static void readDetectorParameters(Ptr<aruco::DetectorParameters> &params);

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

    void JNICALL Java_com_example_esalman17_aruco_1demo_MainActivity_detectMarker(JNIEnv *env, jobject instance, jlong matAddrInput) {
        Mat &image = *(Mat *) matAddrInput;
        cvtColor(image, image, cv::COLOR_BGRA2BGR);
        vector< int > ids;
        vector< vector< Point2f > > corners, rejected;

        aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejected);
        aruco::drawDetectedMarkers(image, corners, ids);
        for(unsigned int i = 0; i < ids.size(); i++){
            LOGD("Marker found: id=%d",ids[i]);
        }

    }

    void JNICALL Java_com_example_esalman17_aruco_1demo_MainActivity_initializeDetector(JNIEnv *env, jobject instance) {
        detectorParams = aruco::DetectorParameters::create();
        //readDetectorParameters(detectorParams);
        detectorParams->cornerRefinementMethod = aruco::CORNER_REFINE_SUBPIX; // do corner refinement in markers

        int dictionaryId = 1;
        dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
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

