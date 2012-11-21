#include "navigator_model.h"
#include <QDebug>

// Constructs a file system model with the given parent.
NavigatorModel::NavigatorModel(QObject *parent)
: QAbstractItemModel(parent) {

  icon_dir = QCoreApplication::applicationDirPath() + "/images/";

  // Add roles
  QHash<int, QByteArray> roles = roleNames();
  roles.insertMulti(NavigatorModel::FileNameRole, "FileName");
  roles.insertMulti(NavigatorModel::FileIconRole, "FileIcon");
  roles.insertMulti(NavigatorModel::FilePathRole, "FilePath");
  roles.insert(NavigatorModel::FileTypeRole, "FileType");
  roles.insert(NavigatorModel::FilePermissionRole, "FilePermissions");
  setRoleNames(roles);
}

// Destroys this file system model.
NavigatorModel::~NavigatorModel() {}

//
QModelIndex NavigatorModel::setWorkspace(const QString &new_path) {

  QString long_path = QDir::fromNativeSeparators(new_path);
  QDir new_path_dir(long_path);
  if (!long_path.isEmpty()) {
    long_path = QDir::cleanPath(long_path);
    new_path_dir.setPath(long_path);
  }

  // We have a new valid root path
  workspace_path = long_path;
  NodeData* data = new NodeData(new_path_dir.dirName(), NodeData::WorkspaceType, workspace_path, 0);
  workspace_index = createIndex(0, 0, data);

  // Find projects in directory
  QFileInfoList info_list;
  QStringList file_list;
  QDir test_dir;
  NodeData* child_data;

  // Iterate through contents of workspace
  info_list = new_path_dir.entryInfoList();
  foreach(QFileInfo info, info_list) {
    bool isProject = false;

    // Determine if folder in directory is a project
    if (info.isDir()) {
      test_dir = QDir(info.absoluteFilePath());
      test_dir.setFilter(QDir::Files | QDir::Hidden);
      file_list = test_dir.entryList();
	  
	    // Search for .umast file
      foreach(QString i, file_list) {
        if(i == ".umast") {
          isProject = true;
          break;
        }
      }

	    // If a project, create a new NodeData
      if(isProject) {
        child_data = new NodeData(info.fileName(), NodeData::ProjectType, info.absoluteFilePath(), data);
        data->children.append(child_data);
      }
    }
  }

  return workspace_index;
}

QModelIndex NavigatorModel::index(int row, int column, const QModelIndex &parent = QModelIndex()) const {

  // Check for valid index
  if (!parent.isValid() || row < 0 || column > 0)
    return QModelIndex();

  // Access NodeData corresponding to parent index
  NodeData *node = static_cast<NodeData*>(parent.internalPointer());
  
  // Create index for child NodeData with the given row
  return createIndex(row, 0, node->children[row]);
}

QModelIndex NavigatorModel::parent(const QModelIndex& index) const {

  // Check for valid index
  if (!index.isValid())
    return QModelIndex();

  // Check for workspace node
  NodeData *node = static_cast<NodeData*>(index.internalPointer());
  Q_ASSERT(node != 0);
  if(node->file_type == NodeData::WorkspaceType)
    return QModelIndex();

  // Access parent node
  NodeData *parent_node = static_cast<NodeData*>(node->parent);
  if(parent_node == 0)
    return QModelIndex();

  // Access parent's parent
  NodeData *grand_parent_node = static_cast<NodeData*>(parent_node->parent);
  if(grand_parent_node == 0)
    return workspace_index;

  // Find position of parent in grandparent's vector
  int row = grand_parent_node->children.indexOf(parent_node);

  return createIndex(row, 0, parent_node);
}

int NavigatorModel::rowCount(const QModelIndex &index = QModelIndex()) const {

  // Check for valid index
  if (!index.isValid())
    return 0;

  // Access the NodeData corresponding to the parent
  NodeData *node = static_cast<NodeData*>(index.internalPointer());

  // Return the number of children
  return node->children.size();
}

int NavigatorModel::columnCount(const QModelIndex &parent = QModelIndex()) const {
  return 1;
}

bool NavigatorModel::hasChildren(const QModelIndex &index = QModelIndex()) const {

  // Check for valid index
  if (!index.isValid())
    return false;

  // Access the NodeData corresponding to the index
  NodeData *node = static_cast<NodeData*>(index.internalPointer());

  // Only the workspace, projects, and folders can have children
  if (node->file_type == NodeData::ProjectType || node->file_type == NodeData::WorkspaceType)
    return true;
  else
    return false;
}

bool NavigatorModel::canFetchMore(const QModelIndex &parent) const {

  // Check for valid index
  if (!parent.isValid())
    return false;

  // Access the NodeData corresponding to the index
  NodeData *node = static_cast<NodeData*>(parent.internalPointer());
  return node->canFetch;
}

void NavigatorModel::fetchMore(const QModelIndex &parent) {

  // Check for valid index
  if (!parent.isValid())
    return;

  // Access the NodeData corresponding to the index
  NodeData *node = static_cast<NodeData*>(parent.internalPointer());
  if (!node->canFetch)
    return;

  // Ensure that this directory can't be read from
  node->canFetch = false;

  // Create a QFileInfo for each file in the parent directory
  QFileInfoList info_list = QDir(node->file_path).entryInfoList();

  NodeData *child_data;
  
  // Iterate through the QFileInfo objects
  foreach(QFileInfo info, info_list) {
  
    // Don't include files that start with a period, including . and ..
    if(info.fileName().startsWith('.'))
      continue;
	  
    // Create a NodeData for each child: a folder or design file
    if(info.isDir())
      child_data = new NodeData(info.fileName(), NodeData::FolderType, info.absoluteFilePath(), node);
    else
      child_data = new NodeData(info.fileName(), NodeData::DesignFileType, info.absoluteFilePath(), node);
    node->children.append(child_data);
  }
}

// Returns data with a given index and role
QVariant NavigatorModel::data(const QModelIndex &index, int role) const {

  // Verify valid index
  if (!index.isValid() || index.model() != this || index.column() > 0)
    return QVariant();

  // Access node data for the QModelIndex
  NodeData *node = static_cast<NodeData*>(index.internalPointer());

  // Provide data depending on the specified role
  switch (role) {

  // Print file name
  case NavigatorModel::FileNameRole:
    return node->file_name;

    // Get icon
  case NavigatorModel::FileIconRole: {
    QIcon icon;
    switch (node->file_type) {
      case NodeData::ProjectType: icon = QIcon(icon_dir + "project.png"); break;
      case NodeData::FolderType: icon = QIcon(icon_dir + "folder.png"); break;
      case NodeData::DesignFileType: icon = QIcon(icon_dir + "designfile.png"); break;
      case NodeData::SimFileType: icon = QIcon(icon_dir + "simfile.png"); break;
      default: icon = QIcon(icon_dir + "file.png"); break;
    }
    return icon;
  }

  // Get path
  case NavigatorModel::FilePathRole:
    return node->file_path;

    // Get type
  case NavigatorModel::FileTypeRole:
    return node->file_type;
  }
  return QVariant();
}


/*
#ifdef Q_OS_WIN32
static QString GetLongPathName(const QString &strShortPath)
{
  if (strShortPath.isEmpty()
      || strShortPath == QLatin1String(".") || strShortPath == QLatin1String(".."))
    return strShortPath;
  if (strShortPath.length() == 2 && strShortPath.endsWith(QLatin1Char(':')))
    return strShortPath.toUpper();
  const QString absPath = QDir(strShortPath).absolutePath();
  if (absPath.startsWith(QLatin1String("//"))
      || absPath.startsWith(QLatin1String("\\\\"))) // unc
    return QDir::fromNativeSeparators(absPath);
  if (absPath.startsWith(QLatin1Char('/')))
    return QString();
  const QString inputString = QLatin1String("\\\\?\\") + QDir::toNativeSeparators(absPath);
  QVarLengthArray<TCHAR, MAX_PATH> buffer(MAX_PATH);
  DWORD result = ::GetLongPathName((wchar_t*)inputString.utf16(),
      buffer.data(),
      buffer.size());
  if (result > DWORD(buffer.size())) {
    buffer.resize(result);
    result = ::GetLongPathName((wchar_t*)inputString.utf16(),
        buffer.data(),
        buffer.size());
  }
  if (result > 4) {
    QString longPath = QString::fromWCharArray(buffer.data() + 4); // ignoring prefix
    longPath[0] = longPath.at(0).toUpper(); // capital drive letters
    return QDir::fromNativeSeparators(longPath);
  } else {
    return QDir::fromNativeSeparators(strShortPath);
  }
}
#endif
*/
