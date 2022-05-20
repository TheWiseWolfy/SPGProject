#pragma once


glm::mat4 calculateFinalMatrix();

int openGLmain(GLFWwindow* window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void processInput(GLFWwindow* window);
GLFWwindow* initWindow();


int nrFaces = 6;
int nrVerticesPerFace = 6;
int nrVertices = nrFaces * nrVerticesPerFace;
float L = 0.5f;

//trebuie adaugate coordonatele vectorilor normali pentru fiecare vertex, pe langa pozitie
