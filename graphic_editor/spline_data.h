#ifndef SPLINE_DATA_H
#define SPLINE_DATA_H

// OpenGL Math Library headers
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

struct SplineProperties {
  int lod;
  int ctrl;
  glm::vec4 coords;
};

#endif
