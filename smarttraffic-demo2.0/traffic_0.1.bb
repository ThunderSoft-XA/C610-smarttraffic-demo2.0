inherit cmake

DESCRIPTION = "smarttraffic demo 2.0"
LICENSE = "BSD"
SECTION = "smarttraffic-demo2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/${LICENSE};md5=3775480a712fc46a69647678acb234cb"

# Dependencies.
DEPENDS := "opencv gtk+3 sqlite3 glib-2.0 dbus-glib glib-2.0-native"
DEPENDS += "zlib fontconfig cairo gst-plugins-base gconf libpng pango"
DEPENDS += "gstreamer1.0"
DEPENDS += "gstreamer1.0-plugins-base"
DEPENDS += "gstreamer1.0-plugins-qti-oss-mlmeta"
DEPENDS += "gstreamer1.0-plugins-qti-oss-tools"
DEPENDS += "gstreamer1.0-rtsp-server"

EXTRA_OECONF += " --with-glib"
CPPFLAGS += " -I${STAGING_INCDIR}/glib-2.0"
CPPFLAGS += " -I${STAGING_LIBDIR}/glib-2.0/include"
CPPFLAGS += "-include glib.h"
CPPFLAGS += "-include glibconfig.h"
LDFLAGS += " -lglib-2.0"


FILESPATH =+ "${WORKSPACE}/video_ai/camera/bin/:"

SRC_URI = "file://camera/"
INSANE_SKIP_${PN}-dev += "ldflags dev-elf dev-deps"
PACKAGES = "${PN}-dbg ${PN} ${PN}-dev"
S = "/home/turbox/wuqx0806/cs-610/apps_proc/src/video_ai/camera/"

# Install directries.
INSTALL_INCDIR := "${includedir}"
INSTALL_BINDIR := "${bindir}"
INSTALL_LIBDIR := "${libdir}"

EXTRA_OECMAKE += ""

FILES_${PN} += "${INSTALL_BINDIR}"
FILES_${PN} += "${INSTALL_LIBDIR}"

SOLIBS = ".so*"
FILES_SOLIBSDEV = ""
