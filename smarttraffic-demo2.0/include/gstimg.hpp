#ifndef __GST_IMG_HPP__
#define __GST_IMG_HPP__


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

class GstImg
{
public:
    GstImg(){}
    ~GstImg() {}

public:
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;
    GstElement *pipeline;

public:
    int CreateImgPipeline();
    /* Handler for the pad-added signal */
};

/*
filesrc location=mux_test.mp4 ! decodebin ! videoconvert 
! video/x-raw,framerate=30/1 ! jpegenc ! multifilesink next-file= max-duration m
ax-file-duration=999999999 location=frame%d.jpeg
*/

int GstImg::CreateImgPipeline() 
{
    GError *errinfo = NULL;

    /* Initialize GStreamer */
    gst_init (NULL,NULL);

    string pipedes = "filesrc location=/data/detection/mux_test.mp4 ! decodebin ! videoconvert ! \
        video/x-raw,format=BGR,framerate=30/1 ! jpegenc ! \
        multifilesink next-file=max-duration max-file-duration=1000000000 \
        location=/data/detection/img/image%d.jpeg";

    pipeline = gst_parse_launch(pipedes.c_str(),&errinfo);

    if (errinfo != NULL) {
        printf ("[Camera %s]could not construct pipeline: %s\n", pipedes, errinfo->message);
        g_clear_error (&errinfo);
        gst_object_unref (pipeline);
    }
    //cout << "Start playing" << endl;
    /* Start playing */
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (pipeline);
    }

    /* Listen to the bus */
    bus = gst_element_get_bus (pipeline);

    GError *err;
    gchar *debug_info;
    gboolean state = false;
    do{
        msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
            (GstMessageType)(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
        if (msg != NULL) {
            switch (GST_MESSAGE_TYPE (msg)) {
                case GST_MESSAGE_ERROR:
                    gst_message_parse_error (msg, &err, &debug_info);
                    g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
                    g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
                    g_clear_error (&err);
                    g_free (debug_info);
                    break;
                case GST_MESSAGE_STATE_CHANGED:
                    break;
                case GST_MESSAGE_EOS:
                    //g_print ("End-Of-Stream reached.\n");
                    state = true;
                    break;
                default:
                    /* We should not reach here because we only asked for ERRORs and EOS */
                    g_printerr ("Unexpected message received %d.\n",GST_MESSAGE_TYPE (msg));
                    break;
            }
            sleep(1);
        }
    }while(!state);
    gst_message_unref(msg);

    /* Free resources */
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
    return 0;
}

#endif //__GST_IMG_HPP__


