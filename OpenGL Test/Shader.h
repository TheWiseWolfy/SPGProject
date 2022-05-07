#pragma once

#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <glm.hpp>

#include <string>

using namespace std;

class Shader
{
public:
    // the program ID

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

    GLuint getUniform(const char* uniformName) {return glGetUniformLocation(ID, uniformName); }
    GLuint getID() { return ID; }
private:
    unsigned int ID;

    void checkCompileErrors(GLuint shader, std::string type);

};

