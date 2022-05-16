#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

#include "Source.h"
#include "stb_image.h"

#include "Shader.h"
#include "Volume.h"
#include "Camera.h"

#include "LookUpTables.h"
#include "GenericCube.h"
#include "Triangle.h"
#include "PerlinNoise.h"

#define PI glm::pi<float>()
vector<Triangle> trianglesToDraw;
vector<Triangle> wireframeCubes;

uint sizeV = 100;
Volume volume(sizeV, sizeV, sizeV);

float granularity = 8.0f;
float offset = 0.0f;
PerlinNoise noiseGenerator;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
Camera camera;

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;

int main() {
	glm::vec3 position = glm::vec3(0.0f, 90.0f, -30.0f);
	camera = Camera(position);

	GLFWwindow* window = initWindow();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	generateTerrain();
	generateTerrainGeometry();
	openGLmain(window);
}

int openGLmain(GLFWwindow* window){
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 4.0f);

	Shader myCoolShader = Shader("vertex.vert", "fragment.frag");
	glUseProgram(myCoolShader.getID());

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)(2 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);

	//Main aplication loop
	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Here we check for inputs 
		processInput(window);

		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//GLuint matrixID = glGetUniformLocation(shaderProgram,"modelViewProjectionMatrix");
		GLuint matrixID = myCoolShader.getUniform("modelViewProjectionMatrix");

		
		glm::vec3 lightPos(0, 1, 5);
		glm::vec3 viewPos(2, 3, 6);
		
		glm::mat4 final = calculateFinalMatrix();
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(final));
		
		int lightPosLoc = myCoolShader.getUniform("lightPos");
		glUniform3fv(lightPosLoc, 1, glm::value_ptr(camera.Position));

		GLuint viewPosLoc = myCoolShader.getUniform("viewPos");
		glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.Position));


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBufferData(GL_ARRAY_BUFFER, trianglesToDraw.size() * sizeof(Triangle), trianglesToDraw.data(), GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, trianglesToDraw.size() * 3);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}


void changeSeed() {
	srand(time(NULL));
	noiseGenerator = PerlinNoise((float)rand());
}

void generateTerrain() {

	volume.clear();

	for (int f1 = 0; f1 < sizeV; f1++) {
		for (int f2 = 0; f2 < sizeV; f2++) {

		
			for (int f3 = 0; f3 < sizeV; f3++) {

				float maxHeight = noiseGenerator.noise(f1 / (float)sizeV * granularity, f2 / (float)sizeV * granularity, f3 / (float)sizeV * granularity);
				volume.setElement(f1, f3, f2, maxHeight);
			}
			

			//float noise = noiseGenerator.noise(f1 / (float)sizeV * granularity + offset, f2 / (float)sizeV * granularity + offset ,0.7 );
			//int actualHeight = noise * sizeV ;

			//if (actualHeight > sizeV) {
			//	actualHeight = sizeV ;
			//}

			//if (actualHeight < 0) {
			//	actualHeight = 0;
			//}

			//for (int f3 = 0; f3 < actualHeight; f3++) {

			//	volume.setElement(f1, f3, f2, 1);
			//}
		}
	}

	generateTerrainGeometry();
}

glm::vec3 oldNormal = glm::vec3(0,1,0);


void generateTerrainGeometry() {
	volume.computeCubes();

	trianglesToDraw.clear();

	for (int f1 = 0; f1 < sizeV - 1; f1++) {

		for (int f2 = 0; f2 < sizeV - 1; f2++) {

			for (int f3 = 0; f3 < sizeV - 1; f3++) {

				int volumeIndex = volume.getCube(f1, f2, f3).getIndex();

				//pentru fiecare tringhi
				for (int f4 = 0; Tables::triTable[volumeIndex][f4] != -1; f4 += 3) {

					int vertex1Index = Tables::triTable[volumeIndex][f4];
					int vertex2Index = Tables::triTable[volumeIndex][f4 + 1];
					int vertex3Index = Tables::triTable[volumeIndex][f4 + 2];

					int a0 = Tables::cornerIndexAFromEdge[vertex1Index];
					int b0 = Tables::cornerIndexBFromEdge[vertex1Index];

					int a1 = Tables::cornerIndexAFromEdge[vertex2Index];
					int b1 = Tables::cornerIndexBFromEdge[vertex2Index];

					int a2 = Tables::cornerIndexAFromEdge[vertex3Index];
					int b2 = Tables::cornerIndexBFromEdge[vertex3Index];

					Triangle trig;

					trig.vertex1 = avarageVec3(corners[a0], corners[b0]);
					trig.vertex2 = avarageVec3(corners[a1], corners[b1]);
					trig.vertex3 = avarageVec3(corners[a2], corners[b2]);

					float finalColorR = ((float)f3 / sizeV);
					float finalColorG = ((float)f2 / sizeV);
					float finalColorB = ((float)f1 / sizeV);


					trig.color1.r = finalColorR;
					trig.color1.g = finalColorG;
					trig.color1.b = finalColorB;

					trig.color2.r = finalColorR;
					trig.color2.g = finalColorG;
					trig.color2.b = finalColorB;

					trig.color3.r = finalColorR;
					trig.color3.g = finalColorG;
					trig.color3.b = finalColorB;

					trig.normal1.x = trig.vertex1.y * trig.vertex2.z - trig.vertex1.z * trig.vertex2.y;
					trig.normal1.y = trig.vertex1.z * trig.vertex2.x - trig.vertex1.x * trig.vertex2.z;
					trig.normal1.z = trig.vertex1.x * trig.vertex2.y - trig.vertex1.y * trig.vertex2.x;

					float scalarProduct = oldNormal.x * trig.normal1.x + oldNormal.y * trig.normal1.y + oldNormal.z * trig.normal1.z;

					if (scalarProduct < 0) {
						trig.normal1 = -trig.normal1;
					}
					trig.normal2 = trig.normal1;
					trig.normal3 = trig.normal1;

					oldNormal = trig.normal1;

					trig.mofify(f1, f2, f3);
					trianglesToDraw.push_back(trig);
				}
			}
		}
	}
}

glm::mat4 calculateFinalMatrix() {
	glm::mat4 projectionMatrix;

	//Perspective Matrix
	projectionMatrix = glm::perspective(PI / 3, 1.0f, 0.1f, 300.0f);

	//View Matrix
	glm::mat4 viewMatrix = camera.GetViewMatrix();

	// Model Matrix
	glm::mat4 model = glm::mat4(1.0f);

	//viewMatrix = glm::translate(viewMatrix, glm::vec3(cameraPozX, cameraPozY, cameraPozZ));

	glm::mat4 final = projectionMatrix * viewMatrix * model;
	return final;
}



GLFWwindow* initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		//return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		//return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window){

	glfwPollEvents();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	float cameraSpeed = 20.0f * deltaTime;

	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
	//	cameraPozZ -= 1;

	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		changeSeed();
		generateTerrain();
	}

	if (key == GLFW_KEY_U && action == GLFW_PRESS) {
		volume.setThreshold(volume.getThreshold() + 0.05);
		generateTerrainGeometry();
	}
	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		volume.setThreshold(volume.getThreshold() - 0.05);
		generateTerrainGeometry();
	}

	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		granularity -= 0.1;
		generateTerrain();
	}

	if (key == GLFW_KEY_K && action == GLFW_PRESS) {
		granularity += 0.1;
		generateTerrain();
	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		offset -= 0.05;
		generateTerrain();
	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		offset += 0.05;
		generateTerrain();
	}


	//if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	//	glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

