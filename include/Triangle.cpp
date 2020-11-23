#include "Triangle.hpp"

Triangle::Triangle() {}

glm::vec3 Triangle::a() const { return vtx[0]; }
glm::vec3 Triangle::b() const { return vtx[1]; }
glm::vec3 Triangle::c() const { return vtx[2]; }

glm::vec3 Triangle::na() const { return nor[0]; }
glm::vec3 Triangle::nb() const { return nor[1]; }
glm::vec3 Triangle::nc() const { return nor[2]; }

// Author: Blurgy <gy@blurgy.xyz>
// Date:   Nov 23 2020, 13:29 [CST]