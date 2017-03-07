// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "triangle/triangle.h"

Triangle triangle;


void Init() {
    // sets background color
    glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);

    triangle.Init();
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    //Simple spiral ----> k = 0;
    //Fermat spiral ----> k = 1;
    /***************************************************/
     int k=1;
     /**************************************************/
    //Simple spiral
    if(k==0){

   //Transformation


  for(int i = 0 ; i<60; i++){

      //scale
      glm::mat3 S = glm::mat3(0.07*i/50);

      //first rotation
      glm::mat3 R1 = glm::mat3(1);
      float a = glm::radians(float(25));
      R1[0][0] =  cos(a*float(i));
      R1[0][1] =  sin(a*float(i));
      R1[1][0] = -sin(a*float(i));
      R1[1][1] =  cos(a*float(i));

      //second rotation
      glm::mat3 R2 = glm::mat3(1);
      float b = glm::radians(float(40));
      R2[0][0] =  cos(b);
      R2[0][1] =  sin(b);
      R2[1][0] = -sin(b);
      R2[1][1] =  cos(b);

      // translation
      glm::mat4 T = glm::mat4(1);
      T[3][0] = .12*i;
      T[3][1] = .12*i;
      glm::mat4 model = glm::mat4(S)*glm::mat4(R1)*glm::mat4(T)*glm::mat4(R2);
         // compute transformations here
      triangle.Draw(model);

         }
    }

    //Fermat spiral
    if(k==1){
        for(int i = -200 ; i<500; i++){

            //scale
            glm::mat3 S = glm::mat3(0.02);

            //first rotation
            glm::mat3 R1 = glm::mat3(1);
            float a = glm::radians(137.508);
            R1[0][0] =  cos(a*float(i));
            R1[0][1] =  sin(a*float(i));
            R1[1][0] = -sin(a*float(i));
            R1[1][1] =  cos(a*float(i));

            //second rotation
            glm::mat3 R2 = glm::mat3(1);
            float b = glm::radians(float(30));
            R2[0][0] =  cos(b);
            R2[0][1] =  sin(b);
            R2[1][0] = -sin(b);
            R2[1][1] =  cos(b);

            // translation
            double c= 1.3;
            glm::mat4 T = glm::mat4(1);
            T[3][0] = c*sqrt(double(i));
            T[3][1] = c*sqrt(double(i));
            glm::mat4 model = glm::mat4(S)*glm::mat4(R1)*glm::mat4(T)*glm::mat4(R2);
               // compute transformations here
            triangle.Draw(model);

          }


        }
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, char *argv[]) {
    // GLFW Initialization
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow* window = glfwCreateWindow(512, 512, "spiral", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init();

    // render loop
    while(!glfwWindowShouldClose(window)) {

        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    triangle.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}

