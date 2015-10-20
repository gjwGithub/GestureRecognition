#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
using namespace cv;
IplImage * change4channelTo3InIplImage(IplImage * src);




#include <cv.h>
#include <highgui.h>

#include <vector>
#include <android/log.h>
using namespace std;

CvSize newSize;
CvSize sz;



IplImage *tmp1;
IplImage *tmp2;
IplImage *tmp3;
IplImage *src2;
IplImage *src1;



IplImage* smooth1;
IplImage* smooth2;
IplImage* smooth3;
IplImage* smooth4;
IplImage* smooth5;



IplImage *img_YCrCb = 0;
IplImage *YCrCb;
IplImage *YCrCb_mask;
IplImage *Y_channel, *Cr_channel, *Cb_channel;
IplImage *Y_cmp, *Cr_cmp, *Cb_cmp;

CvScalar Y_lower;
CvScalar Y_upper;

CvScalar Cr_lower;
CvScalar Cr_upper;

CvScalar Cb_lower;
CvScalar Cb_upper;

CvScalar YCrCb_lower;
CvScalar YCrCb_upper;



IplImage* hsv_image;
IplImage* hsv_mask;
CvScalar  hsv_min;
CvScalar  hsv_max;

IplImage *H_img, *S_img, *V_img;
IplImage *H_mask, *H_mask1, *S_mask, *S_mask1, *V_mask, *V_mask1, *V_mask2;



IplImage* laplace = 0;
IplImage* colorlaplace = 0;
IplImage* planes[3] = { 0, 0, 0 };



IplImage*    tmp_img = 0;
CvMemStorage*  storage_tmp = 0;

CvSeq* handT = 0;
CvSeq* handT1 = 0;
CvSeq* handT2 = 0;

int handTNum = 10;//10��ģ��

char *tmp_names[] = { "/mnt/sdcard/Gestures/1.bmp","/mnt/sdcard/Gestures/2.bmp","/mnt/sdcard/Gestures/3.bmp","/mnt/sdcard/Gestures/4.bmp","/mnt/sdcard/Gestures/5.bmp","/mnt/sdcard/Gestures/6.bmp","/mnt/sdcard/Gestures/7.bmp","/mnt/sdcard/Gestures/8.bmp","/mnt/sdcard/Gestures/9.bmp","/mnt/sdcard/Gestures/10.bmp" };



int pmsf_value = 5;//��ֵƮ�ݷָ�ƽ��ϵ��
int MopEx_value = 2;//������
int Hmatch_value = 25;//ģ��ƥ��ϵ��



//����
int V_low = 30;
int V_high = 250;
//���Ͷ�
int S_low = 40;
int S_high = 170;
//ɫ��
int H_low_max = 40;
int H_high_min = 100;
int if_high_light = 1; //�Ƿ�߹ⲹ��



CvMemStorage*  g_storage = NULL;

CvSeq* seqMidObj = 0;//��ѡ�����������
int handNum = 0;



bool if_match_num = false;
int match_num = -1;

void init_hand_YCrCb()
{
	img_YCrCb = cvCreateImage(sz, 8, 3);
	YCrCb_mask = cvCreateImage(sz, IPL_DEPTH_8U, 1);;

	//���յ�ͼƬ
	YCrCb = cvCreateImage(sz, IPL_DEPTH_8U, 3);


	//��ͨ��
	Y_channel = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	Cr_channel = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	Cb_channel = cvCreateImage(sz, IPL_DEPTH_8U, 1);

	//����Χ��ȡ��
	Y_cmp = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	Cr_cmp = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	Cb_cmp = cvCreateImage(sz, IPL_DEPTH_8U, 1);

	//Y,Cr,Cb����ɫ��Χ
	Y_lower = CV_RGB(0, 0, 130);
	Y_upper = CV_RGB(0, 0, 130);

	Cr_lower = CV_RGB(0, 0, 125);
	Cr_upper = CV_RGB(0, 0, 125);

	Cb_lower = CV_RGB(0, 0, 132);
	Cb_upper = CV_RGB(0, 0, 147);

	YCrCb_lower = cvScalar(0, 0, 132, 0);
	YCrCb_upper = cvScalar(130, 125, 147, 0);
}

void init_hand_HSV()
{
	hsv_image = cvCreateImage(sz, 8, 3);
	hsv_mask = cvCreateImage(sz, 8, 1);
	hsv_min = cvScalar(0, 20, 20, 0);
	hsv_max = cvScalar(20, 250, 255, 0);
	//hsv_mask->origin = 1;

	//����2: �����������ͨ��
	H_img = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	S_img = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	V_img = cvCreateImage(sz, IPL_DEPTH_8U, 1);

	H_mask = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	H_mask1 = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	S_mask = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	S_mask1 = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	V_mask = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	V_mask2 = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	V_mask1 = cvCreateImage(sz, IPL_DEPTH_8U, 1);
}

void init_laplace()
{
	int i;
	for (i = 0; i < 3; i++)
		planes[i] = cvCreateImage(newSize, 8, 1);
	laplace = cvCreateImage(newSize, IPL_DEPTH_16S, 1);
	colorlaplace = cvCreateImage(newSize, 8, 3);
}

void init_hand_template()
{
	storage_tmp = cvCreateMemStorage(0);

	int i = 0;
	for (i = 0; i<handTNum; i++) {

		tmp_img = cvLoadImage(tmp_names[i], CV_LOAD_IMAGE_GRAYSCALE);
		if (!tmp_img) {
			continue;
		}
		handT1 = handT2;
		cvFindContours(tmp_img, storage_tmp, &handT2, sizeof(CvContour), CV_RETR_EXTERNAL);

		if (handT2) {
			if (handT1 == NULL) {
				handT = handT2;
			}
			else {
				handT2->h_prev = handT1;
				handT1->h_next = handT2;
			}

		}
	}
}

void hand_HSV()
{
	cvCvtColor(src2, hsv_image, CV_BGR2HSV);

	//����2: �����������ͨ��
	cvSplit(hsv_image, H_img, S_img, V_img, 0);

	//ɫ��
	cvInRangeS(H_img, cvScalar(0, 0, 0, 0), cvScalar(H_low_max, 0, 0, 0), H_mask);//��ɫ��
	cvInRangeS(H_img, cvScalar(256 - H_high_min, 0, 0, 0), cvScalar(256, 0, 0, 0), H_mask1);//��ɫ��

	//���Ͷ�
	cvInRangeS(S_img, cvScalar(S_low, 0, 0, 0), cvScalar(S_high, 0, 0, 0), S_mask); //�м���

	//����
	cvInRangeS(V_img, cvScalar(V_high, 0, 0, 0), cvScalar(256, 0, 0, 0), V_mask);//������
	cvInRangeS(V_img, cvScalar(V_low, 0, 0, 0), cvScalar(V_high, 0, 0, 0), V_mask1); //�м���

    //���, �����ϵĻ��
	cvOr(H_mask1, H_mask, H_mask, 0);

	//�������Ͷȹ�������
	cvAnd(H_mask, S_mask, H_mask, 0);

	//��ȥ������������
	cvAnd(H_mask, V_mask1, H_mask, 0);

	//������������
	if (if_high_light) { cvOr(H_mask, V_mask, H_mask, 0); }

	//�Ƿ񲹳��ع����
	hsv_mask = H_mask;
}

void reduce_noise()
{
	cvMorphologyEx(hsv_mask, smooth1, 0, NULL, CV_MOP_CLOSE, MopEx_value);
}

void hand_contours(IplImage* dst) {
	if (g_storage == NULL) {
		g_storage = cvCreateMemStorage(0);
	}
	else {
		cvClearMemStorage(g_storage);
	}

	int i = 0, j = 0;
	CvSeq* contours = 0;
	CvSeq* contoursHead = 0;
	CvSeq* p = NULL;
	CvSeq* q = NULL;

	seqMidObj = 0;
	handNum = 0;

	cvFindContours(dst, g_storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL);//ֻ����������
	contoursHead = contours;//contours��ͷ

	cvZero(tmp3);

	if (contours)cvDrawContours(tmp3, contours, cvScalarAll(255), cvScalar(255, 0, 0, 0), 1);//��������

	contours = contoursHead;i = 0;
	CvRect bound;
	int dat = 2;

	//ȥ��С�������
	int contArea = 0;
	int imgArea = newSize.height * newSize.width;

	for (; contours != 0; contours = contours->h_next) {

		i++;

		//��������С, ���ų�
		contArea = fabs(cvContourArea(contours, CV_WHOLE_SEQ));

		if ((double)contArea / imgArea < 0.015) { continue; }

		//����߽��봰������, ���ų�
		bound = cvBoundingRect(contours, 0);

		if (bound.x < dat
			|| bound.y < dat
			|| bound.x + bound.width + dat > newSize.width
			|| bound.y + bound.height + dat > newSize.height)
		{
			continue;
		}

		//������������
		q = p;
		p = contours;

		if (q == NULL) {
			seqMidObj = p;
		}
		else {
			q->h_next = p;
			p->h_prev = q;
		}
		handNum++;

	}

	if (seqMidObj) {
		seqMidObj->h_prev = NULL;
		p->h_next = NULL;
	}
	//if (handNum>0) printf("�ҵ���: %d  ", handNum);

	cvZero(tmp3);
	if (seqMidObj)cvDrawContours(tmp3, seqMidObj, cvScalarAll(255), cvScalar(255, 0, 0, 0), 1);//��������
}

void hand_template_match(CvSeq* handT, CvSeq* hand) {
	int i = 0;
	int kind = -1;
	double hu = 1;

	double hutmp;
	CvSeq* handp = handT;
	int method = CV_CONTOURS_MATCH_I1;

	match_num = 0;

	if (handT == NULL) { return; }
	if (hand == NULL) { return; }

	for (i = 0; i<handTNum; i++) {
		hutmp = cvMatchShapes(handp, hand, method, 0);
		handp = handp->h_next;

		//�ҵ�hu����С��ģ��
		if (hu > hutmp) {
			hu = hutmp;
			kind = i + 1;
		}
	}

	//��ʾƥ����
	if (hu<((double)Hmatch_value) / 100) {
		printf("ƥ��ģ��: %d (%f)", kind, hu);
		match_num = kind;
		if_match_num = true;
	}
	else {
		if_match_num = false;
	}
}






extern "C" {
JNIEXPORT /*jintArray*/jint JNICALL Java_com_example_gesturerecognition_MainActivity_Recognize(
		JNIEnv* env, jobject obj, /*jintArray buf, int w, int h*/ jlong matPtr);

JNIEXPORT /*jintArray*/jint JNICALL Java_com_example_gesturerecognition_MainActivity_Recognize(
		JNIEnv* env, jobject obj, /*jintArray buf, int w, int h*/ jlong matPtr) {

//	jint *cbuf;
//	cbuf = env->GetIntArrayElements(buf, false);
//	if (cbuf == NULL) {
//		return 0;
//	}
//
//	Mat myimg(h, w, CV_8UC4, (unsigned char*) cbuf);
//	IplImage image = IplImage(myimg);
//	IplImage* image3channel = change4channelTo3InIplImage(&image);
//
//	IplImage* pCannyImage = cvCreateImage(cvGetSize(image3channel),
//			IPL_DEPTH_8U, 1);
//
//	cvCanny(image3channel, pCannyImage, 50, 150, 3);
//
//	int* outImage = new int[w * h];
//	for (int i = 0; i < w * h; i++) {
//		outImage[i] = (int) pCannyImage->imageData[i];
//	}
//
//	int size = w * h;
//	jintArray result = env->NewIntArray(size);
//	env->SetIntArrayRegion(result, 0, size, outImage);
//	env->ReleaseIntArrayElements(buf, cbuf, 0);
//	return result;



	Mat* mat = (Mat*)matPtr;
	IplImage src(*mat);

	//double scale = 0.5;
	double scale = 1;

	//���ͼ���С
	sz = cvGetSize(&src);
	newSize.height = (int)(sz.height * scale);
	newSize.width = (int)(sz.width * scale);

	//sz = newSize;

	tmp1 = cvCreateImage(sz, IPL_DEPTH_8U, 1);

	tmp2 = cvCreateImage(sz, IPL_DEPTH_8U, 3);

	tmp3 = cvCreateImage(sz, IPL_DEPTH_8U, 3);

	src2 = cvCreateImage(sz, IPL_DEPTH_8U, 3);

	smooth1 = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	smooth2 = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	smooth3 = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	smooth4 = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	smooth5 = cvCreateImage(sz, IPL_DEPTH_8U, 1);

	init_hand_YCrCb();
	init_hand_HSV();
	init_laplace();

	//����ƥ���ģ��
	init_hand_template();

	src1 = cvCreateImage(sz, IPL_DEPTH_8U, 3);

	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "src %d",src.depth);
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "src1 %d",src1->depth);

	//��СҪ�����ͼ��(��С������)
	cvResize(&src, src1, CV_INTER_LINEAR);
	//src1=&src;

	cvPyrMeanShiftFiltering(src1, src2, pmsf_value, 40, 2);

	//hsvɫ����ȡ
	hand_HSV();

	//����
	reduce_noise();

	//Ѱ��������
	hand_contours(smooth1);

	//Ѱ��ƥ��
	hand_template_match(handT, seqMidObj);


	if (if_match_num)
		return match_num;
	else
		return -1;
}
}

IplImage * change4channelTo3InIplImage(IplImage * src) {
	if (src->nChannels != 4) {
		return NULL;
	}

	IplImage * destImg = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
	for (int row = 0; row < src->height; row++) {
		for (int col = 0; col < src->width; col++) {
			CvScalar s = cvGet2D(src, row, col);
			cvSet2D(destImg, row, col, s);
		}
	}

	return destImg;
}
