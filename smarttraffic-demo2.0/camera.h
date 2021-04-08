#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <iostream>
#include <string>
#include <gst/gst.h>
#include <gst/app/app.h>
#include <glib.h>

namespace camera {

#define PICTURE_WIDTH 640
#define PICTURE_HEIGHT 480
#define FRAME_PER_SECOND 30

class Camera {

public :
    Camera();
    ~Camera();
    GstElement *pipeline,*sink;
    GstBus *gstbus;

    bool flags;

    void signalHandle();
    bool setPipelineStatus(GstState state);
    void setElementsPro();
    bool checkElements();
    bool linkPipeline();

private :
    GstElement *source,*capsfilter;
    GstElement *h264parse, *queue;
    GstElement *qtidec,*trans,*transfilter;
    GstCaps *caps,*transcaps;

    void createElements();

};

}

#endif // !__CAMERA_H__

