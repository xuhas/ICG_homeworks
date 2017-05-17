#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>
#include "../param.h"

class Grid {

private:
    GLuint vertex_array_id_;                // vertex array object
    GLuint vertex_buffer_object_position_;  // memory buffer for positions
    GLuint vertex_buffer_object_index_;     // memory buffer for indices
    GLuint vertex_buffer_object_;           // memory buffer for texture coordinates
    GLuint program_id_;                     // GLSL shader program ID
    GLuint texture_id_;                     // texture ID
    GLuint texture_sand_id_;                // texture sand ID
    GLuint texture_grass_id_;                // texture grass ID
    GLuint texture_rock_id_;                // texture rock ID
    GLuint texture_snow_id_;                // texture snow ID
    GLuint num_indices_;                    // number of vertices to render
    GLuint MVP_id_;                         // model, view, proj matrix ID

public:

    void load_tex(GLuint &textureID, const char* filename){
        int width;
        int height;
        int nb_component;

        // set stb_image to have the same coordinates as OpenGL
        stbi_set_flip_vertically_on_load(1);
        unsigned char * image = stbi_load(filename, &width,
                                         &height, &nb_component, 0);

        if(image == nullptr) {
            printf("%s\n",stbi_failure_reason());
            throw("Failed to load texture");
        }

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        if(nb_component == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, image);
        } else if(nb_component == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, image);
        }

        // cleanup
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(image);
    }


    void Init(GLuint texture) {
        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("grid_vshader.glsl",
                                              "grid_fshader.glsl");
        if(!program_id_) {
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

            int xLength = GRID_DIM;
            int yLength = GRID_DIM;

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

        // texture coordinates
        {
            const GLfloat vertex_texture_coordinates[] = { /*V1*/ 0.0f, 0.0f,
                                                           /*V2*/ 1.0f, 0.0f,
                                                           /*V3*/ 0.0f, 1.0f,
                                                           /*V4*/ 1.0f, 1.0f};

            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates),
                         vertex_texture_coordinates, GL_STATIC_DRAW);

            // attribute
            GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_,
                                                                 "vtexcoord");
            glEnableVertexAttribArray(vertex_texture_coord_id);
            glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                                  DONT_NORMALIZE, ZERO_STRIDE,
                                  ZERO_BUFFER_OFFSET);
        }

        // load/Assign texture
        this->texture_id_ = texture;
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        GLuint tex_id = glGetUniformLocation(program_id_, "tex");
        glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
        glBindTexture(GL_TEXTURE_2D, 0);

        {
            //SAND
            texture_sand_id_ = glGetUniformLocation(program_id_, "tex_sand");
            glUniform1i(texture_sand_id_, 1 /*GL_TEXTURE1*/);
            load_tex(texture_sand_id_,"sand.tga");

            //GRASS
            texture_grass_id_ = glGetUniformLocation(program_id_, "tex_grass");
            glUniform1i(texture_grass_id_, 2 /*GL_TEXTURE2*/);
            load_tex(texture_grass_id_,"grass.tga");

            //ROCK
            texture_rock_id_ = glGetUniformLocation(program_id_, "tex_rock");
            glUniform1i(texture_rock_id_, 3 /*GL_TEXTURE3*/);
            load_tex(texture_rock_id_,"rock.tga");

            //SNOW
            texture_snow_id_ = glGetUniformLocation(program_id_, "tex_snow");
            glUniform1i(texture_snow_id_, 4 /*GL_TEXTURE4*/);
            load_tex(texture_snow_id_,"snow.tga");
        }

        // other uniforms
        MVP_id_ = glGetUniformLocation(program_id_, "MVP");
        glUniform1f(glGetUniformLocation(program_id_, "SPEED"), SPEED);
        glUniform3fv(glGetUniformLocation(program_id_, "LIGHT_POS"), 3,  value_ptr(LIGHT_POS));
        glUniform1f(glGetUniformLocation(program_id_, "WATER_HEIGHT"), WATER_HEIGHT);
        glUniform1f(glGetUniformLocation(program_id_, "BEACH_HEIGHT"), BEACH_HEIGHT);
        glUniform1f(glGetUniformLocation(program_id_, "ROCK_HEIGHT"), ROCK_HEIGHT);
        glUniform1f(glGetUniformLocation(program_id_, "SNOW_HEIGHT"), SNOW_HEIGHT);
        glUniform1i(glGetUniformLocation(program_id_, "USE_COLOURS"), USE_COLOURS ? 1 : 0);        

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_position_);
        glDeleteBuffers(1, &vertex_buffer_object_index_);
        glDeleteBuffers(1, &vertex_buffer_object_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteProgram(program_id_);
        glDeleteTextures(1, &texture_id_);
    }

    void Draw(float time, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX, bool draw_sand = true) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        glUniform1i(glGetUniformLocation(program_id_, "draw_sand"), draw_sand ? 1 : 0);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_sand_id_);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture_grass_id_);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture_rock_id_);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture_snow_id_);

        // setup matrix stack
        GLint model_id = glGetUniformLocation(program_id_,
                                              "model");
        glUniformMatrix4fv(model_id, ONE, DONT_TRANSPOSE, glm::value_ptr(model));
        GLint view_id = glGetUniformLocation(program_id_,
                                             "view");
        glUniformMatrix4fv(view_id, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
        GLint projection_id = glGetUniformLocation(program_id_,
                                                   "projection");
        glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE,
                           glm::value_ptr(projection));

        // setup MVP
        glm::mat4 MVP = projection*view*model;
        glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

        // pass the current time stamp to the shader.
        glUniform1f(glGetUniformLocation(program_id_, "time"), time);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

        glDisable(GL_BLEND);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
