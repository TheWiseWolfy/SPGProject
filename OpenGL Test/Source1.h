

std::string textFileRead(const char* fn);

unsigned int createShader(const char* shaderSource, unsigned int shaderType);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow* initWindow();
