#include "FaceRecognize.h"
#include "mfcc.h"
#include "gmm.h"

// 采样频率
#define TRAINSIZE SAMPLE_FREQUENCY*4        //4 secondes of voice for trainning
// --- you can increase this value to improve the recognition rate
#define RECOGSIZE SAMPLE_FREQUENCY*2 //1 seconde of voice for recognition

//人脸识别主函数
//本函数应于3-5秒内返回，或者使用多线程，以免（过于）阻塞主界面
//@return -1人脸识别失败；0人脸不匹配；1识别成功；
VideoCapture cap;
Ptr<FaceRecognizer> modelPCA;
Mat frame;
//int number=0;
int StartFaceRecognize() {
	system("d:/release/voice.exe");
	if(recognize()==false)
		return 0;
	//cv::destroyWindow("Face");
	cv::destroyAllWindows();
	//number++;
	//modelPCA = LBPHFaceRecognizer::create();
	//modelPCA = FisherFaceRecognizer::create();
	modelPCA = EigenFaceRecognizer::create();
	int time_all=0;
	//int count = 0;
	double predicted_confidence = 0.0;
	int predictPCA = -1;
	Point text_lb;
	ifstream faceFile;
	faceFile.open("d:/faces/model/Face_Model.xml");
	if (!faceFile.is_open())
	{
		return -1;
	}
	faceFile.close();
	modelPCA->read("d:/faces/model/Face_Model.xml");
	if (cap.isOpened())
		cap.release();     //decide if capture is already opened; if so,close it
	cap.open(0);           //open the default camera
	if (cap.isOpened())
	{
		//if(getWindowProperty("Face", CV_WINDOW_AUTOSIZE)!=-1)
		

		namedWindow("Face", CV_WINDOW_AUTOSIZE);
		
		while (1)
		{
			cap >> frame;
			if (!frame.empty())
			{
				time_all++;
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
				Point tishi(0, 20);
				cvtColor(frame, gray, CV_BGR2GRAY);//转为灰度图
				equalizeHist(gray, gray);
				int doLandmark = 1;// do landmark detection
								   ///////////////////////////////////////////
								   // frontal face detection / 68 landmark detection
								   //正面人脸检测/ 68标志性检测
								   // it's fast, but cannot detect side view faces
								   //它很快，但不能检测侧面的脸
								   //////////////////////////////////////////
				if(time_all>10)
					pResults = facedetect_multiview_reinforce(pBuffer, (unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, (int)gray.step,
					1.2f, 2, 48, 0, doLandmark);
				flip(frame, show, 1);
				putText(show, "Please put you face into the camera!",tishi , FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
				//imshow(format("Face%d",number), show);
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
					if (!face.empty()&& face.data)
					{
						flip(frame, show, 1);
						putText(show, "Got face,waiting for recognize!", tishi, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
						imshow("Face", show);
						waitKey(1000);
						//测试图像应该是灰度图
						cvtColor(face, face, CV_BGR2GRAY);
						equalizeHist(face, face);
						cv::resize(face, face, Size(120, 120));
						try
						{
							modelPCA->predict(face, predictPCA, predicted_confidence);
							if (predictPCA == 66 && predicted_confidence<5000.0) {
								//EigenFaceRecognizer一般3000越大差距越大
								//count++;
								cap.release();
								free(pBuffer);
								flip(frame, show, 1);
								putText(show, format("Success! %lf", predicted_confidence), tishi, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
								imshow("Face", show);
								waitKey(1000);
								cv::destroyWindow("Face");

								return 1;
								//putText(show, format("Holding,don't move! %lf", predicted_confidence), tishi, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
								//imshow("Face", show);
							}
							else {
								cap.release();
								free(pBuffer);
								flip(frame, show, 1);
								putText(show, format("Fail! %lf %d", predicted_confidence, predictPCA), tishi, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
								imshow("Face", show);
								waitKey(1000);
								cv::destroyWindow("Face");
								return 0;
							}
							//if (count > 4)
							//{

							//}
						}
						catch (cv::Exception a)
						{
							cout << a.err;
						}
					}
				}
				free(pBuffer);
				waitKey(20);
				
				if (time_all > 80) {
					cap.release();
					cv::destroyWindow("Face");
					return -1;
				}
			}
		}
	}
	else
		return -1;
}
bool recognize()
{
	Gmm gmm;
	Mfcc mfcc(16000, 16, 8, Mfcc::Hamming, 24, 12);
	string filePath;
	size_t realSize;
	size_t frameCount;
	vector<vector<double>> melCepData;
	short int bigVoiceBuffer[TRAINSIZE];
	filePath = "d:/a.gmm";
	/*for(int personId=0; personId<=9; personId++)
	{
	filePath = GetFilePath(personId, 0, 2, "gmm");
	gmm.AddModel(filePath, GetPersonName(personId));
	}*/
	gmm.AddModel("d:/0.gmm", "wrong");
	gmm.AddModel(filePath, "real");

	filePath = "d:/haha.wav";//无声double -15329.3//敲击double -56727.9
						//filePath="F04_1-16000.wav";//double -16938.6
						//filePath="haha.wav";//double -16938.6
						//filePath="F00-16000.wav";//double -20063.4
	ifstream voiceFile;
	voiceFile.open(filePath);
	if (!voiceFile.is_open())
	{
		return false;
	}
	voiceFile.close();
	realSize = ReadWav(filePath, bigVoiceBuffer, RECOGSIZE, 0);
	if (realSize<1) return false;
	frameCount = mfcc.Analyse(bigVoiceBuffer, realSize);
	melCepData = mfcc.GetMFCCData();
	if (gmm.Recogniser(melCepData, frameCount))
	{
		return true;
	}
	else
	{
		return false;
	}
}
size_t ReadWav(const string& filePath, short int voiceData[], size_t sizeData, size_t seek)
{
	ifstream inFile(filePath, ifstream::in | ifstream::binary);
	size_t ret;

	if (!inFile.is_open())
	{
		cout << endl << "Can not open the WAV file !!" << endl;
		return -1;
	}

	char waveheader[44];
	inFile.read(waveheader, 44);

	if (seek != 0) inFile.seekg(seek * sizeof(short int), ifstream::cur);

	inFile.read(reinterpret_cast<char *>(voiceData), sizeof(short int)*sizeData);
	ret = (size_t)inFile.gcount() / sizeof(short int);

	inFile.close();
	return ret;
}