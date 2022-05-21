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

    void move(glm::vec3 distanceMoved) {

        vertex1 += distanceMoved;
        vertex2 += distanceMoved;
        vertex3 += distanceMoved;
    }

    void color(glm::vec3 newColor) {
        color1 = newColor;
        color2 = newColor;
        color3 = newColor;
    }

};
