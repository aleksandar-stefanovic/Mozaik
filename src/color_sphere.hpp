#ifndef RG16_MOZAIK_COLOR_SPHERE_HPP
#define RG16_MOZAIK_COLOR_SPHERE_HPP

#include <GL/glew.h>
#include <glm/vec3.hpp>

/** Create a sphere approximation based on octahedron "inflation"
 *
 * The sphere is generated by diving octahedron sides into smaller
 * triangles, and then normalizing their distance from the center.
 *
 * @param center
 * Center of the resulting sphere.
 * @param diameter
 * Diameter of the resulting sphere.
 * @param precision_level
 * The degree of subdivision. The number of vertices rises exponentially.
 * @return
 * Pointer to the resulting array of sphere vertices, which should be
 * deallocated manually.
 */
GLfloat* create_sphere(glm::vec3 center, GLfloat diameter, unsigned precision_level);

#endif //RG16_MOZAIK_COLOR_SPHERE_HPP
