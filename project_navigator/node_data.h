class NodeData {

public:

  enum FileType {WorkspaceType, ProjectType, FolderType, DesignFileType, SimFileType};

  QString file_name;
  FileType file_type;
  QString file_path;
  NodeData *parent;
  bool canFetch;
  QVector<NodeData*> children;

  // Constructor
  NodeData(QString name, FileType type, QString path, NodeData *ptr)
  : file_name(name), file_type(type), file_path(path), parent(ptr) {
    if(file_type == ProjectType || file_type == FolderType) {
      canFetch = true;
    }
    else {
      canFetch = false;
    }
  }
};


