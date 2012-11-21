#ifndef SPHEREDATA_H
#define SPHEREDATA_H

// OpenGL Math Library headers
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

struct SphereData {
  glm::vec3 center;
  float radius;
  glm::vec4 acceleration;
  glm::vec4 old_velocity;
  glm::vec4 new_velocity;
  glm::vec4 displacement;
};

struct SphereProperties {
  int id;
  QString filename;
  float mass;
  glm::vec3 color;
};

#endif
