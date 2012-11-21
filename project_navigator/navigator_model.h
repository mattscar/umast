#ifndef NAVIGATOR_MODEL_H_
#define NAVIGATOR_MODEL_H_

#include <QFile>
#include <QIcon>
#include <QString>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QCoreApplication>

#include "node_data.h"

class NavigatorModel : public QAbstractItemModel {

public:

  NavigatorModel(QObject *parent = 0);
  ~NavigatorModel();

  enum Roles {
    FileNameRole = Qt::DisplayRole,
    FileIconRole = Qt::DecorationRole,
    FilePathRole = Qt::StatusTipRole,
    FileTypeRole = Qt::UserRole + 1,
    FilePermissionRole = Qt::UserRole + 2
  };

  QModelIndex setWorkspace(const QString&);
  QModelIndex index(int, int, const QModelIndex&) const;
  QModelIndex parent(const QModelIndex&) const;
  int rowCount(const QModelIndex&) const;
  int columnCount(const QModelIndex&) const;
  bool hasChildren(const QModelIndex &parent) const;
  bool canFetchMore(const QModelIndex &parent) const;
  void fetchMore(const QModelIndex &parent);
  QVariant data(const QModelIndex&, int) const;

private:
  // Workspace information
  QString workspace_path;
  QModelIndex workspace_index;
  QString icon_dir;
};

#endif
