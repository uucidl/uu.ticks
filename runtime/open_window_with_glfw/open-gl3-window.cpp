#include <math.h> // sqrt
#include <stdio.h> // snprintf

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <api.h>

static void do_keyboard (GLFWwindow* window, int key, int scancode,
                         int action, int mods)
{
        if (GLFW_KEY_ESCAPE == key) {
                glfwSetWindowShouldClose(window, GL_TRUE);
        }
}

static void do_mouse_button (GLFWwindow* window, int button, int action,
                             int mods)
{
        if (GLFW_MOUSE_BUTTON_2 == button) {
                glfwSetWindowShouldClose(window, GL_TRUE);
        }

}

void open_window(char const * title, bool const prefers_fullscreen)
{
        if (!glfwInit()) {
                printf("glfw: could not initialize\n");
                return;
        }

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        GLFWvidmode const * mode = glfwGetVideoMode(monitor);

        int const screen_wh[] = {
                mode->width,
                mode->height,
        };

        int window_wh[] = {
                screen_wh[0],
                screen_wh[1],
        };

        if (!prefers_fullscreen) {
                window_wh[0] = static_cast<int>(window_wh[0] * 1.0 / sqrt(2.0));
                window_wh[1] = static_cast<int>(window_wh[1] * 1.0 / sqrt(2.0));
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_SAMPLES, 4);
        GLFWwindow* window = glfwCreateWindow
                             (window_wh[0], window_wh[1], title,
                              prefers_fullscreen ? monitor : NULL,
                              NULL);
        if (!window) {
                printf("could not create window\n");
                return;
        }

        glfwSetKeyCallback(window, do_keyboard);
        glfwSetMouseButtonCallback(window, do_mouse_button);

        glfwMakeContextCurrent(window);
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (GLEW_OK != err) {
                /* Problem: glewInit failed, something is seriously wrong. */
                fprintf(stderr, "glew error: %s\n", glewGetErrorString(err));
                return;
        }
        fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

        while(!glfwWindowShouldClose(window)) {
                glfwMakeContextCurrent(window);

                int width, height;
                glfwGetFramebufferSize(window, &width, &height);
                glViewport(0, 0, width, height);

                render_next_gl(now_micros());
                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        glfwDestroyWindow(window);
        glfwTerminate();
}
