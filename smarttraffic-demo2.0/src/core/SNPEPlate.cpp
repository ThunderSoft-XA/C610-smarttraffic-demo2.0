#include <ctime>
#include <iostream>
#include <vector>

#include "easypr/core/SNPEPlate.hpp"


using namespace cv;
using namespace std;
using namespace easypr;

void SNPEPlate::setConfidence(float value) {
    mConfidenceThreshold = value > 1.0f ? 1.0f : (value < 0.0f ? 0.0f : value);
}

SNPEPlate::SNPEPlate()
{
    flags = false;
    Version = zdl::SNPE::SNPEFactory::getLibraryVersion();
    std::cout << "SNPE Version: " << Version.asString().c_str() << std::endl;
}

bool SNPEPlate::init()
{
    /*SNPE Object Init*/
    std::unique_ptr<zdl::DlContainer::IDlContainer> container;
    container = zdl::DlContainer::IDlContainer::open(zdl::DlSystem::String(MODEL_PATH));

    zdl::DlSystem::Runtime_t runtime;
    //runtime = getRunTime();
    runtime = zdl::DlSystem::Runtime_t::GPU;

    this -> snpe = initSnpe(container , runtime);
    if(snpe == nullptr)
    {
        std::cerr << "Error while init the snpe object." << std::endl;
        return false;
    }
    this->setConfidence(0.80f);
    this->input = initITensorInput(this ->snpe);
    flags = true;
    return true;
}

int SNPEPlate::snpeCheck()
{
    cv::Mat frame = this->srcMat.clone();
    bool execStatus = false;

    int x1,y1,x2,y2;

    std::vector<float> output;
    zdl::DlSystem::TensorMap outputTensorMap;

    int width = 3;
    int height = 4;

    cv::Mat image = frame.clone();
    if(image.empty()) {
        cout << "Warn : Read from capture failded!" << endl;
        return -1;
    }

    SNPEPlate::loadITensorInputData(frame,this->input);
    execStatus = snpe->execute(this->input.get(), outputTensorMap);
    if (execStatus != true) {
        cout << "Warn : snpe execute  failded!" << endl;
        return -1;
    }

    zdl::DlSystem::StringList tensorNames = outputTensorMap.getTensorNames();
    int count = 0;
    for(auto& name : tensorNames) {
        auto tensorPtr = outputTensorMap.getTensor(name);
                
        cout << "OutputNode : " << name << endl;
        cout << "OutputShape : " << tensorPtr->getShape().getDimensions()[0] << ","
             << tensorPtr->getShape().getDimensions()[1]<< ","
             << tensorPtr->getShape().getDimensions()[2]<< ","
             << tensorPtr->getShape().getDimensions()[3]<< ","
             << endl;

        output.clear();
                
        cout << "Output Rusult : "<<endl;
        for ( auto it = tensorPtr->cbegin(); it != tensorPtr->cend() ;it++ ) {
            count += 1;
            float f = *it;
            if(f > this->mConfidenceThreshold) {
                cout << f << " ";
                output.push_back(f);
            }
        }
    }
    return count;
}
#if 0
RefreshData SNPEPlate::plate_recognize(cv::Mat &src)
{
    cout << __FILE__ << "plate_recognize" << __LINE__ << endl;
    CPlateRecognize pr;
    RefreshData myFreshData;
    pr.setLifemode(true);
    pr.setDebug(false);
    pr.setMaxPlates(1);
    //pr.setDetectType(PR_DETECT_COLOR | PR_DETECT_SOBEL);
    pr.setDetectType(easypr::PR_DETECT_CMSER | PR_DETECT_COLOR | PR_DETECT_SOBEL);

    //vector<string> plateVec;
    vector<CPlate> plateVec;

    int result = pr.plateRecognize(src, plateVec);
    //int result = pr.plateRecognizeAsText(src, plateVec);
    char* substr,*savestr;
    std::ostringstream license,res;
    if (result == 0) {
        size_t num = plateVec.size();
        cout << __FILE__ << "license num = " << num << "---" << __LINE__ << endl;
        for (size_t j = 0; j < num; j++) {
            cout << "plateRecognize: " << plateVec[j].getPlateStr() << endl;
            if ( !plateVec[j].getPlateStr().empty() ) {
                myFreshData.licenseColor = strtok_r((char *)plateVec[j].getPlateStr().c_str(),":",&savestr);
                cout << __FILE__ << "substr  = " << myFreshData.licenseColor  << "---" << __LINE__ << endl;
                myFreshData.license = (char *)plateVec[j].getPlateStr().substr(strlen(substr)+2, plateVec[j].getPlateStr().length()).c_str();
            }
        }
    }
    return myFreshData;
}
#endif

int SNPEPlate::SnpeSearch()
{
    cv::Mat frame = this->srcMat.clone();
    bool execStatus = false;

    int x1,y1,x2,y2;

    std::vector<float> output;
    zdl::DlSystem::TensorMap outputTensorMap;

    int width = 3;
    int height = 4;

    cv::Mat image = frame.clone();

    if(image.empty()) {
        cout << "Warn : Read from capture failded!" << endl;
        return -1;
    }

    SNPEPlate::loadITensorInputData(frame,this->input);
    execStatus = snpe->execute(this->input.get(), outputTensorMap);
    if (execStatus != true) {
        cout << "Warn : snpe execute  failded!" << endl;
        return -1;
    }

    zdl::DlSystem::StringList tensorNames = outputTensorMap.getTensorNames();
    for(auto& name : tensorNames) {
        auto tensorPtr = outputTensorMap.getTensor(name);
                
        cout << "OutputNode : " << name << endl;
        cout << "OutputShape : " << tensorPtr->getShape().getDimensions()[0] << ","
             << tensorPtr->getShape().getDimensions()[1]<< ","
             << tensorPtr->getShape().getDimensions()[2]<< ","
             << tensorPtr->getShape().getDimensions()[3]<< ","
             << endl;

        output.clear();
                
        cout << "Output Rusult : "<<endl;
        for ( auto it = tensorPtr->cbegin(); it != tensorPtr->cend() ;it++ ) {
            float f = *it;
            if(f > this->mConfidenceThreshold) {
                cout << f << " ";
                output.push_back(f);
            }
        }
        cout << endl;
        for(int i =0;i < tensorPtr->getShape().getDimensions()[2] ; i++) { 
            x1 = output[i*7 + 3] * width;
            y1 = output[i*7 + 4] * height;
            x2 = output[i*7 + 5] * width;
            y2 = output[i*7 + 6] * height;
            cv::rectangle(frame,cv::Point(x1,y1),cv::Point(x2,y2),Scalar(0,255,0),2);
            Rect roi_rect = Rect(x1,y1,(x2-x1),(y2-y1));
            this->roiMatVec.push_back(this->srcMat(roi_rect));
            //RotatedRect rotRect = RotatedRect(cv::Point(x1,y1),cv::Point(x2,y2),cv::Point(x1,y2));
            //this->setTargetRotRect(rotRect);
        }
    }

    if(0) {
        cv::imshow("detect",frame);
        waitKey(0);
        destroyWindow("detect");
    }

    return 0;
}


zdl::DlSystem::Runtime_t SNPEPlate::getRunTime()
{
    zdl::DlSystem::Runtime_t runtime;

   
    if(zdl::SNPE::SNPEFactory::isRuntimeAvailable(zdl::DlSystem::Runtime_t::GPU))
    {
        cout << "Info : check RunTime GPU Success" << endl;
        runtime = zdl::DlSystem::Runtime_t::GPU;
    } 
    else if(zdl::SNPE::SNPEFactory::isRuntimeAvailable(zdl::DlSystem::Runtime_t::DSP))
    {
        cout << "Info : check RunTime DSP Success" << endl;
        runtime = zdl::DlSystem::Runtime_t::DSP;
    }
    else
    {
        cout << "Info : check RunTime Failed , set to CPU" << endl;
        runtime = zdl::DlSystem::Runtime_t::CPU;
    }
    
    return runtime;
}

std::unique_ptr<zdl::SNPE::SNPE> SNPEPlate::initSnpe(std::unique_ptr<zdl::DlContainer::IDlContainer> & container,
    zdl::DlSystem::Runtime_t runtime) 
{
    std::unique_ptr<zdl::SNPE::SNPE> snpe;

    //zdl::DlSystem::RuntimeList runtimeList;
    //runtimeList.add(runtime);

    zdl::SNPE::SNPEBuilder snpeBuilder(container.get());

    snpe = snpeBuilder.setOutputLayers({})
       .setRuntimeProcessor(runtime)
       .setUseUserSuppliedBuffers(false)
       .setCPUFallbackMode(true)
       .build();

    return snpe;
}

std::unique_ptr<zdl::DlSystem::ITensor> SNPEPlate::initITensorInput(std::unique_ptr<zdl::SNPE::SNPE> &snpe)
{  
    const auto &strList_opt = snpe->getInputTensorNames();
    const auto &strList = *strList_opt;
    // Make sure the network requires only a single input
    assert (strList.size() == 1);
    /* Create an input tensor that is correctly sized to hold the input of the network. 
    Dimensions that have no fixed size will be represented with a value of 0. */
    const auto inputShape = *(snpe->getInputDimensions(strList.at(0)));

    return  zdl::SNPE::SNPEFactory::getTensorFactory().createTensor(inputShape);
}

void SNPEPlate::loadITensorInputData(Mat &image,
                                        std::unique_ptr<zdl::DlSystem::ITensor> &input)
{
    /*Note : User must init ITensor Input before call loadITensorInputData()*/

    /*Shape : plane x height x width x channel*/
    size_t input_height = input->getShape().getDimensions()[1];
    size_t input_width = input->getShape().getDimensions()[2];

    Mat tmpImage;
    Mat inputImage(Size(input_width,input_height),CV_32FC3,input.get()->begin().dataPointer());

    /*convert Image Date to Float Format and normalize data to (-1,1) */ 
    /*Pay attention to the call order , wrong call order will cause unexpected problems*/
    resize(image,tmpImage,Size(input_width,input_height));
    tmpImage.convertTo(inputImage,CV_32F);
    cvtColor(inputImage,inputImage,COLOR_BGR2RGB);
    normalize(inputImage,inputImage,-1.0f,1.0f,NORM_MINMAX);
}