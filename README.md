# Qualcomm® QCS610 SoC Open Kit smarttraffic-demo2.0 Developer documentation

## Introduce

This project is based on a open source [EasyPR](https://gitee.com/easypr/EasyPR) and Qualcomm Neural processing SDK for AI.Using the Qualcomm® QCS610 SoC Open Kit and IMX415 camera module. Then,Its powerful computing ability will been show by the license plate recognition process is completed that combine with Qualcomm Neural processing SDK for AI, opencv,SVM and Gstreamer.The EasyPR license plate recognition engine is transplanted to the Qualcomm® QCS610 SoC development board  to display the rich application scenes of Qualcomm® QCS610 SoC.At the same time,UI is developed UI by GTK to display the progress of license plate recognition.

The project was built in x86 host with across complier tool and has been tested in Qualcomm® QCS610 SoC device.

<center>Qualcomm® QCS610 SoC Development board</center>

![Turbox C610  Development board](./res/QCS610.png)

## Materials and Tools used for the project

1. Hardware materials

Except for the development board,The following hardware materials are also needed:

* Type-C usb line

using the usb line to develop on Qualcomm® QCS610 SoC development board.

![usb line](./res/usb.png )

* Charger

Direct power supply for Qualcomm® QCS610 SoC development board

![charger](./res/charger.jpg )

* DP-Line

using the universal dp line to connect LED displayer to  Qualcomm® QCS610 SoC development board.

![DP-Line](./res/dpline.jpg)

* LED-Displayer

using a LED Displayer to display the smarttraffic-demo2.0 interface from Qualcomm® QCS610 SoC development board.

![LED-Displayer](./res/LED-Displayer.png)

## Compile

The compilation of the whole project is based on the yocto compilation tool, so you need to write some .bb and .conf files according to the specification. The traffic_0.1.bb example is as follows:

```
inherit cmake

DESCRIPTION = "smart traffic-demo 2.0"
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
```

Please refer to [the official Manual of Yocto](https://www.yoctoproject.org) for how to add layers,write layer.conf .Then,excute the command as follows:

```
bitbake smarttraffic-demo2.0
```

you will get a  executable bin file named smarttraffic-demo2.0.Move it to the root of the source code.Next,push the whole project code to Qualcomm® QCS610 SoC device`s dir /data/.

```
adb root && adb disable-verity && adb reboot

adb root &&adb remount && adb shell mount -o remount,rw /

adb push xxx/xxx/sourcepath /data

adb shell

cd data/smarttraffic-demo2.0
```

## Configure Weston and Usage

### 1. Configure Weston

To Download *Turbox-C610-aarch64_AI_Demo_Firmware.tgz*, go to: TBU

Push the *firmware package* to target device's */data/* directory.

```
$ adb root
$ adb disable-verity && adb reboot
$ adb root && adb shell mount -o remount,rw /
$ adb push Turbox-C610-aarch64_AI_Demo_Firmware.tgz  /data/
$ adb shell
$ tar -zxvf /data/ Turbox-C610-aarch64_AI_Demo_Firmware.tgz  -C /data/
```

Unpack *Turbox-C610-aarch64_Weston_DP_Firmware.tgz* under root directory to enable weston output to DP.

```
$ tar -zxvf /data/ Turbox-C610-aarch64_AI_Demo_Firmware/aarch64-weston-dp.tgz -C /
```

### 2. Usage

This project only provides a simple command line interface.

run smarttraffic-demo2.0：

Run the **smarttraffic-demo2.0** app.the demo video will play on the DP screen.

```
$ cd /data/<source root dir>
$ ./weston_dp_client smarttraffic-demo2.0
```

**weston_dp_client :**

```


#!/bin/sh

echo "=====Configure Weston environment====="
mount -o remount,rw /
killall weston
#mkdir /usr/bin/weston_socket
export XDG_RUNTIME_DIR=/dev/socket/weston
#mkdir --parents XDG_RUNTIME_DIR
chmod 0700 $XDG_RUNTIME_DIR
cd /usr/bin
./weston --tty=1 --device=msm_drm --idle-time=0 &
sleep  2 

echo "=====Show the Weston flower====="
sleep  1 
sh weston-flower &
sleep  3 

if [[ $1 = "smarttraffic-demo2.0" ]]; then
    cd /data/camera/
    echo "=====Show the smart traffic demo 2.0====="
    sleep  1 
    ./smarttraffic-demo2.0 &
    sleep  1800 
    killall smarttraffic-demo2.0
elif [[ $1 = "decode" ]]; then  
    echo "=====Show the Multi Decode====="
    sleep  1 
    multi-decoder 6 /data/Turbox-C610-aarch64_AI_Demo_Firmware/config.ini &
    sleep  20 
    killall multi-decoder
else   
    echo "=====Missing parameter====="
    sleep  2 

fi

echo "=====Turn off  Weston display====="
killall weston

```
