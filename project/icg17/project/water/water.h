#pragma once

#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>
#include "../param.h"

class Water {
  private:
    GLuint vertex_array_id_;               // vertex array object
    GLuint vertex_buffer_object_position_; // memory buffer for positions
    GLuint vertex_buffer_object_index_;    // memory buffer for indices
    GLuint program_id_;                    // GLSL shader program ID
    GLuint water_texture_id_;              // normal texture ID
    GLuint water_reflection_id_;           // reflection texture ID
    GLuint num_indices_;                   // number of vertices to render
    GLuint MVP_id_;                        // model, view, proj matrix ID
    GLuint MV_id_;
    GLuint time_id_;

  public:
    void Init(GLuint water_reflection_id) {
        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("water_vshader.glsl", "water_fshader.glsl");
        if (!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates and indices
        {
            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;

            float min_pos = -1.0; //from -1 to 1
            float pos_range = 2.0;

            int xLength = WATER_DIM;
            int yLength = WATER_DIM;

            int offset = 0;

            // First, build the data for the vertex buffer
            for (int y = 0; y < yLength; y++) {
                for (int x = 0; x < xLength; x++) {
                    float xRatio = x / (float) (xLength - 1);

                    // Build our grid from the top down, so that our triangles are
                    // counter-clockwise.
                    float yRatio = 1.0f - (y / (float) (yLength - 1));

                    vertices.push_back(min_pos + (xRatio * pos_range));
                    vertices.push_back(min_pos + (yRatio * pos_range));
                }
            }

            // Now build the index data
            int numStripsRequired = yLength - 1;
            int numDegensRequired = 2 * (numStripsRequired - 1);

            offset = 0;

            for (int y = 0; y < yLength - 1; y++) {
                if (y > 0) {
                    // Degenerate begin: repeat first vertex
                    indices.push_back(y * yLength);
                }

                for (int x = 0; x < xLength; x++) {
                    // One part of the strip
                    indices.push_back((y * yLength) + x);
                    indices.push_back(((y + 1) * yLength) + x);
                }

                if (y < yLength - 2) {
                    // Degenerate end: repeat last vertex
                    indices.push_back(((y + 1) * yLength) + (xLength - 1));
                }
            }

            num_indices_ = indices.size();

            // position buffer
            glGenBuffers(1, &vertex_buffer_object_position_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                         &vertices[0], GL_STATIC_DRAW);

            // vertex indices
            glGenBuffers(1, &vertex_buffer_object_index_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                         &indices[0], GL_STATIC_DRAW);

            // position shader attribute
            GLuint loc_position = glGetAttribLocation(program_id_, "position");
            glEnableVertexAttribArray(loc_position);
            glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        // textures
        {
            initTexture("water_texture.tga", &water_texture_id_, "water", GL_TEXTURE0);
        }

        // load/Assign texture
        water_reflection_id_ = water_reflection_id;
        glUniform1i(glGetUniformLocation(program_id_, "water_reflect"), 1 /*GL_TEXTURE1*/);

        // other uniforms
        MVP_id_ = glGetUniformLocation(program_id_, "MVP");
        MV_id_ = glGetUniformLocation(program_id_, "MV");
        glUniform1f(glGetUniformLocation(program_id_, "WATER_HEIGHT"), WATER_HEIGHT);
        glUniform1f(glGetUniformLocation(program_id_, "SPEED"), SPEED);
        glUniform3fv(glGetUniformLocation(program_id_, "LIGHT_POS"), 3,  value_ptr(LIGHT_POS));
        time_id_ = glGetUniformLocation(program_id_, "time");

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_position_);
        glDeleteBuffers(1, &vertex_buffer_object_index_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteProgram(program_id_);
        glDeleteTextures(1, &water_texture_id_);
        glDeleteTextures(1, &water_reflection_id_);
    }

    void Draw(float time, const glm::mat4 &model = IDENTITY_MATRIX, const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX, glm::vec3 cam_pos = vec3(0, 0, 0)) {

        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, water_texture_id_);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, water_reflection_id_);

        // setup MVP
        glm::mat4 MVP = projection * view * model;
        glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

        glm::mat4 MV = view * model;
        glUniformMatrix4fv(MV_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MV));

        // pass the current time stamp to the shader.
        glUniform1f(time_id_, time);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

        glDisable(GL_BLEND);

        glBindVertexArray(0);
        glUseProgram(0);
    }

  private:
    void initTexture(string filename, GLuint *texture_id, string texture_name, int val) {

        int width;
        int height;
        int nb_component;
        // set stb_image to have the same coordinates as OpenGL
        unsigned char *image = stbi_load(filename.c_str(), &width, &height, &nb_component, 0);

        if (image == nullptr) {
            throw(string("Failed to load texture"));
        }

        glGenTextures(1, texture_id);
        glBindTexture(GL_TEXTURE_2D, *texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        if (nb_component == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        } else if (nb_component == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }

        GLuint tex_id = glGetUniformLocation(program_id_, texture_name.c_str());
        glUseProgram(program_id_);
        glUniform1i(tex_id, val - GL_TEXTURE0); //why - GL_TEXTURE0?

        // cleanup
        glBindTexture(GL_TEXTURE_2D, val); //why not bind 0??
        stbi_image_free(image);
    }
};
