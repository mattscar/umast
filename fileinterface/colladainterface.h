#ifndef COLLADAINTERFACE_H
#define COLLADAINTERFACE_H

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <iterator>

//#include "GL3/gl3.h"
#include <windows.h>
#include <GL/gl.h>
#include "pugixml.h"

struct SourceData {
  GLenum type;
  unsigned int size;
  unsigned int stride;
  void* data;
};

typedef std::map<std::string, SourceData> SourceMap;

struct ColGeom {
  std::string name;
  SourceMap map;
  GLenum primitive;
  int index_count;
  unsigned short* indices;
};

SourceData readSource(pugi::xml_node);

class ColladaInterface {

public:
  ColladaInterface() {};
  static void readGeometries(std::vector<ColGeom>*, const char*);
  static void freeGeometries(std::vector<ColGeom>*);
};

#endif

