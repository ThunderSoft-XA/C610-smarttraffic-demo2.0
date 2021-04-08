#include "easyui.h"
#include <iostream>
#include <string>
#include <sstream>


using namespace std;
using namespace easyui;

EasyUI::EasyUI()
{
    flags = false;
    gtk_init(NULL,NULL);
    createController();
    cout << __FILE__ << "====" <<  __LINE__ << endl;
}

void EasyUI::createController()
{
    window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    Hbox =  GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6));
    Vbox =  GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 6));
    videoDraw = GTK_WIDGET(gtk_image_new());
    licenseLab = GTK_LABEL(gtk_label_new(""));
    resultLab = GTK_LABEL(gtk_label_new(""));
    psssLab = GTK_LABEL(gtk_label_new(""));
}


void EasyUI::createUI()
{
    /*setup window */
    GtkStyle  *style = gtk_rc_get_style(GTK_WIDGET( window) );
    pango_font_description_set_family(style->font_desc, "Stsong" );
    gtk_widget_set_style( GTK_WIDGET(window), style );
    //gtk_window_resize (window,800,600);
    gtk_container_set_border_width (GTK_CONTAINER (window), 6);
    gtk_window_set_default_size(GTK_WINDOW(window),900,600);

    gtk_widget_set_size_request(GTK_WIDGET(videoDraw),640,480);

    
    //gtk_widget_set_size_request(GTK_WIDGET(licenseLab),250,220);
    gtk_label_set_lines (licenseLab,1);
    gtk_label_set_line_wrap (GTK_LABEL(licenseLab),TRUE);
    gtk_label_set_markup(GTK_LABEL(licenseLab),"<span foreground='green' underline='single' underline_color='blue' font_desc='24'>show license</span>");
    
    gtk_label_set_lines (resultLab,1);
    gtk_label_set_markup(GTK_LABEL(licenseLab),"<span foreground='green' underline='single' underline_color='blue' font_desc='24'>show license</span>");

    gtk_label_set_lines (psssLab,1);
    gtk_label_set_markup(GTK_LABEL(psssLab),"<span foreground='blue'  font_desc='24'>PASS</span>");

    /* add UI elements to box*/
    gtk_box_pack_start(Vbox,GTK_WIDGET(licenseLab),TRUE,FALSE,2);
    gtk_box_pack_start(Vbox,GTK_WIDGET(resultLab),TRUE,FALSE,2);
    gtk_box_pack_start(Vbox,GTK_WIDGET(psssLab),TRUE,FALSE,2);
    gtk_box_pack_start(Hbox,GTK_WIDGET(videoDraw),TRUE,TRUE,2);
    gtk_box_pack_start(Hbox,GTK_WIDGET(Vbox),TRUE,TRUE,2);

    gtk_container_add (GTK_CONTAINER(window), GTK_WIDGET(Hbox));

#ifdef DEBUG
    std::cout << "all GTK elements create and add to window successfully" << std::endl;
#endif // DEBUG
    cout << __FILE__ << "====" <<  __LINE__ << endl;

}

void EasyUI::showUI()
{
    gtk_widget_show_all (GTK_WIDGET(window));
    flags = true;
}

#if 0
void EasyUI::refreshUI (RefreshData usrData,RefreshObject usrObject)
{
    ostringstream licenseColor,license,pass;
    string tmp;
    if(usrData.licenseColor != NULL) {
        licenseColor << "<span foreground='green' underline='single' underline_color='blue' font_desc='12'>" << usrData.licenseColor << "</span>";
        if (strcmp(usrData.licenseColor,"YellowLicense")  == 0 ) {
            tmp = string("NONE");
        } else if (strcmp(usrData.licenseColor,"BlueLicense") == 0 ) {
            tmp = string("PASS");
        } else if (strcmp(usrData.licenseColor,"WhiteLicense") == 0 ) {
            tmp = string("PASS");
        }
        pass << "<span foreground='blue'  font_desc='24'>" << tmp.c_str() << "</span>";
    }  else {
        licenseColor << "<span foreground='green' underline='single' underline_color='blue' font_desc='24'>show color</span>";
    }
 
    if(usrData.license != NULL) {
        license <<  "<span foreground='green' underline='single' underline_color='blue' font_desc='12'>" << usrData.license << "</span>";
    } else {
        license <<  "<span foreground='green' underline='single' underline_color='blue' font_desc='12'>show license</span>";
    }

    gdk_threads_enter();
    gtk_label_set_markup(GTK_LABEL(usrObject.dstUI->licenseLab),licenseColor.str().c_str());
    gtk_label_set_markup(GTK_LABEL(usrObject.dstUI->psssLab),license.str().c_str());
    gtk_label_set_markup(GTK_LABEL(usrObject.dstUI->psssLab),pass.str().c_str());
    gdk_threads_leave; 
}
#endif // 0
void EasyUI::buildSignal(char *signalName)
{
    signalID = g_signal_new (signalName,   //signal name
                  G_TYPE_OBJECT,
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, 
                  NULL, 
                  g_cclosure_marshal_VOID__POINTER, 
                  G_TYPE_NONE, 
                  1, //parm num
                  G_TYPE_POINTER//parm type
                  );
}

EasyUI::~EasyUI()
{

}