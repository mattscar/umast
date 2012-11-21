#ifndef GL_BASE_H
#define GL_BASE_H

#include <iostream>

#include <QGridLayout>
#include <QSlider>
#include <QWidget>

#include "tab_editor.h"

class GLBase : public QWidget {

  Q_OBJECT

public:
  GLBase(QWidget *parent = 0);
  TabEditor* tab_editor;

private:
  QSlider *x_slider, *y_slider;
};

#endif