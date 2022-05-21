#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

#include "stb_image.h"
#include "ProceduralGenerator.h"
#include "Shader.h"
#include "Camera.h"

#include "Triangle.h"

#define PI glm::pi<float>()

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
Camera camera;

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;
vector<Triangle> trianglesToDraw;
ProceduralGenerator gen(10, 10, 10);

//Local fuctions 
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
glm::mat4 calculateFinalMatrix();
int openGLmain(GLFWwindow* window);
GLFWwindow* initWindow();

int main() {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	camera = Camera(position);

	GLFWwindow* window = initWindow();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);


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

	glm::vec3 lightPos(0, 1, 5);
	glm::vec3 viewPos(2, 3, 6);

	//Main aplication loop
	while (!glfwWindowShouldClose(window))
	{
		trianglesToDraw = gen.GenerateAroundPlayer(camera.Position);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Here we check for inputs 
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//GLuint matrixID = glGetUniformLocation(shaderProgram,"modelViewProjectionMatrix");
		GLuint matrixID = myCoolShader.getUniform("modelViewProjectionMatrix");
		
		glm::mat4 final = calculateFinalMatrix();
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(final));
		
		int lightPosLoc = myCoolShader.getUniform("lightPos");
		//glUniform3fv(lightPosLoc, 1, glm::value_ptr(glm::vec3(0,0,90)   ));
		glUniform3fv(lightPosLoc, 1, glm::value_ptr(camera.Position));

		GLuint viewPosLoc = myCoolShader.getUniform("viewPos");
		glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.Position));

		GLuint cameraFrontLoc = myCoolShader.getUniform("cameraFront");
		glUniform3fv(cameraFrontLoc, 1, glm::value_ptr(camera.Front));
		//cout << camera.Front.x << " " << camera.Front.y << " " << camera.Front.z << "\n";


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBufferData(GL_ARRAY_BUFFER, trianglesToDraw.size() * sizeof(Triangle), trianglesToDraw.data(), GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, trianglesToDraw.size() * 3);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
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

	glGetError();

	return window;
}

//Callbacks

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
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


uint number = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	float cameraSpeed = 20.0f * deltaTime;

	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
	//	cameraPozZ -= 1;

	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
	/*	changeSeed();
		generateTerrain();*/
	}

	if (key == GLFW_KEY_U && action == GLFW_PRESS) {
	/*	volume.setThreshold(volume.getThreshold() + 0.05);
		generateTerrainGeometry();*/
	}
	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		/*volume.setThreshold(volume.getThreshold() - 0.05);
		generateTerrainGeometry();*/
	}

	if (key == GLFW_KEY_J && action == GLFW_PRESS) {

	}

	if (key == GLFW_KEY_K && action == GLFW_PRESS) {
	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		
	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		
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

