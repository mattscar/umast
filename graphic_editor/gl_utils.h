#ifndef GL_UTILS_H
#define GL_UTILS_H

#include <QApplication>

#include "gl_widget.h"

class GLUtils {

public:
  static void InitGL(GLWidget*);
  static GLuint InitShaders(GLWidget*);
  static void InitUniforms(GLWidget*, GLuint);
  static void DrawBase(GLWidget*);
  static void DrawGrid(GLWidget*);
  static void UpdateControlPoints(GLWidget*);
  static void UpdateKnots(GLWidget*);
  static void UpdateSpline(GLWidget*);
};

#endif
