#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include "../main_window.h"
#include "../spheredata.h"

#include "glew.h"

#include "../fileinterface/colladainterface.h"

#include <QGLWidget>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QColor>
#include <QPoint>
#include <QDebug>
//#include <QUrl>
#include <QFileInfo>
#include <QTime>
#include <QTimer>

#include <cfloat>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gl_defines.h"
#include "spline_data.h"

// OpenGL Math Library headers
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

// OpenCL headers
#include "CL/cl_gl.h"
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"

typedef CL_API_ENTRY cl_int
(CL_API_CALL *clGetGLContextInfoKHR_fn)(const cl_context_properties *properties,
                                        cl_gl_context_info param_name,
                                        size_t param_value_size,
                                        void *param_value,
                                        size_t *param_value_size_ret);

#define clGetGLContextInfoKHR clGetGLContextInfoKHR_proc
static clGetGLContextInfoKHR_fn clGetGLContextInfoKHR;

enum ToolType {SELECTION, CIRCLE, RECTANGLE};

enum ToolState {NO_CLICK, SURFACE_SELECT, POINT_SELECT};

class GLUtils;
class CLUtils;

class GLWidget : public QGLWidget {

  friend class GLUtils;
  friend class CLUtils;
  Q_OBJECT

public:
  GLWidget(QWidget *parent = 0);
  ~GLWidget();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  static void addLine(float* vertices, GLint num_vertices);
  void setLOD(int);
  void setWeight(double);

public slots:
  void ReadProperties();
  void XSliderMoved(int);
  void YSliderMoved(int);

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);

  // GLUtil functions
  std::string ReadFile(std::string str);
  void init_physics();
  void init_buffers(GLuint);

  // Mouse related events
  void mouseDoubleClickEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent* event);

  void dropEvent(QDropEvent *event);
  void dragEnterEvent(QDragEnterEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);

private:

  // The main window
  MainWindow *win;
  ToolState state;

  // Zoom values
  float zoom_vals[8];
  int zoom_index;

  // Sphere data
  struct SplineProperties spline_props;

  // OpenGL variables
  float *pick_result;                           // Pick selection result
  uint num_vbos, num_vaos;
  GLuint vao[NUM_VAOS], ibo, ubo, vbo[NUM_VBOS];            // OpenGL buffer objects
//  GLuint vao, ibo, ubo, vbos[2];            // OpenGL buffer objects
  size_t num_vertices, num_triangles;       // Number of vertices and triangles in the rendering
  GLint mvp_location;                       // Index of the MVP/color uniforms
  glm::mat4 modelview_matrix, mvp_matrix;   // The modelview matrices
  glm::mat4 rot_x, rot_y;
  glm::mat4 mvp_inverse; 
  glm::vec3 *vertex_data;
  float *normal_data;
  unsigned short* index_data;
  std::vector<ColGeom> geom_vec;            // Vector containing COLLADA meshes
  GLsizei count;
  GLvoid* indices;

  // Colors
  GLint color_location;                     // Location of the color uniform
  glm::vec4 color;                          // Current color
  glm::vec3 black, white;

  // Screen dimensions
  glm::vec2 screen_dim;

  // Grid
  std::vector<float> grid_coords;         // Array containing coordinates of line endpoints
  int num_grid_points;

  // Spline
  std::vector<float> ctrl_points;
  std::vector<float> knots;
  int num_ctrl_points;
  int num_spline_points, num_knots;              // Number of points in the spline
  int order;
  float test[120];
  cl_mem test_buffer;
  int ctrl_index;                          // Index of the ctrl point selected by the user

  // OpenCL variables
  cl_platform_id platform;
  cl_device_id device;
  cl_context dev_context;
  cl_program program, pick_selection_program;
  cl_command_queue queue;
  cl_kernel kernel, update_kernel, motion_kernel, pick_selection_kernel;
  cl_mem knot_buffer, shared_buffers[2];
  size_t obj_local_size, obj_global_size, vertex_local_size, vertex_global_size, pick_local_size, pick_global_size;
  size_t num_groups;

  // Timing variables
  QTime* timer;
  int previous_time;

  // The current tool
  ToolType current_tool;
  ToolState current_state;

  // Mouse position
  int click_x, click_y;

private slots:

  // Idle function - execute kernels
  void UpdateVertices();

  // Make actions current
  void makeSelectionActionActive();
  void makeCircleActionActive();
  void makeRectActionActive();
};

#endif
