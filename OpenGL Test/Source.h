#pragma once


glm::mat4 calculateFinalMatrix();

void generateTerrain();
void generateTerrainGeometry();
int openGLmain(GLFWwindow* window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void processInput(GLFWwindow* window);
GLFWwindow* initWindow();

glm::vec3 avarageVec3(glm::vec3 polygon1, glm::vec3 polygon2) {
	glm::vec3 tmp = polygon1 + polygon2;

	tmp.x /= 2;
	tmp.y /= 2;
	tmp.z /= 2;

	return tmp;
}

int nrFaces = 6;
int nrVerticesPerFace = 6;
int nrVertices = nrFaces * nrVerticesPerFace;
float L = 0.5f;

//trebuie adaugate coordonatele vectorilor normali pentru fiecare vertex, pe langa pozitie
