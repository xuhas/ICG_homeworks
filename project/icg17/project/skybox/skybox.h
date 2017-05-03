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

    void deleteCubeMap(GLuint& texture) {
        glDeleteTextures(1, &texture);
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

        // cube vertices for vertex buffer object
        GLfloat cube_vertices[] = {
            -1.0,  1.0,  1.0,
            -1.0, -1.0,  1.0,
            1.0, -1.0,  1.0,
            1.0,  1.0,  1.0,
            -1.0,  1.0, -1.0,
            -1.0, -1.0, -1.0,
            1.0, -1.0, -1.0,
            1.0,  1.0, -1.0,
        };
        glGenBuffers(1, &vertex_buffer_object_position_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);

        // cube indices for index buffer object
        GLushort cube_indices[] = {
            0, 1, 2, 3,
            3, 2, 6, 7,
            7, 6, 5, 4,
            4, 5, 1, 0,
            0, 3, 7, 4,
            1, 2, 6, 5,
        };
        num_indices_ = sizeof(cube_indices)/sizeof(GLushort);
        glGenBuffers(1, &vertex_buffer_object_index_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        GLuint loc_position = glGetAttribLocation(program_id_, "position");
        glEnableVertexAttribArray(loc_position);
        glVertexAttribPointer(loc_position, 2, GL_FLOAT, GL_FALSE, 0, 0);

        GLuint texture;
        setupCubeMap(texture, "desertsky_bk.tga", "desertsky_dn.tga", "desertsky_ft.tga", "desertsky_lf.tga", "desertsky_rt.tga", "desertsky_up.tga");

        this->texture_id_ = texture;
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
        GLuint tex_id = glGetUniformLocation(program_id_, "cubemap");
        glUniform1i(tex_id, 4 /*GL_TEXTURE0*/);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

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
        glDeleteTextures(1, &texture_id_);
    }

    void Draw(float time, const glm::mat4 &model = IDENTITY_MATRIX,
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
        GLuint MVP_id = glGetUniformLocation(program_id_, "MVP");
        glUniformMatrix4fv(MVP_id, 1, GL_FALSE, value_ptr(MVP));

        glDrawElements(GL_QUADS, num_indices_, GL_UNSIGNED_SHORT, 0);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
