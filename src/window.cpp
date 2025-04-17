#include "window.h"

#include <GLFW/glfw3.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

// set the default static values
// screen static variables
int Window::width = 100;
int Window::height = 100;
GLFWwindow* Window::glfw_window = Window::create_glfw_window();

// mouse static variables
int Window::mouse_last_x = Window::width / 2;
int Window::mouse_last_y = Window::height / 2;
bool Window::mouse_capture = true;
bool Window::first_mouse_update = true;

// camera static varaibles
float Window::yaw = 90;
float Window::pitch = 0;
glm::vec3 Window::camera_position = glm::vec3(0.0, 0.0, -1.5);
glm::vec3 Window::camera_up = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 Window::camera_direction = glm::vec3(
    std::cos(glm::radians(Window::yaw)) * std::cos(glm::radians(Window::pitch)),
    std::sin(glm::radians(Window::pitch)),
    std::sin(glm::radians(Window::yaw)) * std::cos(glm::radians(Window::pitch))
);
glm::mat4 Window::view = glm::lookAt(
    Window::camera_position,
    Window::camera_position + glm::normalize(Window::camera_direction),
    Window::camera_up
);
glm::mat4 Window::proj = glm::perspective(
    glm::radians(45.0f),
    static_cast<float>(Window::width) / static_cast<float>(Window::height),
    0.1f,
    1000.0f
);

Window::Window()
{
    glfwSetErrorCallback(Window::glfw_error_callback);

    glfwSetKeyCallback(Window::glfw_window, Window::glfw_key_callback);
    glfwSetFramebufferSizeCallback(
        Window::glfw_window,
        Window::glfw_window_resize_callback
    );
    glfwSetCursorPosCallback(Window::glfw_window, Window::glfw_mouse_callback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glViewport(0, 0, Window::width, Window::height);

    glfwSetInputMode(Window::glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Window::~Window()
{
    glfwDestroyWindow(this->glfw_window);
    glfwTerminate();
}

void Window::glfw_error_callback(int error, const char* desc)
{
    std::cerr << "glfw_error_callback " << error << " " << desc << std::endl;
    fflush(stdout);
    fflush(stderr);
    std::exit(1);
}

GLFWwindow* Window::create_glfw_window()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfw_window = glfwCreateWindow(width, height, "ngi window", NULL, NULL);
    if (!glfw_window) {
        glfw_error_callback(-1, "window was null");
    }

    glfwMakeContextCurrent(glfw_window);

    int glad_version = gladLoadGL(glfwGetProcAddress);
    if (glad_version == 0) {
        glfw_error_callback(-1, "glad verison what 0");
    }
    return Window::glfw_window;
}

void Window::glfw_key_callback(
    GLFWwindow* window,
    int key,
    [[maybe_unused]] int scancode,
    int action,
    [[maybe_unused]] int mods
)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        if (!mouse_capture) {
            mouse_capture = true;
            first_mouse_update = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            mouse_capture = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void Window::glfw_window_resize_callback(
    [[maybe_unused]] GLFWwindow* window,
    int new_width,
    int new_height
)
{
    width = new_width;
    height = new_height;
    glViewport(0, 0, new_width, new_height);
}

void Window::glfw_mouse_callback(
    [[maybe_unused]] GLFWwindow* window,
    double xpos,
    double ypos
)
{
    if (first_mouse_update) {
        mouse_last_x = static_cast<int>(xpos);
        mouse_last_y = static_cast<int>(ypos);
        first_mouse_update = false;
    }

    if (mouse_capture) {
        double xoffset = xpos - static_cast<double>(mouse_last_x);
        // reversed since y-coordinates range from bottom to top
        double yoffset = static_cast<double>(mouse_last_y) - ypos;
        mouse_last_x = static_cast<int>(xpos);
        mouse_last_y = static_cast<int>(ypos);

        const double sensitivity = 0.1;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        yaw += static_cast<float>(xoffset);
        pitch += static_cast<float>(yoffset);
        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }
    }
}

void camera_update_per_frame(GLFWwindow* window)
{
    const float camera_speed = 0.25f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        Window::camera_position +=
            camera_speed * glm::normalize(Window::camera_direction);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        Window::camera_position -=
            camera_speed * glm::normalize(Window::camera_direction);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        Window::camera_position -=
            glm::normalize(
                glm::cross(Window::camera_direction, Window::camera_up)
            ) *
            camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        Window::camera_position +=
            glm::normalize(
                glm::cross(Window::camera_direction, Window::camera_up)
            ) *
            camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        Window::camera_position += Window::camera_up * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        Window::camera_position -= Window::camera_up * camera_speed;
    }
    Window::camera_direction.x = std::cos(glm::radians(Window::yaw)) *
                                 std::cos(glm::radians(Window::pitch));
    Window::camera_direction.y = sin(glm::radians(Window::pitch));
    Window::camera_direction.z = std::sin(glm::radians(Window::yaw)) *
                                 std::cos(glm::radians(Window::pitch));
    Window::view = glm::lookAt(
        Window::camera_position,
        Window::camera_position + glm::normalize(Window::camera_direction),
        Window::camera_up
    );
    Window::proj = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(Window::width) / static_cast<float>(Window::height),
        0.1f,
        1000.0f
    );
}

GLFWwindow* Window::get_glfw_window() { return glfw_window; }
