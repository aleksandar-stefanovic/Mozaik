#include <glm/ext.hpp>
#include "shape_manager.hpp"
#include "../mozaik_globals.hpp"

void ShapeManager::subscribe_shape(Shape *shape) {
    shape_list.push_back(shape);
    vertex_buffer_size += shape->vertex_data_size;
    element_buffer_size += shape->element_data_size;
}

void ShapeManager::populate_buffer() {
    // Static draw because data is written once and used many times.
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, nullptr, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, element_buffer_size, nullptr, GL_STATIC_DRAW);

    GLsizei current_vertex_offset = 0;
    GLsizei current_element_offset = 0;
    for (auto shape : shape_list) {
        glBufferSubData(GL_ARRAY_BUFFER, current_vertex_offset, shape->vertex_data_size, shape->vertex_data);
        current_vertex_offset += shape->vertex_data_size;

        // If the shape uses element buffer
        if (shape->element_data_size != -1) {
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, current_element_offset, shape->element_data_size,
                            shape->element_data);
            current_element_offset += shape->element_data_size;
        }
    }
}

void ShapeManager::render() {
    GLint vertex_buffer_offset = 0;
    GLint element_buffer_offset = 0;
    for (auto shape : shape_list) {
        glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(shape->model_matrix));

        // If the shape uses the element buffer
        if (shape->element_data_size != -1) {
            glDrawElementsBaseVertex(
                    shape->draw_mode,
                    shape->element_data_size / sizeof(shape->element_data[0]),
                    GL_UNSIGNED_INT,
                    reinterpret_cast<const void *>(element_buffer_offset),
                    vertex_buffer_offset
            );
            element_buffer_offset += element_buffer_size;
            vertex_buffer_offset += vertex_buffer_size;
        } else {
            glDrawArrays(
                    shape->draw_mode,
                    vertex_buffer_offset,
                    shape->vertex_data_size / (ATTR_COUNT * sizeof(shape->vertex_data[0]))
            );
            vertex_buffer_offset += shape->vertex_data_size;
        }
    }
}