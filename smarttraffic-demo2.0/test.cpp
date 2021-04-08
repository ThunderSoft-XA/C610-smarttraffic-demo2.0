#include <string.h>
#include <mcheck.h>
#include "nativeCamera.h"
#include "sqliteTest.h"

#include "easypr/core/plate_locate.h"
#include "easypr/core/plate_judge.h"
#include "easypr/core/plate.hpp"
#include "easypr/core/plate_recognize.h"
#include "easypr/core/plate_detect.h"

#include "configenv.hpp"
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include "gdk-pixbuf/gdk-pixbuf.h"

#include "easypr/core/SNPEPlate.hpp"

#include <cairo/cairo.h>

using namespace camera;
using namespace cv;
using namespace sqlite;
using namespace easypr;

static GtkLabel *licenseLab,*resultLab,*psssLab;
static SNPEPlate snpecheck;

int plate_recognize(cv::Mat &src) {
  cout << __FILE__ << "plate_recognize" << __LINE__ << endl;
  CPlateRecognize pr;
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
        substr = strtok_r((char *)plateVec[j].getPlateStr().c_str(),":",&savestr);
        cout << __FILE__ << "substr  = " << substr << "---" << __LINE__ << endl;
        if(substr != NULL) {
          license << "<span foreground='green' underline='single' underline_color='blue' font_desc='12'>" << substr << "</span>";
          res <<  "<span foreground='green' underline='single' underline_color='blue' font_desc='12'>" 
            << plateVec[j].getPlateStr().substr(strlen(substr)+1, plateVec[j].getPlateStr().length()).c_str() << "</span>";
          if (strcmp(substr,"YellowLicense")  == 0 ) {
            gdk_threads_enter();
            gtk_label_set_markup(GTK_LABEL(psssLab),"<span foreground='blue'  font_desc='24'>NONE</span>");
            gdk_threads_leave; 
          } else if (strcmp(substr,"BlueLicense") == 0 ) {
            gdk_threads_enter();
            gtk_label_set_markup(GTK_LABEL(psssLab),"<span foreground='blue'  font_desc='24'>PASS</span>");
            gdk_threads_leave; 
          } else if (strcmp(substr,"WhiteLicense") == 0 ) {
            gdk_threads_enter();
            gtk_label_set_markup(GTK_LABEL(psssLab),"<span foreground='blue'  font_desc='24'>PASS</span>");
            gdk_threads_leave; 
          }
        } else  {
            license << "<span foreground='green' underline='single' underline_color='blue' font_desc='24'>show color</span>";
            res << "<span foreground='green' underline='single' underline_color='blue' font_desc='24'>show license</span>";
            break; 
        }
      }
      cout << __FILE__ <<  "---" << __LINE__ << endl;
      gdk_threads_enter();
      gtk_label_set_markup(GTK_LABEL(licenseLab),license.str().c_str());
      gtk_label_set_markup(GTK_LABEL(resultLab),
            g_locale_to_utf8(res.str().c_str(),res.str().length(),NULL,NULL,NULL));
     gdk_threads_enter();
    } 
  }
  return result;
}

static GMainLoop *loop;
/* called when a new message is posted on the bus */
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
      g_main_loop_quit (loop);
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

static GstFlowReturn
on_new_sample (GstAppSink *appsink, GtkWidget *image)
{
    static gint frame_idx = 0;
    cv::Mat srcmat;

    GstSample *sample = gst_app_sink_pull_sample(appsink);
    if(sample == NULL){
      if(gst_app_sink_is_eos(appsink))
        return GST_FLOW_EOS;
    }

    cout << __FILE__ << __LINE__ << endl;
    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstCaps *caps = gst_sample_get_caps(sample);
    GstMapInfo map_info;
    gint width,height;

    gst_buffer_map(buffer,&map_info,GST_MAP_READ);
    GstStructure *structure = gst_caps_get_structure(caps,0);
    gst_structure_get_int(structure,"width",&width);
    gst_structure_get_int(structure,"height",&height);

    frame_idx += 1;
    cairo_format_t format;
    cairo_surface_t *surface;
    format = CAIRO_FORMAT_ARGB32;
    surface = cairo_image_surface_create_for_data (map_info.data,
        format, width, height, cairo_format_stride_for_width(format,width));
    gtk_image_set_from_surface(GTK_IMAGE(image),surface);

    char filename[128] = {0};
    g_mkdir_with_parents("/data/camera/pictures",0700);
    snprintf(filename,sizeof(filename),"/data/camera/pictures/%06d.png",frame_idx);
    cairo_status_t st = cairo_surface_write_to_png(surface,filename);
    if(st != CAIRO_STATUS_SUCCESS){
      g_printerr("st:%s\n",cairo_status_to_string(st));
    }
  
  srcmat = imread(filename);
  snpecheck.setSrcMat(srcmat);
  if(snpecheck.snpeCheck() > 0 ) {
    plate_recognize(srcmat);
  }
  cairo_surface_destroy(surface);
  gst_sample_unref(sample);
  return GST_FLOW_OK;
}
#if 0
static GstPadProbeReturn
have_data_cb (GstPad          *pad,
              GstPadProbeInfo *info,
              gpointer         user_data)
{
  gint x, y;
  GstMapInfo map;
  guint16 *ptr, t;
  GstBuffer *buffer;

  cout << __FILE__ << __LINE__ << endl;
  buffer = GST_PAD_PROBE_INFO_BUFFER (info);
 
  buffer = gst_buffer_make_writable (buffer);
 
  /* Making a buffer writable can fail (for example if it
   * cannot be copied and is used more than once)
   */
  if (buffer == NULL)
    return GST_PAD_PROBE_OK;
 
  /* Mapping a buffer can fail (non-writable) */
  if (gst_buffer_map (buffer, &map, GST_MAP_WRITE)) {
    ptr = (guint16 *) map.data;
    /* invert data */
    for (y = 0; y < 288; y++) {
      for (x = 0; x < 384 / 2; x++) {
        t = ptr[384 - 1 - x];
        ptr[384 - 1 - x] = ptr[x];
        ptr[x] = t;
      }
      ptr += 384;
    }
    gst_buffer_unmap (buffer, &map);
  }
 
  GST_PAD_PROBE_INFO_DATA (info) = buffer;
 
  return GST_PAD_PROBE_OK;
}
#endif

int main(int argc,char** argv)
{
    cout << __FILE__ << __LINE__ << endl;
  if(setCameraEnv()) {
    printf("camera env init failed\n");
    return -1;
  }
  system("source /etc/gstreamer1.0/set_gst_env.sh");

  setenv("MALLOC_TRACE", "mtrace.log", 1);
  mtrace();

  if ( !g_thread_supported() ){
    printf("[%s]--Init g_thread./r/n", __FUNCTION__);
    g_thread_init(NULL);
    gdk_threads_init();
  }
  gtk_init (&argc, &argv);
  gst_init (&argc, &argv);
  if(!snpecheck.init()) {
    return -1;
  }

  GtkWindow *window;
  GtkBox *Hbox,*Vbox;
  GtkWidget *videoDraw;

  GstElement *pipeline;
  GstElement *source,*capsfilter;
  GstElement *h264parse, *queue;
  GstElement *qtidec,*trans,*transfilter, *sink;
  GstBus *gstbus;
  GstCaps *caps,*transcaps;

  loop = g_main_loop_new(NULL, FALSE);

  /*setup window */
  window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  GtkStyle  *style = gtk_rc_get_style(GTK_WIDGET( window) );
  pango_font_description_set_family(style->font_desc, "Simhei" );
  gtk_widget_set_style( GTK_WIDGET(window), style );
  gtk_container_set_border_width (GTK_CONTAINER (window), 6);
  gtk_window_set_default_size(GTK_WINDOW(window),900,600);

  Hbox =  GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6));
  Vbox =  GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 6));

  videoDraw = GTK_WIDGET(gtk_image_new());
  gtk_widget_set_size_request(GTK_WIDGET(videoDraw),640,480);
  licenseLab = GTK_LABEL(gtk_label_new(""));
  gtk_label_set_lines (licenseLab,1);
  gtk_label_set_line_wrap (GTK_LABEL(licenseLab),TRUE);
  gtk_label_set_markup(GTK_LABEL(licenseLab),"<span foreground='green' underline='single' underline_color='blue' font_desc='24'>show color</span>");
  psssLab = GTK_LABEL(gtk_label_new(""));
  gtk_label_set_lines (psssLab,1);
  gtk_label_set_markup(GTK_LABEL(psssLab),"<span foreground='blue'  font_desc='24'>PASS</span>");
  resultLab = GTK_LABEL(gtk_label_new(""));
  gtk_label_set_lines (resultLab,1);
  gtk_label_set_markup(GTK_LABEL(resultLab),"<span foreground='green' underline='single' underline_color='blue' font_desc='24'>show license</span>");

  gtk_box_pack_start(Vbox,GTK_WIDGET(licenseLab),TRUE,FALSE,2);
  gtk_box_pack_start(Vbox,GTK_WIDGET(resultLab),TRUE,FALSE,2);
  gtk_box_pack_start(Vbox,GTK_WIDGET(psssLab),TRUE,FALSE,2);
  gtk_box_pack_start(Hbox,GTK_WIDGET(videoDraw),TRUE,TRUE,2);
  gtk_box_pack_start(Hbox,GTK_WIDGET(Vbox),TRUE,TRUE,2);

  gtk_container_add (GTK_CONTAINER(window), GTK_WIDGET(Hbox));

  // show window
  gtk_widget_show(videoDraw);
  gtk_widget_show_all (GTK_WIDGET(window));
  gtk_window_present(GTK_WINDOW(window));

  /* Create the empty pipeline */
  cout << __FILE__ << "==Create the empty pipeline=="<<__LINE__ << endl;
  pipeline = gst_pipeline_new ("test-pipeline");
  /* Create the elements */
/* gst-launch-1.0 -e qtiqmmfsrc name=qmmf ! video/x-h264,format=NV12,
    framerate=30/1,width=640,height=480 ! h264parse config-interval=1 ! 
    queue ! qtivdec ! qtivtransform ! video/x-raw,format=BGRA ! appsink */
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

  /* Modify the source's properties */
  cout << __FILE__ << "==setup element`s properties==" << __LINE__ << endl;
  g_object_set(G_OBJECT(source), "name", "qmmf", NULL);
  g_object_set(G_OBJECT(source), "af-mode", 1, NULL);
  g_object_set(G_OBJECT(source), "sharpness", 4, NULL);
  g_object_set(G_OBJECT(source), "noise-reduction", 2, NULL);

  g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);
  g_object_set(G_OBJECT(h264parse), "config-interval", -1, NULL);
  g_object_set(G_OBJECT(queue),"max-size-bytes",42949670,NULL);
  g_object_set(G_OBJECT(trans),"rotate",1,NULL);
  g_object_set(G_OBJECT(transfilter), "caps", transcaps, NULL);

  g_object_set(sink,"emit-signals",TRUE,NULL);

  if (!pipeline || !source || !capsfilter || !h264parse  || !queue || !qtidec || !trans || !transfilter || !sink) {
    g_printerr ("Not all elements could be created.\n");
    return false;
  }

  /* link the pipeline */
  gst_bin_add_many (GST_BIN (pipeline), source, capsfilter,h264parse, queue,qtidec, trans,transfilter, sink, NULL);
  if (!gst_element_link_many(source, capsfilter,h264parse,queue,qtidec,trans,transfilter, sink, NULL)) {
    g_printerr ("Elements could not be linked.\n");
    gst_object_unref (pipeline);
    return false;
  }
  cout << __FILE__ << "==build pipline successed==" << __LINE__ << endl;

  g_signal_connect(sink,"new-sample",G_CALLBACK(on_new_sample),videoDraw);

  gstbus = gst_element_get_bus(pipeline);
  gst_bus_add_signal_watch(gstbus);
  g_signal_connect (gstbus, "message", (GCallback) cb_message,pipeline);

#if 0
  GstPad *sinkPad;
  sinkPad = gst_element_get_static_pad(sink,"sink");
  gst_pad_add_probe(sinkPad,GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback)(have_data_cb),NULL,NULL);
  gst_pad_set_active(sinkPad,true);
  gst_object_unref (sinkPad);
#endif

  if ((gst_element_set_state(pipeline,GST_STATE_PLAYING)) == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (pipeline);
    return -1;
  }

  cout << __FILE__ << __LINE__ << endl;

  g_main_loop_run(loop);
  gdk_threads_enter();
  gtk_main ();
  gdk_threads_leave; 
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref (gstbus);
  gst_object_unref(GST_OBJECT(pipeline));

  return 0;
}