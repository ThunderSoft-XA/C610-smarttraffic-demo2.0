//
// Created by liqing on 18-12-3.
//

#ifndef __OBJECT_DETECTION_H__
#define __OBJECT_DETECTION_H__

//#include "jni_types.h"
#include "common.h"

#include <SNPE/SNPE.hpp>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>

namespace objdec{
class ObjectDetection {
    public:
        explicit ObjectDetection();
        virtual ~ObjectDetection();

        state_t init(int device);
        state_t deInit();
    // std::vector<CLASSIFY_DATA> doDetect(cv::Mat img);
        std::vector<int> doDetect(cv::Mat img);
        void setConfidence(float value);

    private:
        std::unique_ptr<zdl::SNPE::SNPE> snpe;
        zdl::DlSystem::StringList outputLayers;
        std::shared_ptr<zdl::DlSystem::ITensor> inTensor;
        zdl::DlSystem::TensorMap outMap;
        zdl::DlSystem::TensorMap inMap;

        float mConfidenceThreshold;
    };

}

#endif //__OBJECT_DETECTION_H__
