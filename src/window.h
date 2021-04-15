#ifndef FRACTALS_PLATONIC4D_WINDOW_H
#define FRACTALS_PLATONIC4D_WINDOW_H

int WIDTH = 1280;
int HEIGHT = 720;

float cameraDistance = 3.0f;
double scroll_speed = 0.2;

double mouse_speed = 0.2;
double xpos, ypos;
double mouse_pos_x = 0.0;
double mouse_pos_y = 0.0;
float horizontal_angle = 0.0f;
float vertical_angle = 0.0f;

bool leftButtonPressed = false;
bool wireframe = false;

double deltaTime = 0.0f;
double lastFrame = 0.0f;

glm::vec3 updateCamera(GLFWwindow* window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

#endif //FRACTALS_PLATONIC4D_WINDOW_H
