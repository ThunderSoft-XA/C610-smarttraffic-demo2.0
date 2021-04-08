#include "camera.h"
#include "configenv.h"

using namespace std;
using namespace camera;


Camera::Camera()
{
    /* Create the empty pipeline */
#ifdef DEBUG
    cout << __FILE__ << "==Create the empty pipeline=="<<__LINE__ << endl;
#endif // DEBUG
    flags = false;

    cout << __FILE__ << "====" <<  __LINE__ << endl;
    /* setup camera environment */
    if(setCameraEnv()) {
        #ifdef DEBUG
            cout << __FILE__ << "camera env init failed" << __LINE__ << endl;
        #endif // DEBUG
    }
    system("source /etc/gstreamer1.0/set_gst_env.sh");
    gst_init(NULL,NULL);

    if( gst_is_initialized()) {
        pipeline = gst_pipeline_new ("test-pipeline");
        createElements();
    }

}

void  Camera::createElements()
{
    /* Create the elements */
    /* gst-launch-1.0 -e qtiqmmfsrc name=qmmf ! video/x-h264,format=NV12,
        framerate=30/1,width=640,height=480 ! h264parse config-interval=1 ! 
        queue ! qtivdec ! qtivtransform ! video/x-raw,format=BGRA ! appsink */
    cout << __FILE__ << "====" <<  __LINE__ << endl;
    source = gst_element_factory_make ("qtiqmmfsrc", "source");
    capsfilter = gst_element_factory_make ("capsfilter", "capsfilter");
    h264parse = gst_element_factory_make ("h264parse", "parse");
    queue = gst_element_factory_make ("queue", "queue1");
    qtidec = gst_element_factory_make("qtivdec","vdec");
    trans = gst_element_factory_make ("qtivtransform", "trans");
    transfilter = gst_element_factory_make ("capsfilter", "transfilter");
    sink = gst_element_factory_make ("appsink", "sink1");

    caps = gst_caps_new_simple ("video/x-h264",
        "format", G_TYPE_STRING, "NV12",
        "framerate", GST_TYPE_FRACTION, FRAME_PER_SECOND, 1,
        "width", G_TYPE_INT,PICTURE_WIDTH,
        "height", G_TYPE_INT,PICTURE_HEIGHT,
        NULL);
    transcaps = gst_caps_new_simple ("video/x-raw","format", G_TYPE_STRING, "BGRA",
        "width", G_TYPE_INT,PICTURE_WIDTH,
        "height", G_TYPE_INT,PICTURE_HEIGHT,
        NULL);
}

bool Camera::checkElements()
{
    if (!pipeline || !source || !capsfilter || !h264parse  || !queue || !qtidec || !trans || !transfilter || !sink) {
        g_printerr ("Not all elements could be created.\n");
        return false;
    }

    return true;
}


void Camera::setElementsPro()
{
    /* Modify the source's properties */
#ifdef DEBUG
    cout << __FILE__ << "==setup element`s properties==" << __LINE__ << endl;
#endif // DEBUG
    g_object_set(G_OBJECT(source), "name", "qmmf", NULL);
    g_object_set(G_OBJECT(source), "af-mode", 1, NULL);
    g_object_set(G_OBJECT(source), "sharpness", 4, NULL);
    g_object_set(G_OBJECT(source), "noise-reduction", 2, NULL);

    g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);
    g_object_set(G_OBJECT(h264parse), "config-interval", -1, NULL);
    g_object_set(G_OBJECT(queue),"max-size-buffers",10,NULL);
    g_object_set(G_OBJECT(queue),"max-size-bytes",5120,NULL);
    g_object_set(G_OBJECT(trans),"rotate",1,NULL);
    g_object_set(G_OBJECT(transfilter), "caps", transcaps, NULL);

    g_object_set(G_OBJECT(sink),"emit-signals",TRUE,NULL);
    g_object_set(G_OBJECT(sink),"drop",TRUE,NULL);

}

bool Camera::linkPipeline()
{
    /* link the pipeline */
    gst_bin_add_many (GST_BIN (pipeline), source, capsfilter,h264parse, queue,qtidec, trans,transfilter, sink, NULL);
    if (!gst_element_link_many(source, capsfilter,h264parse,qtidec,trans,transfilter, sink, NULL)) {
        g_printerr ("Elements could not be linked.\n");
        gst_object_unref (pipeline);
        return false;
    }
    flags = true;
    return true;
}

static void
cb_message (GstBus     *bus,
            GstMessage *message,
            gpointer    user_data)
{
  GstElement *pipeline = GST_ELEMENT (user_data);
 
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:
    case GST_MESSAGE_EOS:
      g_print ("we reached EOS/error\n");
      gst_element_set_state(pipeline,GST_STATE_NULL);
      //g_main_loop_quit (loop);
      break;
    case GST_MESSAGE_APPLICATION:
    {
      if (gst_message_has_name (message, "ExPrerolled")) {
        /* it's our message */
        g_print ("we are all prerolled, do seek\n");
        gst_element_seek (pipeline,
            1.0, GST_FORMAT_TIME,
            GstSeekFlags(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE),
            GST_SEEK_TYPE_SET, 2 * GST_SECOND,
            GST_SEEK_TYPE_SET, 5 * GST_SECOND);
 
        gst_element_set_state (pipeline, GST_STATE_PLAYING);
      }
      break;
    }
    default:
      break;
  }
}

void Camera::signalHandle()
{
    gstbus = gst_element_get_bus(pipeline);
    gst_bus_add_signal_watch(gstbus);
    g_signal_connect (gstbus, "message", (GCallback) cb_message,pipeline);
}

bool Camera::setPipelineStatus(GstState state)
{
    GstStateChangeReturn ret;
    ret = gst_element_set_state(pipeline,state);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        #ifdef DEBUG
            g_printerr ("Unable to set the pipeline`s state.\n");
        #endif // DEBUG
        gst_object_unref (pipeline);
        return false;
    }
  return true;
}

Camera::~Camera()
{
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref (gstbus);
    gst_object_unref(GST_OBJECT(pipeline));
}