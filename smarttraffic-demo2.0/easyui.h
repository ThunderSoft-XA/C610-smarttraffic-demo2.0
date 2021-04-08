#ifndef __EASY_UI_H__
#define __EASY_UI_H__

#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <cairo/cairo.h>

namespace easyui {

class EasyUI {

public :
    EasyUI();
    ~EasyUI();
    
    GtkWindow *window;
    GtkLabel *licenseLab,*psssLab,*resultLab;
    GtkWidget *videoDraw;

    void createUI();
    void showUI();
    // void refreshUI (RefreshData usrData,RefreshObject usrObject);
    void buildSignal(char *signalName);

    bool flags;

private :
    GtkBox *Hbox,*Vbox;
    guint signalID;

    void createController();


};

}


#endif // !__EASY_UI_H__