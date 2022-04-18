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

#define PI glm::pi<float>()

float vertices[] = {
	// positions          // colors           // texture coords
	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

unsigned int indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
};

float texCoords[] = {
0.0f, 0.0f, // lower-left corner
1.0f, 0.0f, // lower-right corner
0.5f, 1.0f // top-center corner
};


int main() {
	Volume volume(3, 3, 3);

	for (int f1 = 0; f1 < 2; f1++) {
		for (int f2 = 0; f2 < 3; f2++) {
			for (int f3 = 0; f3 < 3; f3++) {
				volume.setElement(f1, f2, f3, 1);
			}
		}
	}


	volume.computeCubes();
	for (int f1 = 0; f1 <2; f1++) {

		for (int f2 = 0; f2 < 2; f2++) {

			for (int f3 = 0; f3 < 2; f3++) {

				//cout << volume.getCube(f1, f2, f3)->getIndex() << " ";
				cout << Tables::edgeTable[volume.getCube(f1, f2, f3).getIndex()] << "   ";

			}
			cout << "\n";
		}
		cout << "\n";
	}

}

int altmain()
{



	GLFWwindow* window = initWindow();

	//Texture space
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Minmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Loading texture

	int width, height, nrChannels;
	unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);	//glGenerateMipmap(GL_TEXTURE_2D);



	Shader myCoolShader = Shader("vertex.vert", "fragment.frag");
	glUseProgram(myCoolShader.getID());
	



	//We encapsulate a good ammount of state inside of a VAO (Vertex Array Object)
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//EVERYTHING HERE WILL BE BOUND TO THE VAO
	unsigned int VBO;
	//We generate a buffer
	glGenBuffers(1, &VBO);
	//We bind it to GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//We load stuff into GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//Set vertex atribute pointers
	
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);


	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//We bind the desired VAO for drawing
	glBindVertexArray(VAO);

	//Main aplication loop
	while (!glfwWindowShouldClose(window))
	{
		//Here we check for inputs 
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		//GLuint matrixID = glGetUniformLocation(shaderProgram,"modelViewProjectionMatrix");
		GLuint matrixID = myCoolShader.getUniform("modelViewProjectionMatrix");

		glm::mat4 final = calculateFinalMatrix();
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(final));


		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	return 0;
}

glm::mat4 calculateFinalMatrix() {
	glm::mat4 projectionMatrix;

	projectionMatrix = glm::perspective(PI / 3, 1.0f, 0.1f, 100.0f);
	float xv = 2, yv = 2, zv = 30; //originea sistemului de observare

	//viewMatrix = glm::lookAt(glm::vec3(xv, yv, zv), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 viewMatrix = glm::mat4(1.0f);
	// note that we’re translating the scene in the reverse direction
	viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}