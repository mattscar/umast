#ifndef TAB_EDITOR_H
#define TAB_EDITOR_H

#include <string.h>

#include <QTabWidget>
#include <QMainWindow>
#include <QModelIndex>
#include <QDebug>

#include "../property_browser/qtpropertybrowser.h"

//#include "../io/collada/collada_brep_interface.h"

class TabEditor : public QTabWidget {

  Q_OBJECT

public:
  TabEditor(QMainWindow* parent = 0);

public slots:
  void createTab(const QModelIndex& index);
  void lod_changed(QtProperty*, int);
  void weight_changed(QtProperty*, double);

protected:
  bool eventFilter(QObject*, QEvent*);

private:
  QTabBar* tabbar;
};

#endif
