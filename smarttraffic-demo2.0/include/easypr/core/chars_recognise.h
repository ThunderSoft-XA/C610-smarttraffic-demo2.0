//////////////////////////////////////////////////////////////////////////
// Name:	    chars_recognise Header
// Version:		1.0
// Date:	    2014-09-28
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CCharsRecognise
//////////////////////////////////////////////////////////////////////////
#ifndef EASYPR_CORE_CHARSRECOGNISE_H_
#define EASYPR_CORE_CHARSRECOGNISE_H_

#include "easypr/core/chars_segment.h"
#include "easypr/core/chars_identify.h"
#include "easypr/core/core_func.h"
#include "easypr/util/util.h"
#include "easypr/core/plate.hpp"
#include "easypr/config.h"

namespace easypr {

class CCharsRecognise {
 public:
  CCharsRecognise();

  ~CCharsRecognise();

  int charsRecognise(cv::Mat plate, std::string& plateLicense);
  int charsRecognise(CPlate& plate, std::string& plateLicense);

  inline std::string getPlateColor(cv::Mat input) const {
    std::string color = "UNKNOWN";
    Color result = getPlateType(input, true);
    if (BLUE == result) color = "BlueLicense";
    if (YELLOW == result) color = "YellowLicense";
    if (WHITE == result) color = "WhiteLicense";
#ifdef OS_WINDOWS
    color = utils::utf8_to_gbk(color.c_str());
#endif
    return color;
  } 

  inline std::string getPlateColor(Color in) const {
    std::string color = "UNKNOWN";
    if (BLUE == in) color = "BlueLicense";
    if (YELLOW == in) color = "YellowLicense";
    if (WHITE == in) color = "WhiteLicense";
#ifdef OS_WINDOWS
    color = utils::utf8_to_gbk(color.c_str());
#endif
    return color;
  }

  inline void setMaoDingSize(int param) {
    m_charsSegment->setMaoDingSize(param);
  }
  inline void setColorThreshold(int param) {
    m_charsSegment->setColorThreshold(param);
  }
  inline void setBluePercent(float param) {
    m_charsSegment->setBluePercent(param);
  }
  inline float getBluePercent() const {
    return m_charsSegment->getBluePercent();
  }
  inline void setWhitePercent(float param) {
    m_charsSegment->setWhitePercent(param);
  }
  inline float getWhitePercent() const {
    return m_charsSegment->getWhitePercent();
  }

 private:
  //！ Character segmentation 

  CCharsSegment* m_charsSegment;
};

} /* \namespace easypr  */

#endif  // EASYPR_CORE_CHARSRECOGNISE_H_