#ifndef GL_DEFINES_H
#define GL_DEFINES_H

// Number of vertex objects
#define NUM_VAOS 5
#define NUM_VBOS 5

// Base colors
#define COLOR_CLEAR 0.5f, 0.5f, 0.5f, 1.0f
#define COLOR_BASE glm::vec4(1.00f, 1.00f, 1.00f, 1.00f);
#define COLOR_GRID glm::vec4(0.70f, 0.70f, 1.00f, 0.75f);

// Axis colors
#define COLOR_XAXIS glm::vec4(1.00f, 0.25f, 0.25f, 1.0f);
#define COLOR_YAXIS glm::vec4(0.25f, 1.00f, 0.25f, 1.0f);
#define COLOR_ZAXIS glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);

// Spline colors
#define COLOR_CTRL glm::vec4(0.00f, 0.00f, 0.00f, 1.0f);
#define COLOR_SPLINE glm::vec4(0.75f, 0.10f, 0.10f, 1.0f);

#endif