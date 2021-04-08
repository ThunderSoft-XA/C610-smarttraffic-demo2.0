#ifndef __DETECTION_HPP__

#if 0
#include "core.hpp"
#include "opencv.hpp"
#include "imgproc.hpp"
#include "imgproc/imgproc.hpp"
#endif
#if 1
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#endif

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "ObjectDetection.h"
#include "snpe_udl_layers.h"
#include "Log.h"

using namespace objdec;
using namespace std;

class DetProcess
{
private:
    /* data */
public:
    std::string* labels;
    std::string line;
    std::ifstream in;
    ObjectDetection* detector;
    cv::Mat img;
    std::vector<int> result;
    std::string *goodsNameSet;
    int goodsTotal;
    string imgpath;

    DetProcess(/* args */);
    ~DetProcess();

    int init();
    int getStaticImg();
    int getImg(int imgnum);
    int getResult();
};

DetProcess::DetProcess(/* args */)
{
    labels = new std::string[1001];
    goodsNameSet = new std::string[64];
    in.open("/data/detection/imagenet_slim_labels.txt");

    int count=0;
    while (getline(in,line)) {
        labels[count] = line;
        count++; 
    }
    cout << "open imagenet_slim_labels.txt file successed!!" << endl;
    init();
    goodsTotal = 0;
}

DetProcess::~DetProcess()
{
    detector->deInit();
}

int DetProcess::init()
{
    detector = new ObjectDetection();
    if(detector == NULL) {
        return -1;
    }
    detector->init(GPU);
    detector->setConfidence(0.75f);

    return 0;
}

int DetProcess::getStaticImg()
{
    img = cv::imread("/data/detection/img/orange.jpeg");
    if(img.empty()){
        std::cout << "empty" << std::endl;
        return -1;
    }
    return 0;
}

int DetProcess::getImg(int imgnum)
{
    char buff[64];
    sprintf(buff,"/data/detection/img/image%d.jpeg",imgnum);
    imgpath = string(buff);
    //cout << "imagepath = " << imgpath << endl;
    img = cv::imread(imgpath);
    if(img.empty()){
        //std::cout << "empty" << std::endl;
        return -1;
    }
    return 0;
}

int DetProcess::getResult()
{
    //test
    result = detector->doDetect(img);
    if(result.size()>0) {
      int i;
      for(i = 0; i < result.size(); i++) {
          if(!labels[result[i]].empty() ) {
              goodsNameSet[goodsTotal] = labels[result[i]];
              goodsTotal++;
            }
      }
    }
    return 0;
}

#endif // !__DETECTION_HPP__

