#ifndef __REFRESH_DATA_H__
#define __REFRESH_DATA_H__

#include "easyui.h"
#include "camera.h"
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace easyui;
using namespace camera;

typedef struct _RefreshData {
    char *licenseColor;
    char *license;
} RefreshData;

typedef struct _RefreshObject {
    EasyUI *dstUI;
    Camera *srcCamera;
    cv::Mat imgMat;

} RefreshObject;


#endif // !__REFRESH_DATA_H__