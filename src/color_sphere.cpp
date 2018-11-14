#include "color_sphere.hpp"
#include <math.h>
#include <cstdio>
#include <random>
#include <functional>
#include <glm/detail/type_mat.hpp>
#include <glm/gtc/matrix_transform.hpp>

// TODO: define ATTR_COUNT in a single place.
#define ATTR_COUNT 6

static void normalize(glm::vec3 &vector, GLfloat length);

static glm::vec3 multiply(glm::vec3 vector, GLfloat factor);

/* Hint how many vertices will one octahedron triangle use. */
static unsigned triangle_vertex_array_size_hint(unsigned lod);

static void put_into_vertex_array(GLfloat vertex_array[], glm::vec3 vertex);

static void put_into_index_array(GLuint index_array[], GLuint first, GLuint second, GLuint third);

void create_sphere(GLfloat vertex_array[], GLuint index_array[], GLfloat radius, unsigned lod) {

    // The point from which all other points will be translated
    glm::vec3 origin = glm::vec3(1.0f, 0.0f, 0.0f);

    // Base vectors which will be multiplied and added to the origin vector
    glm::vec3 base1 = glm::vec3(-1.0f, 1.0f, 0.0f);
    glm::vec3 base2 = glm::vec3(-1.0f, 0.0f, 1.0f);

    // Normalize to match length of a single distance between vertices
    normalize(base1, static_cast<GLfloat>(powf(2.0f, 1.0f / 2) / (lod - 1)));
    normalize(base2, static_cast<GLfloat>(powf(2.0f, 1.0f / 2) / (lod - 1)));

    glm::mat4 rotation_matrix = glm::mat4(1.0f);
    glm::vec3 z_axis = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 x_axis = glm::vec3(1.0f, 0.0f, 0.0f);

    unsigned triangle_size = triangle_vertex_array_size_hint(lod);

    for (int hemisphere = 0; hemisphere < 2; hemisphere++) {
        for (int side = 0; side < 4; side++) {
            // Vertex buffer entry
            for (int j = 0; j < lod; j++) {
                for (int i = 0; i < lod - j; ++i) {
                    glm::vec3 vector = origin + multiply(base1, i) + multiply(base2, j);
                    normalize(vector, radius);
                    glm::vec4 rot_vector = rotation_matrix * glm::vec4(vector, 1.0);
                    put_into_vertex_array(vertex_array, glm::vec3(rot_vector.x, rot_vector.y, rot_vector.z));
                }
            }
            rotation_matrix = glm::rotate(rotation_matrix, M_PI_2f32, z_axis);

            // Index buffer entry
            unsigned face_offset = (hemisphere * 4 + side) * triangle_size;
            unsigned offset = 0;
            for (int j = 0; j < lod; j++) {
                for (unsigned i = 0; i < lod - j - 1; ++i) {
                    // Upward-facing triangles
                    put_into_index_array(index_array,
                                         face_offset + offset + i,
                                         face_offset + offset + i + 1,
                                         face_offset + offset + lod - j +i);
                    if (i < lod - j - 2) {
                        // Downward-facing triangles
                        put_into_index_array(index_array,
                                             face_offset + offset + lod - j + i,
                                             face_offset + offset + lod - j + i + 1,
                                             face_offset + offset + i + 1);
                    }
                }
                offset += (lod - j);
            }
        }
        rotation_matrix = glm::rotate(rotation_matrix, M_PIf32, x_axis);
    }
}

void normalize(glm::vec3 &vector, GLfloat length) {
    GLfloat orig_length = sqrtf(
            powf(vector.x, 2) +
            powf(vector.y, 2) +
            powf(vector.z, 2)
    );
    vector.operator*=(length / orig_length);
}

glm::vec3 multiply(glm::vec3 vector, GLfloat factor) {
    vector.x *= factor;
    vector.y *= factor;
    vector.z *= factor;
    return vector;
}

void put_into_vertex_array(GLfloat vertex_array[], glm::vec3 vertex) {
    static unsigned offset;
    static std::default_random_engine generator;
    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    static auto rand = std::bind(distribution, generator);

    vertex_array[offset] = vertex.x;
    vertex_array[offset + 1] = vertex.y;
    vertex_array[offset + 2] = vertex.z;
    // Colors of the vertices are calculated with the vertex shader (not taking into account color
    // values of the vertex). Therefore, there's no point in setting the vertex color at this point.

    offset += ATTR_COUNT;
}

void put_into_index_array(GLuint index_array[], GLuint first, GLuint second, GLuint third) {
    static unsigned offset;

    index_array[offset] = first;
    index_array[offset + 1] = second;
    index_array[offset + 2] = third;

    offset += 3;
}

unsigned triangle_vertex_array_size_hint(unsigned lod) {
    // Sum of the first lod numbers
    // (see https://github.com/MATF-RG18/RG16-mozaik/wiki/Miscellaneous-code-and-geometry-explanations)
    return (lod * (lod + 1) / 2);
}

unsigned sphere_vertex_count_hint(unsigned lod) {
    // Number of vertices per one octahedron side * 8 sides of an octahedron
    return triangle_vertex_array_size_hint(lod) * 8;
}

unsigned sphere_index_count_hint(unsigned lod) {
    // TODO: visual explanation
    return static_cast<unsigned int>(pow(lod - 1, 2) * 3 * 8);
}
