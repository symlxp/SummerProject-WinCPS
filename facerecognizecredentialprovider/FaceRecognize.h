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
using namespace cv;
using namespace std;
using namespace face;
int StartFaceRecognize();