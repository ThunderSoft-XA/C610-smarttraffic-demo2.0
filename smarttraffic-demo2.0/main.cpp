#include "refreshdata.hpp"

#include "easypr/core/plate_locate.h"
#include "easypr/core/plate_judge.h"
#include "easypr/core/plate.hpp"
#include "easypr/core/plate_recognize.h"
#include "easypr/core/plate_detect.h"
#include "easypr/core/SNPEPlate.hpp"


#include <mcheck.h>

#include <omp.h>

using namespace camera;
using namespace easyui;
using namespace easypr;
using namespace cv;

static SNPEPlate *snpecheck;

static GstFlowReturn on_new_sample (GstAppSink *appsink, RefreshObject *RefreshObject);
void plate_recognize(RefreshData *myFreshData,cv::Mat &src);
void refreshUI (EasyUI *usrUI,RefreshData *usrData);

int main(int argc,char **argv)
{
    /* start up malloc trace record log */
    setenv("MALLOC_TRACE", "mtrace.log", 1);
    mtrace();

    GMainLoop *loop;
    loop = g_main_loop_new(NULL, FALSE);

    cout << __FILE__ << "====" <<  __LINE__ << endl;
    /* start up g_object thread for refresh thread across thread*/ 
    if ( !g_thread_supported() ) {
        #ifdef DEBUG
            printf("[%s]--Init g_thread./r/n", __FUNCTION__);
        #endif // DEBUG
        g_thread_init(NULL);
        gdk_threads_init();
    }

    cout << __FILE__ << "====" <<  __LINE__ << endl;
    /* gtk gdk gst init */
    gtk_init(&argc,&argv);
    gdk_init(&argc,&argv);
    gst_init(&argc,&argv);
    
    RefreshObject *myObject;
    RefreshData *myRefreshData;

again :
#pragma omp parallel
#pragma omp section
{
    #pragma omp  section
        myObject->dstUI = new EasyUI();
        myObject->dstUI->createUI();
        cout << __FILE__ << "====" <<  __LINE__ << endl;
        myObject->dstUI->buildSignal("recognition-signal");
        myObject->dstUI->buildSignal("refresh-signal");
        myObject->dstUI->showUI();
        cout << __FILE__ << "====" <<  __LINE__ << endl;

    #pragma omp section
        snpecheck = new SNPEPlate();
        cout << __FILE__ << "====" <<  __LINE__ << endl;
        snpecheck->init();
    
    #pragma omp section
        myObject->srcCamera = new Camera();
        do {
            if(myObject->srcCamera->checkElements()) {
                myObject->srcCamera->setElementsPro();
                if(myObject->srcCamera->linkPipeline()) {
                    assert( myObject->srcCamera->setPipelineStatus(GST_STATE_NULL) != false);
                }
            }
            cout << __FILE__ << "====" <<  __LINE__ << endl;
        } while ( !myObject->srcCamera->flags);
}

    if(myObject->srcCamera->flags == false || myObject->dstUI->flags == false || snpecheck->flags == false) {
        goto again;
    }
    cout << __FILE__ << "====" <<  __LINE__ << endl;
#pragma  omp parallel for ordered
{
    g_signal_connect(myObject->srcCamera->sink,"new-sample",
        G_CALLBACK(on_new_sample),myObject);
    cout << __FILE__ << "====" <<  __LINE__ << endl;
	#pragma omp ordered
    g_signal_connect(myRefreshData,"recognition-signal",
        G_CALLBACK(plate_recognize),&myObject->imgMat);
    cout << __FILE__ << "====" <<  __LINE__ << endl;
    g_signal_connect(myObject->dstUI->window,"refresh-signal",
        G_CALLBACK(refreshUI),myRefreshData);
}
    myObject->srcCamera->signalHandle();
    assert (myObject->srcCamera->setPipelineStatus(GST_STATE_PLAYING) != false);

    cout << __FILE__ << "====" <<  __LINE__ << endl;
    g_main_loop_run(loop);
    gdk_threads_enter();
    gtk_main ();
    gdk_threads_leave; 
    myObject->srcCamera->~Camera();

    return 0;
}

static GstFlowReturn
on_new_sample (GstAppSink *appsink, RefreshObject *usrRefreshObject)
{
    static gint frame_idx = 0;
    cv::Mat srcmat;

    GstSample *sample = gst_app_sink_pull_sample(appsink);
    if(sample == NULL){
      if(gst_app_sink_is_eos(appsink))
        return GST_FLOW_EOS;
    }

    cout << __FILE__ << "====" <<  __LINE__ << endl;
    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstCaps *caps = gst_sample_get_caps(sample);
    GstMapInfo map_info;
    gint width,height;

    gst_buffer_map(buffer,&map_info,GST_MAP_READ);
    GstStructure *structure = gst_caps_get_structure(caps,0);
    gst_structure_get_int(structure,"width",&width);
    gst_structure_get_int(structure,"height",&height);

    cout << __FILE__ << "====" <<  __LINE__ << endl;
    frame_idx += 1;
    cairo_format_t format;
    cairo_surface_t *surface;
    format = CAIRO_FORMAT_ARGB32;
    surface = cairo_image_surface_create_for_data (map_info.data,
        format, width, height, cairo_format_stride_for_width(format,width));
    gtk_image_set_from_surface(GTK_IMAGE(usrRefreshObject->dstUI->videoDraw),surface);

    cout << __FILE__ << "====" <<  __LINE__ << endl;
    char filename[128] = {0};
    g_mkdir_with_parents("/data/camera/pictures",0700);
    snprintf(filename,sizeof(filename),"/data/camera/pictures/%06d.png",frame_idx);
    cairo_status_t st = cairo_surface_write_to_png(surface,filename);
    if(st != CAIRO_STATUS_SUCCESS){
      g_printerr("st:%s\n",cairo_status_to_string(st));
    }
  
  usrRefreshObject->imgMat = imread(filename);
  snpecheck->setSrcMat(usrRefreshObject->imgMat);
  cout << __FILE__ << "====" <<  __LINE__ << endl;
  if(snpecheck->snpeCheck() > 0 ) {
        //emit signal 
#pragma  omp parallel for ordered
        g_signal_emit_by_name (usrRefreshObject->dstUI->window, "recognition-signal");
        #pragma  omp ordered
        g_signal_emit_by_name (usrRefreshObject->dstUI->window, "refresh-signal");
        cout << __FILE__ << "====" <<  __LINE__ << endl;
  }

  cairo_surface_destroy(surface);
  gst_sample_unref(sample);
  return GST_FLOW_OK;
}

void refreshUI (EasyUI *usrUI,RefreshData *usrData)
{
    std::ostringstream licenseColor,license,pass;
    string tmp;
    if(usrData->licenseColor != NULL) {
        licenseColor << "<span foreground='green' underline='single' underline_color='blue' font_desc='12'>" << usrData->licenseColor << "</span>";
        if (strcmp(usrData->licenseColor,"YellowLicense")  == 0 ) {
            tmp = string("NONE");
        } else if (strcmp(usrData->licenseColor,"BlueLicense") == 0 ) {
            tmp = string("PASS");
        } else if (strcmp(usrData->licenseColor,"WhiteLicense") == 0 ) {
            tmp = string("PASS");
        }
        pass << "<span foreground='blue'  font_desc='24'>" << tmp.c_str() << "</span>";
    }  else {
        licenseColor << "<span foreground='green' underline='single' underline_color='blue' font_desc='24'>show color</span>";
    }

    cout << __FILE__ << "====" <<  __LINE__ << endl;
    if(usrData->license != NULL) {
        license <<  "<span foreground='green' underline='single' underline_color='blue' font_desc='12'>" << usrData->license << "</span>";
    } else {
        license <<  "<span foreground='green' underline='single' underline_color='blue' font_desc='12'>show license</span>";
    }

    gdk_threads_enter();
    gtk_label_set_markup(GTK_LABEL(usrUI->licenseLab),licenseColor.str().c_str());
    gtk_label_set_markup(GTK_LABEL(usrUI->psssLab),license.str().c_str());
    gtk_label_set_markup(GTK_LABEL(usrUI->psssLab),pass.str().c_str());
    gdk_threads_leave; 
}

void plate_recognize(RefreshData *myFreshData,cv::Mat &src) 
{
    cout << __FILE__ << "====" <<  __LINE__ << endl;
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
                myFreshData->licenseColor = strtok_r((char *)plateVec[j].getPlateStr().c_str(),":",&savestr);
                cout << __FILE__ << "substr  = " << myFreshData->licenseColor  << "---" << __LINE__ << endl;
                myFreshData->license = (char *)plateVec[j].getPlateStr().substr(strlen(substr)+2, plateVec[j].getPlateStr().length()).c_str();
            }
        }
    }
}

