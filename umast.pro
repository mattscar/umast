#-------------------------------------------------
#
# Project created by QtCreator 2012-05-02T05:41:37
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = umast
TEMPLATE = app

SOURCES += main.cc \
	main_window.cc \
    project_navigator/navigator_model.cc \
    project_navigator/project_navigator.cc \
    graphic_editor/tab_editor.cc \
    graphic_editor/gl_widget.cc \
    graphic_editor/gl_utils.cc \
    graphic_editor/cl_utils.cc \
    graphic_editor/gl_base.cc \ 	
    fileinterface/colladainterface.cpp \
    fileinterface/pugixml.cpp \
    property_browser/property_browser.cpp \
    property_browser/qteditorfactory.cpp \
    property_browser/qtpropertybrowser.cpp \
    property_browser/qtpropertybrowserutils.cpp \
    property_browser/qtpropertymanager.cpp \
    property_browser/qttreepropertybrowser.cpp

HEADERS  += main_window.h \
    project_navigator/node_data.h \
    project_navigator/navigator_model.h \
    project_navigator/project_navigator.h \
    project_navigator/node_data.h \
    graphic_editor/gl_widget.h \
    graphic_editor/gl_utils.h \
    graphic_editor/cl_utils.h \
    graphic_editor/tab_editor.h \
    graphic_editor/gl_base.h \ 
    graphic_editor/sphere_data.h \ 	
    fileinterface/colladainterface.h \
    fileinterface/pugixml.h \
    fileinterface/pugiconfig.h \
    property_browser/property_browser.h \
    property_browser/qteditorfactory.h \
    property_browser/qtpropertybrowser.h \
    property_browser/qtpropertybrowserutils_p.h \
    property_browser/qtpropertymanager.h \
    property_browser/qttreepropertybrowser.h	

LIBS += -lOpenCL -lOpenGL32 -lglew32d

QMAKE_INCDIR += $(INCLUDE_DIR) \
    $(GLEW_DIR)/include

QMAKE_LIBDIR += $(LIB_DIR) \
    $(GLEW_DIR)/lib
