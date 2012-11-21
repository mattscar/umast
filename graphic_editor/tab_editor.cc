#include "tab_editor.h"
#include "gl_widget.h"
#include "../main_window.h"

TabEditor::TabEditor(QMainWindow *parent) : QTabWidget(parent) {
  addTab(new GLWidget(this), tr("example.dae"));
  tabbar = this->tabBar();
  tabbar->installEventFilter(this);
}

void TabEditor::createTab(const QModelIndex& index) {

  if(index.data(NavigatorModel::FileTypeRole) == NodeData::DesignFileType  ) {
//    addTab(new GLWidget(this), index.data(Qt::DisplayRole).toString());
    qDebug() << index.data(NavigatorModel::FilePathRole).toString();
//    ColladaBREPInterface::read_geometries((Solid*)NULL, index.data(NavigatorModel::FilePathRole).toString().toStdString().c_str());
    setCurrentIndex(count()-1);
  }
}

void TabEditor::lod_changed(QtProperty* prop, int val) {
  GLWidget* widget = (GLWidget*)currentWidget();
  if(widget != NULL) {
    widget->setLOD(val);
  }
}

void TabEditor::weight_changed(QtProperty* prop, double val) {
  GLWidget* widget = (GLWidget*)currentWidget();
  if(widget != NULL) {
    widget->setWeight(val);
  }
}

bool TabEditor::eventFilter(QObject* obj, QEvent* e) {

  bool result = QObject::eventFilter(obj, e);

  if (obj == tabbar && e->type() == QEvent::MouseButtonDblClick) {

    ((MainWindow*)(parent()->parent()))->MaximizeEditor();
    QMouseEvent* me = static_cast<QMouseEvent*>(e);
    int index = tabbar->tabAt(me->pos());
    if (index == -1)
      return result;
    this->showFullScreen();

    return true;
  }

  return result;
}

/*
GLWidget* getCurrentTab() {
  return 
}
*/