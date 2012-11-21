#include <QDebug>

#include "cl_utils.h"
#include <stdio.h>

#define PROGRAM_FILE "kernels/test.cl"
#define KERNEL_FUNC "test"

const int spline_size = 40;
const int control_size = 9;
const int order = 3;
const int knot_size = order + control_size;

float spline_points[spline_size * 4];

cl_mem control_buffer, knot_buffer, spline_buffer;

void CLUtils::init_cl(GLWidget* glw) {

  std::string program_string;
  const char *program_chars;
  char *program_log;
  size_t program_size, log_size;
  int err;

  // The control points for this curve
  float control_points[control_size][4] = {
    { 0.5,  0.0,  0.0,  1.0},
    { 0.5,  0.5,  0.0,  0.70711},
    { 0.0,  0.5,  0.0,  1.0},
    {-0.5,  1.0,  0.0,  0.70711},
    {-0.5,  0.0,  0.0,  1.0},
    {-0.5, -0.5,  0.0,  0.70711},
    { 0.0, -0.5,  0.0,  1.0},
    { 0.5, -0.5,  0.0,  0.70711},
    { 0.5,  0.0,  0.0,  1.0}};

  // The knot vector
  float knots[knot_size] = {0, 0, 0, 0.25, 0.25, 0.5, 0.5, 0.75, 0.75, 1.0, 1.0, 1.0};

  // Identify a platform
  err = clGetPlatformIDs(1, &(glw->platform), NULL);
  if(err < 0) {
    exit(1);
  }

  // Access a device
  err = clGetDeviceIDs(glw->platform, CL_DEVICE_TYPE_GPU, 1, &(glw->device), NULL);
  if(err == CL_DEVICE_NOT_FOUND) {
     err = clGetDeviceIDs(glw->platform, CL_DEVICE_TYPE_CPU, 1, &(glw->device), NULL);
  }
  if(err < 0) {
      std::cerr << "Couldn't access any devices" << std::endl;
      exit(1);
  }

  // Create OpenCL context properties
  cl_context_properties properties[] = {
    CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
    CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
    CL_CONTEXT_PLATFORM, (cl_context_properties)glw->platform, 0};

  // Create context
  glw->context = clCreateContext(properties, 1, &(glw->device), NULL, NULL, &err);
  if(err < 0) {
    std::cerr << "Couldn't create a context" << std::endl;
    exit(1);
  }

  // Create program
  program_string = glw->read_file(PROGRAM_FILE);
  program_chars = program_string.c_str();
  program_size = program_string.size();
  glw->program = clCreateProgramWithSource(glw->context, 1, &program_chars, &program_size, &err);
  if(err < 0) {
    std::cerr << "Couldn't create the program" << std::endl;
    exit(1);
  }

  // Build program
  err = clBuildProgram(glw->program, 0, NULL, NULL, NULL, NULL);
  if(err < 0) {

    // Find size of log and print to std output
    clGetProgramBuildInfo(glw->program, glw->device, CL_PROGRAM_BUILD_LOG,
                          0, NULL, &log_size);
    program_log = new char(log_size + 1);
    program_log[log_size] = '\0';
    clGetProgramBuildInfo(glw->program, glw->device, CL_PROGRAM_BUILD_LOG,
                          log_size + 1, (void*)program_log, NULL);
    std::cout << program_log << std::endl;
    delete(program_log);
    exit(1);
  }

  // Create kernel
  glw->kernel = clCreateKernel(glw->program, KERNEL_FUNC, &err);
  if(err < 0) {
    std::cerr << "Couldn't create the motion kernel: " << err << std::endl;
    exit(1);
  };

  // Create buffers
  control_buffer = clCreateBuffer(glw->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        control_size * 4 * sizeof(float), control_points, &err);
  if(err < 0) {
     perror("Couldn't create a buffer");
     exit(1);
  };
  knot_buffer = clCreateBuffer(glw->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        knot_size * sizeof(float), knots, &err);

  // Create buffer for VBO
  /*
  spline_buffer = clCreateFromGLBuffer(glw->context, CL_MEM_WRITE_ONLY, glw->vbo, &err);
  if(err < 0) {
     perror("Couldn't create a buffer object from the VBO");
     exit(1);
  }
*/

  /* Create kernel argument */
  err = clSetKernelArg(glw->kernel, 0, sizeof(cl_mem), &control_buffer);
  err |= clSetKernelArg(glw->kernel, 1, sizeof(cl_mem), &knot_buffer);
  err |= clSetKernelArg(glw->kernel, 3, knot_size * sizeof(float), NULL);
  err |= clSetKernelArg(glw->kernel, 4, sizeof(spline_size), &spline_size);
  err |= clSetKernelArg(glw->kernel, 5, sizeof(order), &order);
  err |= clSetKernelArg(glw->kernel, 6, sizeof(knot_size), &knot_size);
  if(err < 0) {
     perror("Couldn't set a kernel argument");
     exit(1);
  };

  // Create a command queue
  glw->queue = clCreateCommandQueue(glw->context, glw->device, 0, &err);
  if(err < 0) {
    std::cerr << "Couldn't create a command queue" << std::endl;
    exit(1);
  };

  /* Read and print the result
  err = clEnqueueReadBuffer(glw->queue, spline_buffer, CL_TRUE, 0,
     spline_size * 4 * sizeof(float), spline_points, 0, NULL, NULL);
  if(err < 0) {
     perror("Couldn't read the buffer");
     exit(1);
  }
*/

  /* Display the results
  for(int i=0; i<spline_size*4; i+=4) {
     printf("%d: %6.3f, %6.3f, %6.3f\n",
       (i/4), spline_points[i], spline_points[i+1], spline_points[i+2], spline_points[i+3]);
  }
  */
}

void CLUtils::compute_vertices(GLWidget *glw) {

  int err;

  glFinish();

  err = clEnqueueAcquireGLObjects(glw->queue, 1, &spline_buffer, 0, NULL, NULL);
  if(err < 0) {
    std::cerr << "Couldn't acquire the GL objects" << std::endl;
    exit(1);
  }

  err = clSetKernelArg(glw->kernel, 2, sizeof(cl_mem), &spline_buffer);

  /* Enqueue kernel */
  size_t global_size = 256;
  size_t local_size = 256;
  err = clEnqueueNDRangeKernel(glw->queue, glw->kernel, 1, NULL, &global_size,
                               &local_size, 0, NULL, NULL);
  if(err < 0) {
     perror("Couldn't enqueue the kernel");
     exit(1);
  }

  /* Read and print the result
  err = clEnqueueReadBuffer(glw->queue, spline_buffer, CL_TRUE, 0,
     spline_size * 4 * sizeof(float), spline_points, 0, NULL, NULL);
  if(err < 0) {
     perror("Couldn't read the buffer");
     exit(1);
  } */

  /* Display the results
  for(int i=0; i<40*4; i+=4) {
     printf("%d: %6.3f, %6.3f, %6.3f, %6.3f\n",
       (i/4), spline_points[i], spline_points[i+1], spline_points[i+2], spline_points[i+3]);
  } */

  clEnqueueReleaseGLObjects(glw->queue, 1, &spline_buffer, 0, NULL, NULL);
  clFinish(glw->queue);
}
