package com.example.esalman17.aruco_demo;

import android.Manifest;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.JavaCamera2View;
import org.opencv.android.JavaCameraView;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

public class MainActivity extends AppCompatActivity implements CameraBridgeViewBase.CvCameraViewListener2 {

    private static String TAG = "MainActivity";
    JavaCameraView javaCameraView;
    TextView tvResult;
    Mat matFrame;
    boolean loaded = false;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    //Native functions
    public native String stringFromJNI();
    public native float[] detectMarker(long matAddrGray);
    public native void initializeDetector(int width, int height);

    BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch(status){
                case BaseLoaderCallback.SUCCESS:
                    Log.i(TAG, "OpenCV loaded successfully");
                    javaCameraView.setCameraIndex(0);
                    loaded = true;
                    break;
                default:
                    super.onManagerConnected(status);
                    break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setRequestedOrientation (ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        // Permissions for Android 6+
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.CAMERA}, 1);

        javaCameraView = findViewById(R.id.java_camera_view);
        javaCameraView.setVisibility(View.VISIBLE);
        javaCameraView.enableFpsMeter();
        javaCameraView.setCvCameraViewListener(this);
        tvResult = findViewById(R.id.textView);

        findViewById(R.id.buttonStart).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(loaded) {
                    javaCameraView.enableView();
                }
            }
        });
        findViewById(R.id.buttonStop).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(loaded) {
                    javaCameraView.disableView();
                }
            }
        });


    }

    @Override
    protected void onResume() {
        super.onResume();
        if(OpenCVLoader.initDebug()){
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
            Log.i(TAG, "OpenCV loaded successfully");
        }else{
            Log.i(TAG, "OpenCV cannot be loaded ");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_3_0, this, mLoaderCallback );
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if(javaCameraView != null){
            javaCameraView.disableView();
        }
    }



    // Opencv CameraBridgeViewBase methods -----------------------------------------------------------
    @Override
    public void onCameraViewStarted(int width, int height) {
        matFrame = new Mat(width, height, CvType.CV_8UC1);
        Log.i(TAG, "CameraView: width="+width+" height="+height);
        initializeDetector(width, height);
    }

    @Override
    public void onCameraViewStopped() {
        if(matFrame != null) {
            matFrame.release();
        }
    }

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        matFrame = inputFrame.rgba();
        float result[] = detectMarker(matFrame.getNativeObjAddr());
        if(result != null){
            final String text = String.format("id:%d\nx=%.1f cm\ny=%.1f cm\nz=%.1f cm\nroll=%.1f\npitch=%.1f\nyaw=%.1f",
                    (int)result[0], result[1]*-100, result[2]*100, result[3]*100, result[4], result[5], result[6]);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    tvResult.setText(text);
                }
            });
        }
        return matFrame;
    }
    // -----------------------------------------------------------------------------------------------


    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        switch (requestCode) {
            case 1: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                } else {
                    Toast.makeText(MainActivity.this, "Permission denied to read your External storage", Toast.LENGTH_SHORT).show();
                }
                return;
            }
        }
    }
}
