#include "FaceRecognize.h"


//人脸识别主函数
//本函数应于3-5秒内返回，或者使用多线程，以免（过于）阻塞主界面
//@return -1人脸识别失败；0人脸不匹配；1识别成功；
int StartFaceRecognize() {

	return (rand()%3)-1;
}