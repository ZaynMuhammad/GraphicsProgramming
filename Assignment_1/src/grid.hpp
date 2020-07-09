#ifndef GRID_H_
#define GRID_H_

#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

const int GRID_DIMENSIONS = 100;
const int VERTEX_QTY = GRID_DIMENSIONS * 4;
glm::vec3 gr[VERTEX_QTY];
//glm::vec3 grid_vertices[VERTEX_QTY];

std::vector<glm::vec3> grid_vertices = {};
#endif // GRID_H_
