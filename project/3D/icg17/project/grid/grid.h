#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

class Grid {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID
        GLuint texture_id_;                     // texture ID
        GLuint num_indices_;                    // number of vertices to render
        GLuint MVP_id_;                         // model, view, proj matrix ID

    public:
        void Init() {
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

				bool use_new = true;

				if (use_new){ //use new grid
					int grid_dim = 512;
					float min_pos = -1.0;
					float pos_range = 2.0;

					int xLength = grid_dim;
					int yLength = grid_dim;

					int offset = 0;

					// First, build the data for the vertex buffer
					for (int y = 0; y < yLength; y++) {
						for (int x = 0; x < xLength; x++) {
							float xRatio = x / (float) (xLength - 1);

							// Build our heightmap from the top down, so that our triangles are
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
				}
				else{ //else use old grid
					int k=0;
					float i = -1.0f;
					for(float j = 1.0f ; j>= -1.0f ; j-=0.01f){

						for( i = -1.0f ; i <= 1.0f ; i+=0.01f){
							vertices.push_back(i); vertices.push_back( j);
							vertices.push_back( i); vertices.push_back(j - 0.01f);
							vertices.push_back( i + 0.01f); vertices.push_back( j);
							vertices.push_back(i-0.01f); vertices.push_back(j-0.01f);

							indices.push_back(k);
							indices.push_back(k+1);
							indices.push_back(k+2);
							indices.push_back(k+3);

							k+=4;

						}
						//vertices.last()=; vertices.push_back(j-0.01f);
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
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

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

			glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
