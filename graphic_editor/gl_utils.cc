#include "gl_utils.h"

class GLWidget;

void GLUtils::UpdateKnots(GLWidget* glw) {

  float increment;
  int err;

  glw->num_knots = glw->num_ctrl_points + 3;
  glw->knots.resize(glw->num_knots);
  glw->knots.clear();
  increment = 1.0f/(glw->num_ctrl_points - 2);

  // Set knot values
  glw->knots.push_back(0.0f);
  glw->knots.push_back(0.0f);
  glw->knots.push_back(0.0f);
  for(int i=1; i<glw->num_ctrl_points - 2; i++) {
    glw->knots.push_back(increment * i);
  }
  glw->knots.push_back(1.0f);
  glw->knots.push_back(1.0f);
  glw->knots.push_back(1.0f);

  glw->knot_buffer = clCreateBuffer(glw->dev_context, 
    CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
    glw->num_knots * sizeof(float), &(glw->knots[0]), &err);
}

void GLUtils::UpdateSpline(GLWidget* glw) {

  int err;

  // Create vertex objects to contain spline points
  glBindVertexArray(glw->vao[3]);
  glBindBuffer(GL_ARRAY_BUFFER, glw->vbo[3]);

  // Set data to NULL - data will be initialized by the OpenCL kernel
  glBufferData(GL_ARRAY_BUFFER, glw->num_spline_points * 4 * sizeof(GLfloat), 
    NULL, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(NULL);
  glBindBuffer(GL_ARRAY_BUFFER, NULL);

  // Create OpenCL buffer for the spline data
  glw->shared_buffers[1] = clCreateFromGLBuffer(glw->dev_context, CL_MEM_WRITE_ONLY, 
       glw->vbo[3], &err);
  if(err < 0) {
    perror("Couldn't create a buffer object from the VBO");
    exit(1);
  }
}

void GLUtils::UpdateControlPoints(GLWidget* glw) {

  int err;

  // VBO for control points
  glBindVertexArray(glw->vao[2]);
  glBindBuffer(GL_ARRAY_BUFFER, glw->vbo[2]);
  glBufferData(GL_ARRAY_BUFFER, glw->num_ctrl_points * 4 * sizeof(GLfloat),
    NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, glw->num_ctrl_points * 4 * sizeof(GLfloat),
    &(glw->ctrl_points[0]));
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(NULL);
  glBindBuffer(GL_ARRAY_BUFFER, NULL);

//  if(glw->shared_buffers[0] == NULL) {

    // Create memory object from the VBO
    glw->shared_buffers[0] = clCreateFromGLBuffer(glw->dev_context, CL_MEM_WRITE_ONLY, 
      glw->vbo[2], &err);
    if(err < 0) {
      perror("Couldn't create a buffer object from the VBO");
      exit(1);
    }

    // Set the buffer as a kernel argument
    err = clSetKernelArg(glw->kernel, 0, sizeof(cl_mem), &glw->shared_buffers[0]);
    if(err < 0) {
      printf("Couldn't set a kernel argument");
      exit(1);
    };
//  }
}

void GLUtils::DrawBase(GLWidget* glw) {

  const float max = 2000.0f;

  float base_coords[64] = {-max, -max, 0.0f, 1.0,
                             max, -max, 0.0f, 1.0,
                             max,  max, 0.0f, 1.0,
                            -max,  max, 0.0f, 1.0,
                            -max, 0.0f, 0.0f, 1.0,
                           0.0f, 0.0f, 0.0f, 1.0,
                           0.0f, 0.0f, 0.0f, 1.0,
                            max, 0.0f, 0.0f, 1.0,
                           0.0f, -max, 0.0f, 1.0,
                           0.0f, 0.0f, 0.0f, 1.0,
                           0.0f, 0.0f, 0.0f, 1.0,
                           0.0f,  max, 0.0f, 1.0,
                           0.0f, 0.0f, -max, 1.0,
                           0.0f, 0.0f, 0.0f, 1.0,
                           0.0f, 0.0f, 0.0f, 1.0,
                           0.0f, 0.0f,  max, 1.0};

  // VBO for axis points
  glBindVertexArray(glw->vao[0]);
  glBindBuffer(GL_ARRAY_BUFFER, glw->vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, 64 * sizeof(GLfloat), base_coords, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(NULL);
  glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

void GLUtils::DrawGrid(GLWidget* glw) {

  // Clear grid arrays
  glw->grid_coords.clear();

  // Set grid points
  for(float i=1.0f; i<glw->screen_dim.x; i+=1.0f) {
    for(float j=1.0f; j<glw->screen_dim.y; j+=1.0f) {
      glw->grid_coords.push_back(i);
      glw->grid_coords.push_back(j);
      glw->grid_coords.push_back(0.0f);
      glw->grid_coords.push_back(1.0f);

      glw->grid_coords.push_back(-i);
      glw->grid_coords.push_back(j);
      glw->grid_coords.push_back(0.0f);
      glw->grid_coords.push_back(1.0f);

      glw->grid_coords.push_back(i);
      glw->grid_coords.push_back(-j);
      glw->grid_coords.push_back(0.0f);
      glw->grid_coords.push_back(1.0f);

      glw->grid_coords.push_back(-i);
      glw->grid_coords.push_back(-j);
      glw->grid_coords.push_back(0.0f);
      glw->grid_coords.push_back(1.0f);
    }
  }

  glw->num_grid_points = glw->grid_coords.size()/4;

  // VBO for grid points
  glBindVertexArray(glw->vao[1]);
  glBindBuffer(GL_ARRAY_BUFFER, glw->vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, glw->num_grid_points * 4 * sizeof(GLfloat),
    NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, glw->num_grid_points * 4 * sizeof(GLfloat),
    &(glw->grid_coords[0]));
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(NULL);
  glBindBuffer(GL_ARRAY_BUFFER, NULL);

/*
  int loc;
  float *normal_addr;
  glm::vec3 *vertex_addr;
  unsigned short *index_addr;

  // Create a glw->vao for each geometry
  glGenVertexArrays(1, &glw->vao);

  // Create two glw->vbos for each geometry
  glGenBuffers(2, glw->vbos);

  // Create an glw->ibo for each geometry
  glGenBuffers(1, &glw->ibo);

  // Create arrays containing position and normal data
  glw->vertex_data = new glm::vec3[glw->geom_vec[0].map["POSITION"].size/12];
  glw->normal_data = new float[glw->geom_vec[0].map["NORMAL"].size/4];
  glw->index_data = new unsigned short[glw->geom_vec[0].index_count];

  // Initialize array addresses
  vertex_addr = glw->vertex_data;
  normal_addr = glw->normal_data;
  index_addr = glw->index_data;

  // Set vertex positions, normal vectors, and glw->indices
  memcpy(vertex_addr, glw->geom_vec[0].map["POSITION"].data, glw->geom_vec[0].map["POSITION"].size);
  memcpy(normal_addr, glw->geom_vec[0].map["NORMAL"].data, glw->geom_vec[0].map["NORMAL"].size);
  memcpy(index_addr, glw->geom_vec[0].indices, glw->geom_vec[0].index_count * sizeof(unsigned short));

  // Update the glw->count and glw->indices values
  glw->count = glw->geom_vec[0].index_count;
  glw->indices = (GLvoid*)(glw->geom_vec[0].indices);

  // Update addresses
  vertex_addr += glw->geom_vec[0].map["POSITION"].size/12;
  normal_addr += glw->geom_vec[0].map["NORMAL"].size/4;
  index_addr += glw->geom_vec[0].index_count;

  // Update vertices
  for(unsigned j=0; j<glw->num_vertices; j++) {
    glw->vertex_data[j] *= glw->sphere_vec.radius/0.5f;
    glw->vertex_data[j] += glw->sphere_vec.center;
  }

  // Configure glw->vbos to hold positions and normals for each geometry
  glBindVertexArray(glw->vao);

  // Set vertex coordinate data
  glBindBuffer(GL_ARRAY_BUFFER, glw->vbos[0]);
  glBufferData(GL_ARRAY_BUFFER, glw->geom_vec[0].map["POSITION"].size, glw->vertex_data, GL_DYNAMIC_DRAW);
  loc = glGetAttribLocation(program, "in_coords");
  glVertexAttribPointer(loc, glw->geom_vec[0].map["POSITION"].stride,
                        glw->geom_vec[0].map["POSITION"].type, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // Set normal vector data
  glBindBuffer(GL_ARRAY_BUFFER, glw->vbos[1]);
  glBufferData(GL_ARRAY_BUFFER, glw->geom_vec[0].map["NORMAL"].size, glw->normal_data, GL_DYNAMIC_DRAW);
  loc = glGetAttribLocation(program, "in_normals");
  glVertexAttribPointer(loc, glw->geom_vec[0].map["NORMAL"].stride,
                        glw->geom_vec[0].map["NORMAL"].type, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  // Set index data
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glw->ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, glw->geom_vec[0].index_count*sizeof(unsigned short),
               glw->index_data, GL_DYNAMIC_DRAW);
*/
}

// Initialize uniform data
void GLUtils::InitUniforms(GLWidget *glw, GLuint program) {

  // Determine the locations of the color and modelview-projection matrices
  glw->color_location = glGetUniformLocation(program, "color");
  glw->mvp_location = glGetUniformLocation(program, "mvp");

  // Specify the modelview matrix
  glw->modelview_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

  // Specify the rotation matrices
  glw->rot_x = glm::mat4(1.0f);
  glw->rot_y = glm::mat4(1.0f);
}

// Compile the shader
void CompileShader(GLint shader) {

  GLint success;
  GLsizei log_size;
  char *log;

  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
    log = new char[log_size+1];
    log[log_size] = '\0';
    glGetShaderInfoLog(shader, log_size+1, NULL, log);
    std::cout << log;
    delete(log);
    exit(1);
  }
}

GLuint GLUtils::InitShaders(GLWidget* glw) {

  GLuint vs, fs, prog;
  std::string vs_source, fs_source;
  const char *vs_chars, *fs_chars;
  GLint vs_length, fs_length;

  QString vert_string = QCoreApplication::applicationDirPath() + "/shaders/basic_interop.vert";
  QString frag_string = QCoreApplication::applicationDirPath() + "/shaders/basic_interop.frag";

  // Create shader descriptors
  vs = glCreateShader(GL_VERTEX_SHADER);
  fs = glCreateShader(GL_FRAGMENT_SHADER);

  // Read shader text from files
  vs_source = glw->ReadFile(vert_string.toStdString());
  fs_source = glw->ReadFile(frag_string.toStdString());

  // Set shader source code
  vs_chars = vs_source.c_str();
  fs_chars = fs_source.c_str();
  vs_length = (GLint)vs_source.length();
  fs_length = (GLint)fs_source.length();
  glShaderSource(vs, 1, &vs_chars, &vs_length);
  glShaderSource(fs, 1, &fs_chars, &fs_length);

  // Compile shaders and create program
  CompileShader(vs);
  CompileShader(fs);
  prog = glCreateProgram();

  // Bind attributes
  glBindAttribLocation(prog, 0, "in_coords");

  // Attach shaders
  glAttachShader(prog, vs);
  glAttachShader(prog, fs);

  glLinkProgram(prog);
  glUseProgram(prog);

  return prog;
}

// Initialize OpenGL
void GLUtils::InitGL(GLWidget* glw) {

  GLuint program;

  // Configure culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0.0f, 1.0f);

  // Initialize GLEW
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    qDebug() << "Can't initialize glew.\n";
  }

  program = InitShaders(glw);
  InitUniforms(glw, program);

  // Create vertex array objects
  glGenVertexArrays(glw->num_vaos, glw->vao);

  // Create vertex buffer objects
  glGenBuffers(glw->num_vbos, glw->vbo);

  // Draw x,y,z axes as RGB
  DrawBase(glw);
}
