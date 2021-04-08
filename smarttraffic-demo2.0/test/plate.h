#ifndef __PLATE_H__

#include "easypr/core/plate_locate.h"
#include "easypr/core/plate_judge.h"
#include "easypr/core/plate.hpp"
#include "easypr/core/plate_recognize.h"
#include "easypr/core/plate_detect.h"

using namespace cv;
using namespace easypr;

int test_plate_locate();
int test_plate_judge();
int test_plate_detect();
std::vector<easypr::CPlate> test_plate_recognize(Mat userMat);

#endif // !__PLATE_H__



