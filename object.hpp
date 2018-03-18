#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SOIL.h>

#include <vector>

#ifndef CREATE_OBJECTS_HPP
#define CREATE_OBJECTS_HPP

struct TexturedObject
{
    int vertexCount;
    GLuint vertexBuffer, texture;
    glm::mat4 model = glm::mat4(1);

    TexturedObject() {}

    TexturedObject(const char* texturefile)
    {
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        int width, height;
        unsigned char* image = SOIL_load_image(texturefile, &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
};

TexturedObject createTexturedCube(glm::vec3 pos, float sz, const char* texturefile)
{
    TexturedObject myObject(texturefile);
    myObject.vertexCount = 36;
    std::vector<GLfloat> vertices = {
        pos.x + sz, pos.y + sz, pos.z + sz, 0, 0,
        pos.x + sz, pos.y + sz, pos.z - sz, 0, 1,
        pos.x + sz, pos.y - sz, pos.z + sz, 1, 0,
        pos.x + sz, pos.y + sz, pos.z - sz, 0, 1,
        pos.x + sz, pos.y - sz, pos.z + sz, 1, 0,
        pos.x + sz, pos.y - sz, pos.z - sz, 1, 1,

        pos.x - sz, pos.y + sz, pos.z + sz, 0, 0,
        pos.x - sz, pos.y + sz, pos.z - sz, 0, 1,
        pos.x + sz, pos.y + sz, pos.z + sz, 1, 0,
        pos.x - sz, pos.y + sz, pos.z - sz, 0, 1,
        pos.x + sz, pos.y + sz, pos.z + sz, 1, 0,
        pos.x + sz, pos.y + sz, pos.z - sz, 1, 1,

        pos.x + sz, pos.y - sz, pos.z + sz, 0, 0,
        pos.x - sz, pos.y - sz, pos.z + sz, 0, 1,
        pos.x + sz, pos.y + sz, pos.z + sz, 1, 0,
        pos.x - sz, pos.y - sz, pos.z + sz, 0, 1,
        pos.x + sz, pos.y + sz, pos.z + sz, 1, 0,
        pos.x - sz, pos.y + sz, pos.z + sz, 1, 1,

        pos.x - sz, pos.y - sz, pos.z + sz, 0, 0,
        pos.x - sz, pos.y - sz, pos.z - sz, 0, 1,
        pos.x - sz, pos.y + sz, pos.z + sz, 1, 0,
        pos.x - sz, pos.y - sz, pos.z - sz, 0, 1,
        pos.x - sz, pos.y + sz, pos.z + sz, 1, 0,
        pos.x - sz, pos.y + sz, pos.z - sz, 1, 1,

        pos.x + sz, pos.y - sz, pos.z + sz, 0, 0,
        pos.x + sz, pos.y - sz, pos.z - sz, 0, 1,
        pos.x - sz, pos.y - sz, pos.z + sz, 1, 0,
        pos.x + sz, pos.y - sz, pos.z - sz, 0, 1,
        pos.x - sz, pos.y - sz, pos.z + sz, 1, 0,
        pos.x - sz, pos.y - sz, pos.z - sz, 1, 1,

        pos.x - sz, pos.y - sz, pos.z - sz, 0, 0,
        pos.x + sz, pos.y - sz, pos.z - sz, 0, 1,
        pos.x - sz, pos.y + sz, pos.z - sz, 1, 0,
        pos.x + sz, pos.y - sz, pos.z - sz, 0, 1,
        pos.x - sz, pos.y + sz, pos.z - sz, 1, 0,
        pos.x + sz, pos.y + sz, pos.z - sz, 1, 1
    };
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
    return myObject;
}

#endif // CREATE_OBJECTS_HPP
