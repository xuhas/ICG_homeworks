#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

class Skybox {

private:
    GLuint vertex_array_id_;                // vertex array object
    GLuint vertex_buffer_object_position_;  // memory buffer for positions
    GLuint vertex_buffer_object_index_;     // memory buffer for indices
    GLuint program_id_;                     // GLSL shader program ID
    GLuint texture_id_;                     // texture ID
    GLuint num_indices_;                    // number of vertices to render
    GLuint MVP_id_;                         // model, view, proj matrix ID

public:

    void initCubeMap(GLuint& texture) {
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_CUBE_MAP);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    void setupOneSide(GLuint& texture, const char* filename, GLenum type)
    {
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

        glTexImage2D(type, 0, GL_RGBA, width, height, 0,
                     nb_component == 4 ? GL_RGBA : GL_RGB,
                     GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);
    }

    void setupCubeMap(GLuint& texture, const char* xpos, const char* xneg, const char* ypos, const char* yneg, const char* zpos, const char* zneg) {
        initCubeMap(texture);
        setupOneSide(texture, xpos, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
        setupOneSide(texture, xneg, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
        setupOneSide(texture, ypos, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
        setupOneSide(texture, yneg, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
        setupOneSide(texture, zpos, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
        setupOneSide(texture, zneg, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
    }
    void Init() {
        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("skybox_vshader.glsl",
                                              "skybox_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // Position buffer
        const GLfloat position[] = {-1.0f, -1.0f,  1.0f, // left, bottom, front
                                    1.0f, -1.0f,  1.0f,  // right, bottom, front
                                    1.0f,  1.0f,  1.0f,  // right, top, front
                                    -1.0f,  1.0f,  1.0f, // left, top, front
                                    -1.0f, -1.0f, -1.0f, // left, bottom, back
                                    1.0f, -1.0f, -1.0f,  // right, bottom, back
                                    1.0f,  1.0f, -1.0f,  // right, top, back
                                    -1.0f,  1.0f, -1.0f};// left, top, back

        glGenBuffers(1, &vertex_buffer_object_position_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

        // position shader attribute
        // fetch attribute ID for vertex positions
        GLuint loc_position = glGetAttribLocation(program_id_, "position");
        glEnableVertexAttribArray(loc_position); // Enable it
        glVertexAttribPointer(loc_position, 3, GL_FLOAT, DONT_NORMALIZE,
                              ZERO_STRIDE, ZERO_BUFFER_OFFSET);

        // index buffer
        const GLuint index[] = {0, 1, 2,  // front face triangle 1
                                0, 2, 3,  // front face triangle 2
                                1, 5, 6,  // right face triangle 1
                                1, 6, 2,  // right face triangle 2
                                5, 4, 7,  // back face triangle 1
                                5, 7, 6,  // back face triangle 2
                                4, 0, 3,  // left face triangle 1
                                4, 3, 7,  // left face triangle 2
                                3, 2, 6,  // top face triangle 1
                                3, 6, 7,  // top face triangle 2
                                1, 0, 4,  // bottom face triangle 1
                                1, 4, 5}; // bottom face triangle 2

        num_indices_ = sizeof(index) / sizeof(GLuint);

        glGenBuffers(1, &vertex_buffer_object_index_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

        //texture
        GLuint texture;
        setupCubeMap(texture,
                     "TropicalSunnyDayRight2048.png",
                     "TropicalSunnyDayLeft2048.png",
                     "TropicalSunnyDayBack2048.png",
                     "TropicalSunnyDayFront2048.png",
                     "TropicalSunnyDayUp2048.png",
                     "TropicalSunnyDayDown2048.png");

        this->texture_id_ = texture;
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
        GLuint tex_id = glGetUniformLocation(program_id_, "cubemap");
        glUniform1i(tex_id, 4 /*GL_TEXTURE0*/);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        // other uniforms
        MVP_id_ = glGetUniformLocation(program_id_, "MVP");

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_position_);
        glDeleteBuffers(1, &vertex_buffer_object_index_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1, &vertex_array_id_);
    }

    void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // bind textures
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);

        // setup MVP
        glm::mat4 MVP = projection*view*model;
        glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

        // draw
        glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
