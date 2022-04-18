#pragma once

std::string textFileRead(const char* fn);

glm::mat4 calculateFinalMatrix();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow* initWindow();
