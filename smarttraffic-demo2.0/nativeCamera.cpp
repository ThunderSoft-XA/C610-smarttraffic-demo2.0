#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <thread>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <chrono>
#include <time.h>

#include "nativeCamera.h"

#define CURRENTPATH "./capimages/"

using namespace camera;
using namespace sqlite;
using namespace std;
using namespace cv;

using namespace easypr;

Sqlite *NativeCamera::imageDB = new Sqlite();
Mat NativeCamera::imageMat = Mat(480,320,CV_8UC1);
bool NativeCamera::capture = false;
DataProerty *NativeCamera::classData = (DataProerty *)malloc(sizeof(DataProerty));

GstPadProbeReturn NativeCamera::padHaveData (GstPad *pad,GstPadProbeInfo *info,gpointer user_data)
{
  GstMapInfo map;
  GstBuffer *buffer;
 
  buffer = GST_PAD_PROBE_INFO_BUFFER (info);
 
  buffer = gst_buffer_make_writable (buffer);
 
  /* Making a buffer writable can fail (for example if it
   * cannot be copied and is used more than once)
   */
  if (buffer == NULL)
    return GST_PAD_PROBE_OK;
 
  /* Mapping a buffer can fail (non-writable) */
  if (gst_buffer_map (buffer, &map, GST_MAP_WRITE)) {
       Mat frame(Size(1920,1080), CV_8UC3, (char*)map.data, Mat::AUTO_STEP);
       if(capture == true) {
           imageMat = frame.clone();
           test_plate_recognize(frame);
           imwrite("CURRENTPATH",imageMat);
           classData->path = CURRENTPATH;
           classData->name = "dssddad";
           classData->preTime = 3131;
           imageDB->insertData(classData);
       }
        gint size = frame.cols*frame.rows*3;
       //copy image data to queue after Processing is complete
       memcpy(map.data,frame.data,size);
       gst_buffer_unmap (buffer, &map);
    }
 
  GST_PAD_PROBE_INFO_DATA (info) = buffer;
  return GST_PAD_PROBE_OK;
}

NativeCamera::NativeCamera()
{
    cout << "=====init sqlite=====" << endl;
    setPiplineParm(PICTURE_WIDTH,PICTURE_HEIGHT,FRAME_PER_SECOND,CODE_TYPE);
    cout << "=====finished setup parm=====" << endl;
    classData = (DataProerty *) malloc(sizeof(DataProerty));
    this->buildPipline();
    cout << "parm init successed..." << endl;

    this->createPipline();
    cout << "build pipline successed..." << endl;
}

void NativeCamera::setPiplineParm(int width, int height, int frame, char *type)
{
    this->setHeight(width);
    this->setHeight(height);
    this->setframeRate(frame);
    this->setCodeType(type);
}

void NativeCamera::buildPipline()
{
    /* gst-launch-1.0 -e qtiqmmfsrc af-mode=1 name=qmmf ! 
     video/x-h264,format=NV12,width=1920,height=1080,framerate=30/1 ! 
     h264parse ! mp4mux ! queue ! filesink location=/data/mux_h264_4k_avc.mp4 */
    /* Create the elements */
    //filled factoryname and alias name
    this->source = gst_element_factory_make ("qtiqmmfsrc", "qmmf");
    this->capsfilter = gst_element_factory_make ("capsfilter", "qtifilter");
    this->h264parse = gst_element_factory_make ("h264parse", "qtih264p");
    this->mp4mux = gst_element_factory_make ("mp4mux", "qtimp4m");
    this->queue = gst_element_factory_make ("queue", "qtiqueue");
    this->sink = gst_element_factory_make ("filesink", "qtisink");
    //this->sink = gst_element_factory_make("appsink","sink1");
    this->videoCaps = gst_caps_new_simple ("video/x-h264",
        "format", G_TYPE_STRING, "NV12",
        "framerate", GST_TYPE_FRACTION, FRAME_PER_SECOND, 1,
        "width", G_TYPE_INT, PICTURE_WIDTH,
        "height", G_TYPE_INT, PICTURE_HEIGHT,
        NULL);
        /**
         * @brief qmmf. ! "image/jpeg,width=3840,height=2160,framerate=30/1" ! multifilesink
                            location=/data/frame%d.jpg sync=true async=false
         * 
         */
        // this->imageCaps = gst_caps_new_simple("image/jpeg",
        //     "framerate",GST_TYPE_FRACTION,FRAME_PER_SECOND,1,
        //     "width", G_TYPE_INT, PICTURE_WIDTH,
        //     "height", G_TYPE_INT, PICTURE_HEIGHT,
        //     NULL);

        g_assert(this->source);
        g_assert(this->capsfilter);
        g_assert(this->h264parse);
        g_assert(this->mp4mux);
        g_assert(this->queue);
        g_assert(this->sink);
        g_assert(this->videoCaps);
        // g_assert(this->imageCaps);
}

bool NativeCamera::createPipline()
{
    /* Modify the source's properties */
    //filled gpointer bject and const gchar    *first_property_name
    g_object_set(G_OBJECT(this->source), "af-mode", 1, NULL);
    g_object_set(G_OBJECT(this->source), "sharpness", 4, NULL);
    g_object_set(G_OBJECT(this->source), "noise-reduction", 2, NULL);

    g_object_set(G_OBJECT(this->capsfilter), "caps", this->videoCaps, NULL);
    g_object_set(G_OBJECT(this->h264parse), "config-interval", 1, NULL);
    g_object_set(G_OBJECT(this->mp4mux), "name", "muxer", NULL);
    g_object_set(G_OBJECT(this->sink), "location", "/data/camera/test.mp4", NULL);

    /* Create the empty pipeline */
    //cout << "create a empty pipline" << endl;
    this->pipeline = gst_pipeline_new ("test-pipeline");

    if (!pipeline || !source || !capsfilter || !h264parse || !mp4mux || !queue || !sink) {
        cout << pipeline << "+1+"<< source << "+2+"<< capsfilter  << "+3+" << h264parse  << "+4+"<< mp4mux  << "+5+"<< queue  << "+6+"<< sink  << "+7+"<< endl;
        g_printerr ("Not all elements could be created.\n");
        return false;
    }

    /* Build the pipeline */
    gst_bin_add_many (GST_BIN (this->pipeline), source, capsfilter, h264parse, mp4mux, queue, sink, NULL);
    if (!gst_element_link_many(source, capsfilter, h264parse, mp4mux, queue, sink, NULL)) {
        g_printerr ("Elements could not be linked.\n");
        gst_object_unref (pipeline);
        return false;
    }

     gst_element_set_state (this->pipeline, GST_STATE_NULL);

    this->pad = gst_element_get_static_pad(this->capsfilter,"sink");
    gst_pad_add_probe(this->pad,GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback)padHaveData,NULL,NULL);

    return true;
}

bool NativeCamera::setPlayingStatus()
{
    g_print("Playing status");
    gst_element_set_state (this->pipeline, GST_STATE_PLAYING);
}
bool NativeCamera::setPauseStatus()
{
    g_print("Pause status");
    gst_element_set_state (this->pipeline, GST_STATE_PAUSED);
}

bool NativeCamera::setStopStatus()
{
    g_print("Stop status");
    gst_element_set_state (this->pipeline, GST_STATE_READY);
}

bool NativeCamera::getAFrame()
{
    g_print("get a frame data");
    this->capture = true;
}

NativeCamera::~NativeCamera()
{
    this->imageDB->~Sqlite();
    gst_object_unref(this->pad);
    gst_element_set_state (this->pipeline, GST_STATE_NULL);
    gst_object_unref (this->pipeline);
}


