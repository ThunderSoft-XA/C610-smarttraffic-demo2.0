#ifndef __GST_CAMERA_HPP__
#define __GST_CAMERA_HPP__

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>
#include <glib.h>
#include <gobject/gsignal.h>
#include <gobject/gclosure.h>
#include <glib-object.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

#define PICTURE_WIDTH 1920
#define PICTURE_HEIGHT 1080
#define FRAME_PER_SECOND 30

class GstCamera
{
public:
    GstCamera();
    ~GstCamera() {
        /* Free resources */
        StopPipeline();
    }

public:
    GstBus *gstbus;
    GstElement *pipeline;
    GstElement *source;
    GstElement *capsfilter, *h264parse, *mp4mux, *queue, *sink;

    GstMessage *msg;
    GstStateChangeReturn ret;
    GstCaps *caps;

public:
    int CreateCameraPipeline();
    void StopPipeline();
    void handlerMsgInfo(GstMessage *msg);
    gboolean setPipelineState(GstState state);
    void sendEventAndGetInfo(GstEvent *event);

};

GstCamera::GstCamera() {
    /* Initialize GStreamer */
    gst_init (NULL,NULL);
    /* gst-launch-1.0 -e qtiqmmfsrc af-mode=1 name=qmmf ! 
     video/x-h264,format=NV12,width=1920,height=1080,framerate=30/1 ! 
     h264parse ! mp4mux ! queue ! filesink location=/data/mux_h264_4k_avc.mp4 */

    /* Create the elements */
    source = gst_element_factory_make ("qtiqmmfsrc", "source");
    capsfilter = gst_element_factory_make ("capsfilter", "capsfilter");
    h264parse = gst_element_factory_make ("h264parse", "h264parse");
    mp4mux = gst_element_factory_make ("mp4mux", "mp4mux");
    queue = gst_element_factory_make ("queue", "queue1");
    sink = gst_element_factory_make ("filesink", "sink1");
    caps = gst_caps_new_simple ("video/x-h264",
        "format", G_TYPE_STRING, "NV12",
        "framerate", GST_TYPE_FRACTION, FRAME_PER_SECOND, 1,
        "width", G_TYPE_INT, PICTURE_WIDTH,
        "height", G_TYPE_INT, PICTURE_HEIGHT,
        NULL);

    cout << "parm init successed..." << endl;
}

void GstCamera::handlerMsgInfo(GstMessage *msg)
{
    GError *err;
    gchar *debug_info;
    if (msg != NULL) {
        switch (GST_MESSAGE_TYPE (msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error (msg, &err, &debug_info);
                g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
                g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
                g_clear_error (&err);
                g_free (debug_info);
                break;
            case GST_MESSAGE_EOS:
                //g_print ("End-Of-Stream reached.\n");
                break;
            default:
                /* We should not reach here because we only asked for ERRORs and EOS */
                g_printerr ("Unexpected message received.\n");
                break;
        }
        gst_message_unref(msg);
    }
}

void GstCamera::StopPipeline()
{
    gst_element_set_state(pipeline,GST_STATE_NULL);
    g_object_unref(pipeline);
    g_object_unref(gstbus);
}

int GstCamera::CreateCameraPipeline()
{
    /* Modify the source's properties */
    g_object_set(G_OBJECT(source), "name", "qmmf", NULL);
    g_object_set(G_OBJECT(source), "af-mode", 1, NULL);
    g_object_set(G_OBJECT(source), "sharpness", 4, NULL);
    g_object_set(G_OBJECT(source), "noise-reduction", 2, NULL);

    g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);
    g_object_set(G_OBJECT(h264parse), "config-interval", 1, NULL);
    g_object_set(G_OBJECT(mp4mux), "name", "muxer", NULL);
    g_object_set(G_OBJECT(sink), "location", "/data/detection/mux_test.mp4", NULL);

    /* Create the empty pipeline */
    //cout << "create a empty pipline" << endl;
    pipeline = gst_pipeline_new ("test-pipeline");

    if (!pipeline || !source || !capsfilter || !h264parse || !mp4mux || !queue || !sink) {
        cout << pipeline << "+1+"<< source << "+2+"<< capsfilter  << "+3+" << h264parse  << "+4+"<< mp4mux  << "+5+"<< queue  << "+6+"<< sink  << "+7+"<< endl;
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }

    /* Build the pipeline */
    gst_bin_add_many (GST_BIN (pipeline), source, capsfilter, h264parse, mp4mux, queue, sink, NULL);
    if (!gst_element_link_many(source, capsfilter, h264parse, mp4mux, queue, sink, NULL)) {
        g_printerr ("Elements could not be linked.\n");
        gst_object_unref (pipeline);
        return -1;
    }
    cout << "build pipline successed..." << endl;

    gstbus = gst_element_get_bus(pipeline);
    return 0;
}

gboolean GstCamera::setPipelineState(GstState state)
{
    ret = gst_element_set_state (pipeline, state);
    if (ret == state) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return false;
    }
    return true;
}

void GstCamera::sendEventAndGetInfo(GstEvent *event)
{
    if(!gst_element_send_event(pipeline, event)) {
        cout << "send event failed" << endl;
    }
    /* Wait until error or EOS */
    msg = gst_bus_timed_pop_filtered (gstbus, GST_CLOCK_TIME_NONE,(GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* Parse message */
    handlerMsgInfo(msg);
}


#endif // __GST_CAMERA_HPP__