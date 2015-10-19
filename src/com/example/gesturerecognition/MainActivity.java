package com.example.gesturerecognition;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

import android.R.integer;
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.TextView;

public class MainActivity extends Activity implements CvCameraViewListener2 {
	
	private static final String    TAG = "GestureRecognition";
	private CameraBridgeViewBase   mOpenCvCameraView;
	private Mat                    mCamera;
	private TextView               textView;
	
	//OpenCV类库加载并初始化成功后的回调函数，在此我们不进行任何操作
    private BaseLoaderCallback  mLoaderCallback = new BaseLoaderCallback(this) {
       @Override
       public void onManagerConnected(int status) {
           switch (status) {
               case LoaderCallbackInterface.SUCCESS:{
               	System.loadLibrary("Recognize");
               	mOpenCvCameraView.enableView();
               } break;
               default:{
                   super.onManagerConnected(status);
               } break;
           }
       }
   };
   
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.cameraView);
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(this);
        
        textView=(TextView)findViewById(R.id.textView1);
    }
    
    public void onClick(View v){
    	int result=Recognize(mCamera.nativeObj);
    	textView.setText("Result: "+result);
    }
    
    @Override
    public void onResume(){
    	super.onResume();
        if (!OpenCVLoader.initDebug()) {
            Log.d(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback);
        } else {
            Log.d(TAG, "OpenCV library found inside package. Using it!");
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }
    }
    
    public void onDestroy() {
        super.onDestroy();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

	@Override
	public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
		// TODO Auto-generated method stub
//		int w = inputFrame.rgba().width(), h = inputFrame.rgba().height();
//		Bitmap bmp=Bitmap.createBitmap(w, h, Config.RGB_565);
//		Utils.matToBitmap(inputFrame.rgba(), bmp);
//		int[] pix = new int[w * h];
//		bmp.getPixels(pix, 0, w, 0, 0, w, h);
//		int[] resultInt = Recognize(pix, w, h);
//		Bitmap resultImg = Bitmap.createBitmap(w, h, Config.RGB_565);
//		resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);
//		Mat mat=new Mat();
//		Utils.bitmapToMat(resultImg, mat);
//		
//		return mat;
		
		
		
		mCamera=inputFrame.rgba();
		
		return mCamera;
	}

	@Override
	public void onCameraViewStarted(int width, int height) {
		// TODO Auto-generated method stub
		mCamera = new Mat(height, width, CvType.CV_8UC4);
	}

	@Override
	public void onCameraViewStopped() {
		// TODO Auto-generated method stub
		mCamera.release();
	}
	
	//public native int[] Recognize(int[] buf, int w, int h);
	public native int Recognize(long matPtr);
}
