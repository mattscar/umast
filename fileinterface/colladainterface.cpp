#define _CRT_SECURE_NO_WARNINGS

#include "colladainterface.h"
#include <QDebug>
#include <QDir>
#include <QApplication>

char array_types[7][15] = {"float_array", "int_array", "bool_array", "Name_array", 
                           "IDREF_array", "SIDREF_array", "token_array"};

char primitive_types[7][15] = {"lines", "linestrips", "polygons", "polylist", 
                               "triangles", "trifans", "tristrips"};

void ColladaInterface::readGeometries(std::vector<ColGeom>* v, const char* file_name) {

  pugi::xml_document doc;
  pugi::xml_node geometry, mesh, vertices, input, source, primitive;
  std::string source_name;
  int prim_count, num_indices;

  // Create document, load COLLADA file, and access <geometry> element
  pugi::xml_parse_result result = doc.load_file(file_name);
  qDebug() << "File name:" << file_name;
  geometry = doc.child("COLLADA").child("library_geometries").child("geometry");

  // Iterate through geometry elements 
  while(geometry != NULL) {

    // Create new geometry
    ColGeom data;

    // Set the geometry name
    data.name = geometry.attribute("id").value();

    // Iterate through mesh elements 
    mesh = geometry.child("mesh");
    while(mesh != NULL) {         
      vertices = mesh.child("vertices");
      input = vertices.child("input");
      
      // Iterate through input elements 
      while(input != NULL) {
        source_name = std::string(input.attribute("source").value());
        source_name = source_name.erase(0, 1);
        source = mesh.child("source");

        // Iterate through source elements 
        while(source != NULL) {
          if(std::string(source.attribute("id").value()) == source_name) {
            data.map[std::string(input.attribute("semantic").value())] = readSource(source);
          }

          source = source.next_sibling("source");
        }

        input = input.next_sibling("input");
      }

      // Determine primitive type
      for(int i=0; i<7; i++) {
        primitive = mesh.child(primitive_types[i]);
        if(primitive != NULL) {
          
          // Determine number of primitives
          prim_count = primitive.attribute("count").as_int();

          // Determine primitive type and set count
          switch(i) {
            case 0:
              data.primitive = GL_LINES; 
              num_indices = prim_count * 2; 
            break;
            case 1: 
              data.primitive = GL_LINE_STRIP; 
              num_indices = prim_count + 1;
            break;
            case 4: 
              data.primitive = GL_TRIANGLES; 
              num_indices = prim_count * 3; 
            break;
            case 5: 
              data.primitive = GL_TRIANGLE_FAN; 
              num_indices = prim_count + 2; 
            break;
            case 6: 
              data.primitive = GL_TRIANGLE_STRIP; 
              num_indices = prim_count + 2; 
            break;
            default: std::cout << "Primitive " << primitive_types[i] << 
                     " not supported" << std::endl;
          }
          data.index_count = num_indices;

          // Allocate memory for indices
          data.indices = (unsigned short*)malloc(num_indices * sizeof(unsigned short));

          // Read the index values
          char* text = (char*)(primitive.child("p").child_value());
          data.indices[0] = (unsigned short)atoi(strtok(text, " "));
          for(int index=1; index<num_indices; index++) {
            data.indices[index] = (unsigned short)atoi(strtok(NULL, " "));   
          }
        }
      }
      mesh = mesh.next_sibling("mesh");
    }

    v->push_back(data);    

    geometry = geometry.next_sibling("geometry");
  }
}

void ColladaInterface::freeGeometries(std::vector<ColGeom>* v) {
  
  std::vector<ColGeom>::iterator geom_it;
  SourceMap::iterator map_it;

  for(geom_it = v->begin(); geom_it < v->end(); geom_it++) {

    // Deallocate index data
    free(geom_it->indices);

    // Deallocate array data in each map value
    for(map_it = geom_it->map.begin(); map_it != geom_it->map.end(); map_it++) {
      free((*map_it).second.data);
    }
  }

  // Clear the vector
  v->clear();
}

SourceData readSource(pugi::xml_node source) {

  SourceData source_data;
  pugi::xml_node array;
  char* text;
  unsigned int num_vals, stride;
  int check;

  for(int i=0; i<7; i++) {
    array = source.child(array_types[i]);
    if(array != NULL) {

      // Find number of values
      num_vals = array.attribute("count").as_uint();
      source_data.size = num_vals;

      // Find stride
      check = source.child("technique_common").child("accessor").attribute("stride").as_uint();
      if(check != 0)
        source_data.stride = stride;
      else
        source_data.stride = 1;

      // Read array values
      text = (char*)(array.child_value());

      // Initialize mesh data according to data type
      switch(i) {

        // Array of floats
        case 0:
          source_data.type = GL_FLOAT;
          source_data.size *= sizeof(float);
          source_data.data = malloc(num_vals * sizeof(float));

          // Read the float values
          ((float*)source_data.data)[0] = atof(strtok(text, " "));  
          for(unsigned int index=1; index<num_vals; index++) {
            ((float*)source_data.data)[index] = atof(strtok(NULL, " "));   
          }
        break;

        // Array of integers
        case 1:
          source_data.type = GL_INT;
          source_data.size *= sizeof(int);
          source_data.data = malloc(num_vals * sizeof(int));

          // Read the int values
          ((int*)source_data.data)[0] = atof(strtok(text, " "));  
          for(unsigned int index=1; index<num_vals; index++) {
            ((int*)source_data.data)[index] = atof(strtok(NULL, " "));   
          }
        break;

          // Other
        default:
          std::cout << "Collada Reader doesn't support mesh data in this format" << std::endl;
        break;
      }
    }
  }
  return source_data;
}
