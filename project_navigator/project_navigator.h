#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <QAbstractItemModel>
#include <QCoreApplication>
#include <QDragMoveEvent>
#include <QMainWindow>
#include <QTreeView>

#include "navigator_model.h"

class ProjectNavigator : public QTreeView {

  Q_OBJECT

public:
  ProjectNavigator(QMainWindow* parent = 0);

protected:
  void dragMoveEvent(QDragMoveEvent *event);

private:
  NavigatorModel *model;
};

#endif
