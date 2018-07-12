#include "FaceRecognize.h"


//人脸识别主函数
//本函数应于3-5秒内返回，或者使用多线程，以免（过于）阻塞主界面
//@return -1人脸识别失败；0人脸不匹配；1识别成功；
VideoCapture cap;
Ptr<FaceRecognizer> modelPCA;
Mat frame;
int StartFaceRecognize() {
	//modelPCA = LBPHFaceRecognizer::create();
	modelPCA = FisherFaceRecognizer::create();
	int time_all=0;
	int predictPCA = -1;
	Point text_lb;
	ifstream faceFile;
	faceFile.open("d:/faces/dreacter_faces/model/Face_Model.xml");
	if (!faceFile.is_open())
	{
		return -1;
	}
	else
		faceFile.close();
	modelPCA->read("d:/faces/dreacter_faces/model/Face_Model.xml");
	if (cap.isOpened())
		cap.release();     //decide if capture is already opened; if so,close it
	cap.open(0);           //open the default camera
	if (cap.isOpened())
	{
		while (1)
		{
			cap >> frame;
			if (!frame.empty())
			{
				//分配空间失败返回-1,中断识别返回0,识别到返回1,正在检测返回2,存储完成返回3.
				int * pResults = NULL;
				//pBuffer is used in the detection functions.
				//If you call functions in multiple threads, please create one buffer for each thread!
				unsigned char * pBuffer = (unsigned char *)malloc(DETECT_BUFFER_SIZE);
				if (!pBuffer)
				{
					fprintf(stderr, "Can not alloc buffer.\n");
				}
				Mat face;
				Mat gray;
				Mat show;
				cvtColor(frame, gray, CV_BGR2GRAY);//转为灰度图
												   //equalizeHist(gray, gray);
				int doLandmark = 1;// do landmark detection
								   ///////////////////////////////////////////
								   // frontal face detection / 68 landmark detection
								   //正面人脸检测/ 68标志性检测
								   // it's fast, but cannot detect side view faces
								   //它很快，但不能检测侧面的脸
								   //////////////////////////////////////////
				pResults = facedetect_multiview_reinforce(pBuffer, (unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, (int)gray.step,
					1.2f, 2, 48, 0, doLandmark);
				flip(frame, show, 1);
				imshow("Face", show);
				//print the detection results
				if (pResults != NULL)
				{
					for (int i = 0; i < (pResults ? *pResults : 0); i++)
					{
						short * p = ((short*)(pResults + 1)) + 142 * i;
						Rect_<float> face_rect = Rect_<float>(p[0], p[1], p[2], p[3]);
						//face = gray(face_rect);
						face = frame(face_rect);
						cv::rectangle(frame, Rect(p[0], p[1], p[2], p[3]), Scalar(230, 255, 0), 2);
					}
					flip(frame, show, 1);
					imshow("Face", show);
					if (!face.empty())
					{
						//测试图像应该是灰度图
						cvtColor(face, face, CV_BGR2GRAY);
						equalizeHist(face, face);
						cv::resize(face, face, Size(250, 250));
						try
						{
							predictPCA = modelPCA->predict(face);
							if (predictPCA == 1) {
								cap.release();
								free(pBuffer);
								return 1;
							}
							else {
								cap.release();
								free(pBuffer);
								return 0;
							}
						}
						catch (cv::Exception a)
						{
							cout << a.err;
						}
					}
				}
				free(pBuffer);
				waitKey(20);
				time_all++;
				if (time_all > 200) {
					cap.release();
					return -1;
				}
			}
		}
	}
	else
		return -1;
}