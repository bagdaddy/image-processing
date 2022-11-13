TEMPLATE = app
DEPENDPATH += ./
INCLUDEPATH += ./ ../ ../io_skeleton/imagelib ## path to imagelib
RESOURCES += 

CONFIG += qt c++11 debug_and_release
QT += widgets gui charts

TARGET = viewer 
LIBS+= ~/studies/image_processing/io_skeleton/imagelib/libimagelib.so -ltiff

HEADERS += qtimageviewer.hpp

SOURCES += viewer.cpp \
	qtimageviewer.cpp 

## add call to compile image library
imagelib.target = myimagelibtarget
imagelib.commands = make -w -C ../io_skeleton/imagelib/;
imagelib.depends =
QMAKE_EXTRA_TARGETS += imagelib
PRE_TARGETDEPS += myimagelibtarget
