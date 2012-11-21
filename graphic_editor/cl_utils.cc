#include <QDebug>
#include <QApplication>

#include "cl_utils.h"
#include <stdio.h>

#define PROGRAM_FILE "/kernels/spline.cl"
#define KERNEL_FUNC "spline"

class GLWidget;

// Initialize OpenCL processing
void CLUtils::InitCL(GLWidget* glw) {

  cl_context_properties properties[7];
  std::string program_string;
  const char *program_chars;
  char *program_log;
  size_t program_size, log_size;
  int err;

  // Identify a platform
  err = clGetPlatformIDs(1, &glw->platform, NULL);
  if(err < 0) {
    std::cerr << "Couldn't identify a platform" << std::endl;
    exit(1);
  }

  // Access a device
  err = clGetDeviceIDs(glw->platform, CL_DEVICE_TYPE_CPU, 1, &glw->device, NULL);
  if(err == CL_DEVICE_NOT_FOUND) {
    err = clGetDeviceIDs(glw->platform, CL_DEVICE_TYPE_GPU, 1, &glw->device, NULL);
  }
  if(err < 0) {
    std::cerr << "Couldn't access any devices" << std::endl;
    exit(1);
  }

  // Create OpenCL context properties
  properties[0] = CL_GL_CONTEXT_KHR;
  properties[1] = (cl_context_properties)wglGetCurrentContext();
  properties[2] = CL_WGL_HDC_KHR;
  properties[3] = (cl_context_properties)wglGetCurrentDC();
  properties[4] = CL_CONTEXT_PLATFORM;
  properties[5] = (cl_context_properties)glw->platform;
  properties[6] = 0;

  // Create context
  glw->dev_context = clCreateContext(properties, 1, &glw->device, NULL, NULL, &err);
  if(err < 0) {
    std::cerr << "Couldn't create a context" << std::endl;
    exit(1);
  }

  // Create program
  QString str = QCoreApplication::applicationDirPath() + PROGRAM_FILE;
  program_string = glw->ReadFile(str.toStdString());
  program_chars = program_string.c_str();
  program_size = program_string.size();
  glw->program = clCreateProgramWithSource(glw->dev_context, 1, &program_chars, &program_size, &err);
  if(err < 0) {
    std::cerr << "Couldn't create the program" << std::endl;
    exit(1);
  }

  // Build motion program
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

  // Create kernels
  glw->kernel = clCreateKernel(glw->program, KERNEL_FUNC, &err);
  if(err < 0) {
    std::cerr << "Couldn't create the kernel: " << err << std::endl;
    exit(1);
  };

  // Add arguments to kernel
  err |= clSetKernelArg(glw->kernel, 5, sizeof(glw->order), &glw->order);
  if(err < 0) {
    printf("Couldn't set a kernel argument");
    exit(1);
  };

  // Create a command glw->queue
  glw->queue = clCreateCommandQueue(glw->dev_context, glw->device, 0, &err);
  if(err < 0) {
    std::cerr << "Couldn't create a command queue" << std::endl;
    exit(1);
  };
}

void CLUtils::ExecuteKernel(GLWidget* glw) {

  int err;

  // Add arguments to kernel
  err = clSetKernelArg(glw->kernel, 1, sizeof(cl_mem), &glw->knot_buffer);  
  err |= clSetKernelArg(glw->kernel, 2, sizeof(cl_mem), &glw->shared_buffers[1]);
  err |= clSetKernelArg(glw->kernel, 3, glw->num_knots * sizeof(float), NULL);
  err |= clSetKernelArg(glw->kernel, 4, sizeof(glw->num_spline_points), &glw->num_spline_points);
  err |= clSetKernelArg(glw->kernel, 6, sizeof(glw->num_knots), &glw->num_knots);
  if(err < 0) {
    printf("Couldn't set a kernel argument");
    exit(1);
  };

  // Complete OpenGL processing
  glFinish();

  // Execute the kernel
  err = clEnqueueAcquireGLObjects(glw->queue, 2, glw->shared_buffers, 0, NULL, NULL);
  if(err < 0) {
     perror("Couldn't acquire the GL objects");
     exit(1);
  }

  err = clEnqueueNDRangeKernel(glw->queue, glw->kernel, 1, NULL, (size_t*)&glw->num_spline_points,
    (size_t*)&glw->num_spline_points, 0, NULL, NULL);
  if(err < 0) {
     perror("Couldn't enqueue the kernel");
     exit(1);
  }

  clEnqueueReleaseGLObjects(glw->queue, 2, glw->shared_buffers, 0, NULL, NULL);
  clFinish(glw->queue);
}

void CLUtils::UpdateVertices(GLWidget* glw) {
  /*
  qDebug() << "update_vertices";

  int current_time, err;
  float delta_t;

  // Execute collision kernel
  err = clEnqueueNDRangeKernel(glw->queue, glw->kernel, 1, NULL,
                               &glw->obj_global_size,
                               &glw->obj_local_size, 0, NULL, NULL);
  if(err < 0) {
    std::cerr << "Couldn't enqueue the collision kernel" << std::endl;
    exit(1);
  }

  // Measure the elapsed time
  current_time = glw->timer->elapsed();
  delta_t = (current_time - glw->previous_time)/1000.0f;
  glw->previous_time = current_time;

  // Update kernel with time delta
  err = clSetKernelArg(glw->update_kernel, 2, sizeof(float), &delta_t);
  if(err < 0) {
    std::cerr << "Couldn't set a kernel argument" << std::endl;
    exit(1);
  };

  // Execute update kernel
  err = clEnqueueNDRangeKernel(glw->queue, glw->update_kernel, 1, NULL,
                               &glw->obj_global_size,
                               &glw->obj_local_size, 0, NULL, NULL);
  if(err < 0) {
    std::cerr << "Couldn't enqueue the update kernel" << std::endl;
    exit(1);
  }

  glFinish();

  err = clEnqueueAcquireGLObjects(glw->queue, 1, &glw->vbo_memobj, 0, NULL, NULL);
  if(err < 0) {
    std::cerr << "Couldn't acquire the GL objects" << std::endl;
    exit(1);
  }

  // Execute motion kernel
  err = clEnqueueNDRangeKernel(glw->queue, glw->motion_kernel, 1, NULL,
                               &glw->vertex_global_size,
                               &glw->vertex_local_size, 0, NULL, NULL);
  if(err < 0) {
    std::cerr << "Couldn't enqueue the motion kernel" << std::endl;
    exit(1);
  }

  clEnqueueReleaseGLObjects(glw->queue, 1, &glw->vbo_memobj, 0, NULL, NULL);
  clFinish(glw->queue);

  glw->updateGL();
  glw->update();
  */
}

