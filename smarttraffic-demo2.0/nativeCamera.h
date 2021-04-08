#ifndef __NATIVE_CAMERA_H__
#define __NATIVE_CAMERA_H__

#include <gst/gst.h>
#include <gst/app/app.h>
#include <glib.h>
#include <iostream>
#include <condition_variable>
#include <tr1/memory>
#include <unistd.h>
#include <sys/time.h>
#include <opencv.hpp>

#include "sqliteTest.h"
#include "test/plate.h"

using namespace cv;
using namespace sqlite;
using namespace easypr;

#define PICTURE_WIDTH 640
#define PICTURE_HEIGHT 480
#define FRAME_PER_SECOND 30
#define CODE_TYPE "NV12"

namespace camera{
class NativeCamera
{
private:
    /* data */
    GstElement *source, \
                             *capsfilter, \
			     *h264parse, \
			     *mp4mux, \
			     *queue,\
			     *sink;
	GstCaps *videoCaps,*imageCaps;
	GstElement *pipeline;
	GstBus *gstbus;
	GstBuffer *frameBuffer;
	GstPad *pad;

	static Sqlite *imageDB;
	static Mat imageMat;
	static bool capture;

	int imageWidth,imageHeight,frameRate;
	char* codeType;
	static DataProerty *classData;
	void buildPipline();
	bool createPipline();
	static GstPadProbeReturn padHaveData (GstPad *pad,GstPadProbeInfo *info,gpointer user_data);

public:
    NativeCamera(/* args */);
    ~NativeCamera();
	void setPiplineParm(int width, int height, int frame, char *type);
	bool setPlayingStatus();
	bool setPauseStatus();
	bool setStopStatus();
	bool getAFrame();

	Sqlite *getImageDB()
	{
		return this->imageDB;
	}

	GstElement *getPipeline()
	{
		return this->pipeline;
	}

	GstBus *getGstBus() {
		return this->gstbus;
	}

    void setWidth(int width) {
	    this->imageWidth = width;
    }
    int getWidth() {
	    return this->imageWidth;
    }

	void setHeight(int height) {
	    this->imageHeight = height;
    }
    int getHeight() {
	    return this->imageHeight;
    }

	void setframeRate(int frame) {
	    this->frameRate = frame;
    }
    int getFrameRate() {
	    return this->frameRate;
    }

	void setCodeType(char* str) {
		this->codeType = str;
	}
	string getCodeType() {
		return this->codeType;
	}

};

}

#endif // !__NATIVE_CAMERA_H__
