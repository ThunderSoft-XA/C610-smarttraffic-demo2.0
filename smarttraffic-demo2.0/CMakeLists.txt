cmake_minimum_required (VERSION 3.4.1)
set(CMAKE_VERBOSE_MAKEFILE on)
project (trafficUI)

find_package(PkgConfig REQUIRED)
set(OpenCV_DIR /usr/local/opencv/share/OpenCV)
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "OpenCV Version: ${OpenCV_VERSION}")
message(STATUS "OpenCV Libraries: ${OpenCV_LIBS}")
message(STATUS "OpenCV Include path: ${OpenCV_INCLUDE_DIRS}")

pkg_check_modules(GLIB REQUIRED glib-2.0)

find_program(SQLite sqlite3)

# find_package(GTK3 REQUIRED gtk+-3.0)
# include_directories (${GTK3_INCLUDE_DIRS})
# link_directories (${GTK3_LIBRARY_DIRS})


# gstreamer
set(GSTREAMER_INCLUDE_DIRS "/usr/include/gstreamer-1.0;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include/glib-2.0;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include/glib-2.0/include;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include/gobject;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include/gst;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "/usr/lib/gstreamer-1.0/include;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "/usr/include/glib-2.0;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "/usr/lib/glib-2.0/include;")
set(GSTREAMER_LIBS  "gstapp-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstbase-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstrtsp-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstreamer-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstsdp-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gio-2.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gobject-2.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "glib-2.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstvideo-1.0;")
set(GSTREAMER_LIBRARY_DIRS  "/usr/lib/;")

message(STATUS "    GST DEBUG:###########################")
message(STATUS "    libraries: ${GSTREAMER_LIBS}")
message(STATUS "    include path: ${GSTREAMER_INCLUDE_DIRS}")
message(STATUS "    library path: ${GSTREAMER_LIBRARY_DIRS}")

#SNPE
set(SNPE_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/snpe/include/zdl")
set(SNPE_LIBS  "SNPE")
set(SNPE_LIBRARY_DIRS  "${CMAKE_SOURCE_DIR}/libs/snpe")
set(GLIB_LIBRARY_DIRS "${CMAKE_SOURCE_DIR}/libs")

message(STATUS "    SNPE DEBUG:###########################")
message(STATUS "    libraries: ${SNPE_LIBS}")
message(STATUS "    include path: ${SNPE_INCLUDE_DIRS}")
message(STATUS "    library path: ${SNPE_LIBRARY_DIRS}")

#OPENGL
set(OPENGL_LIBS  "GL;GLU;glut;GLEW;")

# sub directories
add_subdirectory(thirdparty)

aux_source_directory(${CMAKE_SOURCE_DIR} DIR_SRCS)

include_directories(
        ${CMAKE_SOURCE_DIR}/
        ${SNPE_INCLUDE_DIRS} /
        ${OpenCV_INCLUDE_DIRS}
        ${GLIB_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include/
        ${CMAKE_SOURCE_DIR}/include/easypr/
        ${CMAKE_SOURCE_DIR}/include/gdk/
        ${CMAKE_SOURCE_DIR}/include/glib-2.0/
        ${CMAKE_SOURCE_DIR}/include/gobject
        ${CMAKE_SOURCE_DIR}/include/gst
        ${CMAKE_SOURCE_DIR}/include/gtk
        ${CMAKE_SOURCE_DIR}/include/imgproc
        ${CMAKE_SOURCE_DIR}/include/unix-print/
        )

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )

link_directories(
        ${OpenCV_LIBRARY_DIRS}/
        ${GSTREAMER_LIBRARY_DIRS}/
        ${SNPE_LIBRARY_DIRS}/
        ${GLIB_LIBRARY_DIRS}
        )
# sources to be compiled
set(SOURCE_FILES
        src/core/core_func.cpp
        src/core/chars_identify.cpp
        src/core/chars_recognise.cpp
        src/core/chars_segment.cpp
        src/core/feature.cpp
        src/core/plate_detect.cpp
        src/core/plate_judge.cpp
        src/core/plate_locate.cpp
        src/core/plate_recognize.cpp
        src/core/params.cpp
        src/core/SNPEPlate.cpp

        src/train/ann_train.cpp
        src/train/annCh_train.cpp
        src/train/svm_train.cpp
        src/train/train.cpp
        src/train/create_data.cpp

        src/util/util.cpp
        src/util/program_options.cpp
        src/util/kv.cpp

        #sqliteTest.cpp
        #nativeCamera.cpp
        configenv.cpp
        camera.cpp
        easyui.cpp
        )

# pack objects to static library
add_library(easypr STATIC ${SOURCE_FILES})

add_executable(
        ${PROJECT_NAME} 
        #${DIR_SRCS}
        #${CMAKE_SOURCE_DIR}/test.cpp
        #${CMAKE_SOURCE_DIR}/player.cpp
        #${CMAKE_SOURCE_DIR}/deal.cpp
        ${CMAKE_SOURCE_DIR}/main.cpp
        )

target_link_libraries(
    ${PROJECT_NAME}
    easypr
    thirdparty
    ${OpenCV_LIBS}
    ${GSTREAMER_LIBS}
    ${SNPE_LIBS}
    ${CMAKE_SOURCE_DIR}/libs/
    ${CMAKE_SOURCE_DIR}/libs/libgtk-3.so.0
    ${CMAKE_SOURCE_DIR}/libs/libgtk-3.so.0.1800.8
    ${CMAKE_SOURCE_DIR}/libs/libglib-2.0.so.0
    ${CMAKE_SOURCE_DIR}/libs/snpe/libSNPE.so
    # zlib
    z
)


# c++11 required
set(CMAKE_BUILD_TYPE "DEBUG")
# MESSAGE(${CMAKE_BINARY_DIR}/../)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread -lrt -lm -lcairo-gobject -lcairo -lpango-1.0 -lpangocairo-1.0 -lpangoft2-1.0 -lgstqtivideobase -lgdk-3 -lgdk_pixbuf-2.0  -lglib-2.0 -ldl -lsqlite3 -latomic -lsync -DANDROID_TARGET -DSNPE_ENGINE_SUPPORTED -DLOG_SWITCH=0")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs/)

