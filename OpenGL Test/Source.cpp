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

#include "LookUpTables.h"
#include "GenericCube.h"
#include "Triangle.h"
#include "PerlinNoise.h"

#define PI glm::pi<float>()

float verticalRotationLevel = 0;
float horizontalRotationLevel = 0;

vector<Triangle> trianglesToDraw;
vector<Triangle> wireframeCubes;

uint sizeV = 50;
Volume volume(sizeV, sizeV, sizeV);

float granularity = 0.9;
float offset = 0.9;
PerlinNoise noiseGenerator;

int openGLmain()
{
	GLFWwindow* window = initWindow();

	glfwSetKeyCallback(window, key_callback);

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


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);


	//Main aplication loop
	while (!glfwWindowShouldClose(window))
	{
		offset += 0.02;
		generateTerrain();



		//Here we check for inputs 
		processInput(window);

		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//GLuint matrixID = glGetUniformLocation(shaderProgram,"modelViewProjectionMatrix");
		GLuint matrixID = myCoolShader.getUniform("modelViewProjectionMatrix");

		glm::mat4 final = calculateFinalMatrix();
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(final));

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBufferData(GL_ARRAY_BUFFER, trianglesToDraw.size() * sizeof(Triangle), trianglesToDraw.data(), GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0 , trianglesToDraw.size() * 3 );


		glBufferData(GL_ARRAY_BUFFER, nrVertices * 6 * sizeof(float), cube, GL_STATIC_DRAW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		for(int f1 = 0; f1 < sizeV -1; f1++){
			for (int f2 = 0; f2 < sizeV -1; f2++) {
				for (int f3 = 0; f3 < sizeV - 1; f3++) {

					if (volume.getCube(f1, f2, f3).getIndex() != 0) {
						glm::mat4 final = calculateFinalMatrix();
						final = glm::translate(final, glm::vec3(f1, f2, f3));
						glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(final));

						//glDrawArrays(GL_TRIANGLES, 0, nrVertices);
					}
				}
			}
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	return 0;
}



int main() {

	generateTerrain();
	displayTerrain();
	openGLmain();
}

void changeSeed() {
	srand(time(NULL));

	noiseGenerator = PerlinNoise((float)rand());
}


void generateTerrain() {

	volume.clear();

	for (int f1 = 0; f1 < sizeV; f1++) {
		for (int f2 = 0; f2 < sizeV; f2++) {

		/*	
			for (int f3 = 0; f3 < sizeV; f3++) {

				float maxHeight = noiseGenerator.noise(f1 / (float)sizeV * granularity, f2 / (float)sizeV * granularity, f3 / (float)sizeV * granularity);
				volume.setElement(f1, f3, f2, maxHeight);
			}*/
			

			float noise = noiseGenerator.noise(f1 / (float)sizeV * granularity + offset, f2 / (float)sizeV * granularity + offset ,0.7 );
	
			int actualHeight = noise * sizeV ;

			if (actualHeight > sizeV) {
				actualHeight = sizeV ;
			}

			if (actualHeight < 0) {
				actualHeight = 0;
			}

			for (int f3 = 0; f3 < actualHeight; f3++) {

				volume.setElement(f1, f3, f2, 1);
			}
		}
	}

	displayTerrain();

}


void displayTerrain() {
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

					trig.mofify(f1, f2, f3);
					trianglesToDraw.push_back(trig);
				}
			}
		}
	}
}




float cameraPozX = -5.0f;
float cameraPozY = -13.0f;
float cameraPozZ = -30.0f;

glm::mat4 calculateFinalMatrix() {
	glm::mat4 projectionMatrix;

	projectionMatrix = glm::perspective(PI / 3, 1.0f, 0.1f, 100.0f);
	float xv = 2, yv = 2, zv = 30; //originea sistemului de observare

	//viewMatrix = glm::lookAt(glm::vec3(xv, yv, zv), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	// note that we’re translating the scene in the reverse direction

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, horizontalRotationLevel, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, verticalRotationLevel, glm::vec3(1.0f, 0.0f, 0.0f));

	viewMatrix = glm::translate(viewMatrix, glm::vec3(cameraPozX, cameraPozY, cameraPozZ));

	glm::mat4 final = projectionMatrix * viewMatrix * model;
	return final;
}



GLFWwindow* initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return window;
}


std::string textFileRead(const char* fn){
	std::ifstream ifile(fn);
	std::string filetext;
	while (ifile.good()) {
		std::string line;
		std::getline(ifile, line);
		filetext.append(line + "\n");
	}
	return filetext;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window){

	glfwPollEvents();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		verticalRotationLevel += 0.1;

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		verticalRotationLevel -= 0.1;

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		horizontalRotationLevel += 0.1;

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		horizontalRotationLevel -= 0.1;

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		cameraPozX += 0.5;

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		cameraPozX -= 0.5;


	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		cameraPozY += 0.5;

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		cameraPozY -= 0.5;


	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		cameraPozZ += 1;


	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
		cameraPozZ -= 1;

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		changeSeed();
		generateTerrain();

	if (key == GLFW_KEY_U && action == GLFW_PRESS) {
		volume.setThreshold(volume.getThreshold() + 0.05);
		displayTerrain();
	}
	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		volume.setThreshold(volume.getThreshold() - 0.05);
		displayTerrain();
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
}


