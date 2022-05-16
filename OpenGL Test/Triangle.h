#pragma once
#include <glm.hpp>

struct Triangle {
    glm::vec3 vertex1;
    glm::vec3 color1;
    glm::vec3 normal1;

    glm::vec3 vertex2;
    glm::vec3 color2;
    glm::vec3 normal2;

    glm::vec3 vertex3;
    glm::vec3 color3;
    glm::vec3 normal3;

    void mofify(int translateX , int translateY, int translateZ) {

        vertex1.x += translateX;
        vertex2.x += translateX;
        vertex3.x += translateX;

        vertex1.y += translateY;
        vertex2.y += translateY;
        vertex3.y += translateY;

        vertex1.z += translateZ;
        vertex2.z += translateZ;
        vertex3.z += translateZ;
    }
};
