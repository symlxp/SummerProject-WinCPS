#pragma once
#include <cstdlib>
#include <cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/face.hpp>
#include <opencv2/face/facerec.hpp>
#include <facedetect-dll.h>
#include <iostream>

#define DETECT_BUFFER_SIZE 0x20000
// 采样频率
#define SAMPLE_FREQUENCY 16000

using namespace cv;
using namespace std;
using namespace face;
int StartFaceRecognize();
bool recognize();
size_t ReadWav(const string& filePath, short int voiceData[], size_t sizeData, size_t seek);