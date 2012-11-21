#ifndef CL_UTILS_H
#define CL_UTILS_H

#include <iostream>

#include "gl_widget.h"

class CLUtils {

public:
  static void InitCL(GLWidget*);
  static void ExecuteKernel(GLWidget*);
  static void UpdateVertices(GLWidget*);
};

#endif
