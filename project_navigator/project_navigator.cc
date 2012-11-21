#include "project_navigator.h"
#include <QDebug>

ProjectNavigator::ProjectNavigator(QMainWindow *parent) : QTreeView(parent) {

  model = new NavigatorModel;
//  model->setSupportedDragActions(Qt::CopyAction);
  QString workspace_dir = QCoreApplication::applicationDirPath() + "/workspace";
  QModelIndex workspace_index = model->setWorkspace(workspace_dir);
  setHeaderHidden(true);
  setModel(model);
  setRootIndex(workspace_index);


//  setDragEnabled(true);
//  setDragDropMode(QAbstractItemView::DragDrop);
}

void ProjectNavigator::dragMoveEvent(QDragMoveEvent* event) {
  event->accept();
}
