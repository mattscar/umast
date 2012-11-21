#include "gl_base.h"

GLBase::GLBase(QWidget* parent) : QWidget(parent) {

 QGridLayout *grid = new QGridLayout(this);
 grid->setSpacing(2);

 // Add tab editor
 tab_editor = new TabEditor((QMainWindow*)this->parent());
 grid->addWidget(tab_editor, 0, 0);

 // Add scroll bar
 y_slider = new QSlider(Qt::Vertical);
 y_slider->setMinimum(-180);
 y_slider->setMaximum(180);
 y_slider->setValue(0);
 grid->addWidget(y_slider, 0, 1);
 //y_slider->setVisible(false);

  // Add scroll bar
 x_slider = new QSlider(Qt::Horizontal);
 x_slider->setMinimum(-180);
 x_slider->setMaximum(180);
 x_slider->setValue(0);
 grid->addWidget(x_slider, 1, 0);
 //x_slider->setVisible(false);

 // Connect signals and slots
  connect(x_slider, SIGNAL(sliderMoved(int)), 
          tab_editor->currentWidget(), SLOT(XSliderMoved(int)));
  connect(y_slider, SIGNAL(sliderMoved(int)), 
          tab_editor->currentWidget(), SLOT(YSliderMoved(int)));
};