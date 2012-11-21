#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

#define MIN_Z 2.5f
#define MAX_Z 20.0f

#include "gl_widget.h"
#include "gl_utils.h"
#include "cl_utils.h"

// Pick-selection information
unsigned int selected_object = UINT_MAX;
int collide = 0;

GLWidget::GLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {

  makeCurrent();
  setAcceptDrops(true);

  // Configure tool settings
  win = (MainWindow*)(parent->parent());
  win->draw_group->setEnabled(true);
  connect(win->selection_action, SIGNAL(triggered()), this, SLOT(makeSelectionActionActive()));
  connect(win->circle_action, SIGNAL(triggered()), this, SLOT(makeCircleActionActive()));
  connect(win->rect_action, SIGNAL(triggered()), this, SLOT(makeRectActionActive()));
  win->selection_action->setChecked(true);
  current_tool = SELECTION;

  // Configure tool state
  current_state = NO_CLICK;

  // Set spline points per control point
  spline_props.lod = 20;

  // Set grid spacing
  zoom_vals[0] = 1.0f;
  for(int i=1; i<8; i++)
    zoom_vals[i] = zoom_vals[i-1] * 2.0f;
  zoom_index = 4;

  // Set number of GL objects
  num_vaos = NUM_VAOS;
  num_vbos = NUM_VBOS;

  // Set spline data
  num_spline_points = 0;
  num_knots = 0;
  order = 4;

  // Initialize control points
  //InitControlPoints();
  num_ctrl_points = 0;

  // Read graphic data
  /*
  QString string = QCoreApplication::applicationDirPath() + "/sphere.dae";
  ColladaInterface::readGeometries(&geom_vec, string.toStdString().c_str());
  num_vertices = geom_vec[0].map["POSITION"].size/12;
  qDebug() << "Num of vertices:" << num_vertices;
  num_triangles = geom_vec[0].index_count/3;
  */
}

GLWidget::~GLWidget() {
  // Deallocate mesh data
 // ColladaInterface::freeGeometries(&geom_vec);

  // Deallocate arrays
  //delete(vertex_data);
  //delete(normal_data);
  //delete(index_data);

//  if(pick_result != NULL)
 //   delete(pick_result);

  // Deallocate OpenGL objects
//  glDeleteBuffers(1, &ibo);
  glDeleteBuffers(num_vbos, vbo);
  glDeleteBuffers(num_vaos, vao);
//  glDeleteBuffers(1, &ubo);

  // Deallocate OpenCL resources
  clReleaseKernel(kernel);
  clReleaseCommandQueue(queue);
  clReleaseProgram(program);
  clReleaseContext(dev_context);
  for(unsigned i=0; i<2; i++)
    clReleaseMemObject(shared_buffers[i]);
}

void GLWidget::XSliderMoved(int pos) {

  glm::mat4 tmp_matrix;

  rot_x = glm::rotate(glm::mat4(1.0f), (float)pos, glm::vec3(0.0f, 0.0f, 1.0f));
  tmp_matrix = mvp_matrix * rot_x * rot_y;
  mvp_inverse = glm::inverse(tmp_matrix);
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(tmp_matrix[0]));
  updateGL();
}

void GLWidget::YSliderMoved(int pos) {

  glm::mat4 tmp_matrix;

  rot_y = glm::rotate(glm::mat4(1.0f), (float)pos, glm::vec3(1.0f, 0.0f, 0.0f));
  tmp_matrix = mvp_matrix * rot_y * rot_x;
  mvp_inverse = glm::inverse(tmp_matrix);
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(tmp_matrix[0]));
  updateGL();
}

void GLWidget::ReadProperties() {

/*
  struct SphereData select_data;
  int err;

  if(selected_object < NUM_OBJECTS) {

    // Read object results
    err = clEnqueueReadBuffer(queue, sphere_memobj, CL_TRUE, selected_object * sizeof(select_data),
        sizeof(select_data), &select_data, 0, NULL, NULL);
    if(err < 0) {
      std::cerr << "Couldn't read the object information" << std::endl;
      exit(1);
    }

    win->propertybrowser->setSphereData(&select_data, &(sphere_props[selected_object]));
  }
*/
}

// Read a character buffer from a file
std::string GLWidget::ReadFile(std::string filename) {

  // Open the file
  std::ifstream ifs(filename, std::ifstream::in);
  if(!ifs.good()) {
    std::cerr << "Couldn't find the source file " << filename << std::endl;
    exit(1);
  }

  // Read file text into string and close stream
  std::string str((std::istreambuf_iterator<char>(ifs)),
                   std::istreambuf_iterator<char>());
  ifs.close();
  return str;
}

void GLWidget::UpdateVertices() {

//  CLUtils::UpdateVertices(this);
}

void GLWidget::initializeGL() {

  // Set background color
  glClearColor(COLOR_CLEAR);

  // Initialize shaders and buffers
  GLUtils::InitGL(this);
  CLUtils::InitCL(this);

  // Set knots for spline
  // GLUtils::CreateKnots(this);

  // Set aside memory for spline
  //GLUtils::UpdateSpline(this);

  // Update spline display
  //UpdateSpline();

  // Start main timer
  timer = new QTime();
  timer->start();
  previous_time = 0;

  // Start idle timer
  QTimer *idle_timer = new QTimer(this);
  connect(idle_timer, SIGNAL(timeout()), this, SLOT(UpdateVertices()));
  idle_timer->start();

  // Start property polling
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(ReadProperties()));
  timer->start(150);
}

void GLWidget::resizeGL(int width, int height) {

  glViewport(0, 0, (GLsizei)width, (GLsizei)height);

  // Set window dimensions
  screen_dim.x = (float)width/(2*zoom_vals[zoom_index]);
  screen_dim.y = (float)height/(2*zoom_vals[zoom_index]);
  
// Set new modelview matrix
  mvp_matrix = glm::ortho(-screen_dim.x, screen_dim.x,
    -screen_dim.y, screen_dim.y, MIN_Z, MAX_Z) * modelview_matrix * rot_x * rot_y;
//  mvp_matrix = glm::perspective(45.0f, width*zoom_vals[zoom_index]/height, MIN_Z, MAX_Z) * 
//     modelview_matrix * rot_x * rot_y;
  mvp_inverse = glm::inverse(mvp_matrix);
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp_matrix[0]));

  // Create new grid
  GLUtils::DrawGrid(this);
}

void GLWidget::paintGL() {

  glm::vec4 color;

  //glUniformMatrix4fv(color_location, 1, GL_FALSE, glm::value_ptr(black));

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //CLUtils::ExecuteKernel(this);
  /*
  glBindVertexArray(vao[3]);
  glDrawArrays(GL_LINES, 0, num_line_vertices);

  glBindVertexArray(vao[2]);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  */  

  color = COLOR_BASE;
  glUniform4fv(color_location, 1, glm::value_ptr(color));
  glBindVertexArray(vao[0]);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

  color = COLOR_XAXIS;
  glUniform4fv(color_location, 1, glm::value_ptr(color));
  glDrawArrays(GL_LINES, 4, 4);

  color = COLOR_YAXIS;
  glUniform4fv(color_location, 1, glm::value_ptr(color));
  glDrawArrays(GL_LINES, 8, 4);

  color = COLOR_ZAXIS;
  glUniform4fv(color_location, 1, glm::value_ptr(color));
  glDrawArrays(GL_LINES, 12, 4);

  color = COLOR_GRID;
  glUniform4fv(color_location, 1, glm::value_ptr(color));
  glPointSize(3.0f);
  glBindVertexArray(vao[1]);
  glDrawArrays(GL_POINTS, 0, num_grid_points);

  color = COLOR_CTRL;
  glUniform4fv(color_location, 1, glm::value_ptr(color));
  glPointSize(5.0f);
  glBindVertexArray(vao[2]);
  glDrawArrays(GL_POINTS, 0, num_ctrl_points);

  if(num_ctrl_points > 3) {
    color = COLOR_SPLINE;
    glUniform4fv(color_location, 1, glm::value_ptr(color));
    glBindVertexArray(vao[3]);
    glDrawArrays(GL_LINE_STRIP, 0, num_spline_points);
  }

  /*
  glBindVertexArray(vao);
  glUniform3fv(color_location, 1, &(white[0]));
  glDrawElements(geom_vec[0].primitive, count, GL_UNSIGNED_SHORT, NULL);
  */

  glBindVertexArray(0);
  swapBuffers();
}

QSize GLWidget::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize GLWidget::sizeHint() const {
  return QSize(400, 400);
}

// Respond to double-click events
void GLWidget::mouseDoubleClickEvent(QMouseEvent *event) {

  float x, y;

  if(event->buttons() && Qt::LeftButton) {

    // Compute origin (O) and direction (D) in object coordinates
    x = event->pos().x()/zoom_vals[zoom_index] - screen_dim.x;
    y = screen_dim.y - event->pos().y()/zoom_vals[zoom_index];

    ctrl_points.push_back(x);
    ctrl_points.push_back(y);
    ctrl_points.push_back(0.0f);
    ctrl_points.push_back(1.0f);
    num_ctrl_points++;

    GLUtils::UpdateControlPoints(this);

    if(num_ctrl_points > 3) {
      num_spline_points = num_ctrl_points * spline_props.lod;
      GLUtils::UpdateKnots(this);
      GLUtils::UpdateSpline(this);
      CLUtils::ExecuteKernel(this);
    }

    updateGL();
  }
}

void GLWidget::mousePressEvent(QMouseEvent *event) {

  float dist;
  float min_dist = FLT_MAX;

  if(event->buttons() && Qt::LeftButton) {

    // Compute origin (O) and direction (D) in object coordinates
    float x = event->pos().x()/zoom_vals[zoom_index] - screen_dim.x;
    float y = screen_dim.y - event->pos().y()/zoom_vals[zoom_index];

    // Check the distance between the mouse click and the control points
    for(std::vector<float>::size_type i = 0; i != ctrl_points.size(); i+=4) {
      dist = sqrt((x - ctrl_points[i]) * (x - ctrl_points[i]) + (y - ctrl_points[i+1]) * (y - ctrl_points[i+1]));
      if(dist < min_dist) {
        min_dist = dist;
        ctrl_index = i/4;
      }
    }

    // Check if the minimum distance is less than 0.2
    if(min_dist < 0.2f) {
      state = POINT_SELECT;
      spline_props.ctrl = ctrl_index;
      spline_props.coords = glm::vec4(ctrl_points[ctrl_index*4], ctrl_points[ctrl_index*4+1],
                                      ctrl_points[ctrl_index*4+2], ctrl_points[ctrl_index*4+3]);
      win->property_browser->setSplineData(&spline_props);
    }
    else {
      state = NO_CLICK;
    }

    /*
    glm::vec4 origin = mvp_inverse * glm::vec4((x-)/screen_dim.x,
        (screen_dim.y-y)/screen_dim.y, -1.0f, 1.0f);
        */
  }

/*
  glm::vec3 K, L, M, E, F, G, ans;
  int x, y, err;
  float t_test = 10000.0f;
  unsigned int i;

  if(event->type() == QEvent::MouseButtonPress) {

    // Compute origin (O) and direction (D) in object coordinates
    x = event->pos().x();
    y = event->pos().y();
    glm::vec4 origin = mvp_inverse * glm::vec4((x-half_width)/half_width,
        (half_height-y)/half_height, -1.0f, 1.0f);
    glm::vec4 dir = mvp_inverse * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    glm::vec4 O = glm::vec4(origin.x, origin.y, origin.z, 0.0f);
    glm::vec4 D = glm::vec4(glm::normalize(glm::vec3(dir.x, dir.y, dir.z)), 0.0f);

    // Create kernel arguments for the origin and direction
    err = clSetKernelArg(pick_selection_kernel, 4, 4*sizeof(float), glm::value_ptr(O));
    err |= clSetKernelArg(pick_selection_kernel, 5, 4*sizeof(float), glm::value_ptr(D));
    if(err < 0) {
      std::cerr << "Couldn't set a kernel argument: " << err << std::endl;
      exit(1);
    };

    // Complete OpenGL processing
    glFinish();

    // Acquire lock on OpenGL objects
    err = clEnqueueAcquireGLObjects(queue, 1, &vbo_memobj, 0, NULL, NULL);
    err |= clEnqueueAcquireGLObjects(queue, 1, &ibo_memobj, 0, NULL, NULL);
    if(err < 0) {
      std::cerr << "Couldn't acquire the GL objects for pick selection" << std::endl;
      exit(1);
    }

    // Execute kernel
    err = clEnqueueNDRangeKernel(queue, pick_selection_kernel, 1, NULL, &pick_global_size,
        &pick_local_size, 0, NULL, NULL);
    if(err < 0) {
      std::cerr << "Couldn't enqueue the pick-selection kernel" << std::endl;
      exit(1);
    }

    // Read pick_result results
    err = clEnqueueReadBuffer(queue, pick_buffer, CL_TRUE, 0,
        2 * num_groups * sizeof(float), pick_result, 0, NULL, NULL);
    if(err < 0) {
      std::cerr << "Couldn't read the pick-selection result buffer" << std::endl;
      exit(1);
    }

    // Deallocate and release objects
    clEnqueueReleaseGLObjects(queue, 1, &vbo_memobj, 0, NULL, NULL);
    clEnqueueReleaseGLObjects(queue, 1, &ibo_memobj, 0, NULL, NULL);

    // Check for smallest output
    for(i=0; i<2*num_groups; i+=2) {
      if(pick_result[i] < t_test) {
        t_test = pick_result[i];
        selected_object = (unsigned int)(floor((pick_result[i+1]+(i/2)*pick_local_size)/num_triangles));
      }
    }
    if(t_test == 1000) {
      selected_object = UINT_MAX;
    }
  }
*/

/*
  switch(current_tool) {
    case SELECTION:
      qDebug() << "Selection";
      break;

    case CIRCLE:
      switch(current_state) {
        case NO_CLICK:
          click_x = event->pos().x();
          click_y = event->pos().y();
          qDebug() << "State: FIRST_CLICK";
          current_state = FIRST_CLICK;
        break;

        case FIRST_CLICK:
          qDebug() << "State: NO_CLICK";
          current_state = NO_CLICK;
          win->selection_action->setChecked(true);
        break;
      }
      break;

    case RECTANGLE:
      qDebug() << "Rectangle";
      break;

    default:
      qDebug() << "Hey now";
      break;
  }
*/
}

void GLWidget::setLOD(int value) {
  spline_props.lod = value;
  num_spline_points = num_ctrl_points * value;
  GLUtils::UpdateSpline(this);
  CLUtils::ExecuteKernel(this);
  updateGL();
}

void GLWidget::setWeight(double value) {
  ctrl_points[ctrl_index*4+3] = value;
  GLUtils::UpdateControlPoints(this);
  CLUtils::ExecuteKernel(this);
  updateGL();
}


void GLWidget::mouseReleaseEvent(QMouseEvent *event) {

  if(state == POINT_SELECT) {
    state = NO_CLICK;
  }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {

  if(state == POINT_SELECT) {
    ctrl_points[ctrl_index*4] = (float)event->x()/zoom_vals[zoom_index] - screen_dim.x;
    ctrl_points[ctrl_index*4+1] = screen_dim.y - (float)event->y()/zoom_vals[zoom_index];
    spline_props.coords = glm::vec4(ctrl_points[ctrl_index*4], ctrl_points[ctrl_index*4+1],
                                      ctrl_points[ctrl_index*4+2], ctrl_points[ctrl_index*4+3]);
    win->property_browser->setSplineData(&spline_props);
    GLUtils::UpdateControlPoints(this);
    GLUtils::UpdateSpline(this);
    CLUtils::ExecuteKernel(this);
    updateGL();
  }
  /*
  int dx = event->x() - lastPos.x();
     int dy = event->y() - lastPos.y();

     if (event->buttons() & Qt::LeftButton) {
         setXRotation(xRot + 8 * dy);
         setYRotation  (yRot + 8 * dx);
     } else if (event->buttons() & Qt::RightButton) {
         setXRotation(xRot + 8 * dy);
         setZRotation(zRot + 8 * dx);
     }
     lastPos = event->pos();
     */
}

void GLWidget::wheelEvent(QWheelEvent* event) {

  // The delta() function returns the wheel turn in 1/8ths of a degree
  // A value of 120 = 120/8 = 15 degrees
  float delta = event->delta()/8.0f;
  
  if(delta < 0.0f && zoom_index > 0) {
    zoom_index--;
    screen_dim.x *= 0.5f;
    screen_dim.y *= 0.5f;
  }
  else if(delta > 0.0f && zoom_index < 7) {
    zoom_index++;
    screen_dim.x *= 2.0f;
    screen_dim.y *= 2.0f;
  }

  // Set new modelview matrix
  mvp_matrix = glm::ortho(-screen_dim.x, screen_dim.x,
    -screen_dim.y, screen_dim.y, MIN_Z, MAX_Z) * modelview_matrix * rot_x * rot_y;
  mvp_inverse = glm::inverse(mvp_matrix);
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp_matrix[0]));

  // Create new grid
  GLUtils::DrawGrid(this);
  updateGL();
}

void GLWidget::dropEvent(QDropEvent* event) {

  /*
  const QMimeData *mimeData = event->mimeData();
  if (mimeData->hasUrls()) {
    QUrl url = mimeData->urls()[0];
    QFileInfo info(url.path());
    if(info.isFile() && info.exists() && info.suffix() == "dae") {
      qDebug() << "Got it!\n";
    }
  } else {
    qDebug() << "Can't recognize the format\n";
  }
  */
  event->accept();
}

void GLWidget::dragEnterEvent(QDragEnterEvent *event) {
  event->accept();
}

void GLWidget::dragMoveEvent(QDragMoveEvent *event) {
  event->accept();
}

void GLWidget::makeSelectionActionActive() {
  qDebug() << "Selection!";
  current_tool = SELECTION;
}

void GLWidget::makeCircleActionActive() {
  current_tool = CIRCLE;
}

void GLWidget::makeRectActionActive() {
  current_tool = RECTANGLE;
}
