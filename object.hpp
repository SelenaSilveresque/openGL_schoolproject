#ifndef CREATE_OBJECTS_HPP
#define CREATE_OBJECTS_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>

#include <vector>

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
        unsigned char* image = SOIL_load_image(texturefile, &width, &height, 0, SOIL_LOAD_RGBA);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    void draw(GLint posAttrib, GLint texAttrib, GLint uniModel)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), 0);
        glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
};

TexturedObject createTexturedSquare(glm::vec3 pos, float sz, const char* texturefile)
{
    TexturedObject myObject(texturefile);
    myObject.vertexCount = 6;
    std::vector<GLfloat> vertices = {
        pos.x - sz, pos.y, pos.z + sz, 0, 0,
        pos.x - sz, pos.y, pos.z - sz, 0, 1,
        pos.x + sz, pos.y, pos.z + sz, 1, 0,
        pos.x - sz, pos.y, pos.z - sz, 0, 1,
        pos.x + sz, pos.y, pos.z + sz, 1, 0,
        pos.x + sz, pos.y, pos.z - sz, 1, 1
    };
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
    return myObject;
}

TexturedObject createTexturedCube(glm::vec3 pos, float sz, const char* texturefile)
{
    TexturedObject myObject(texturefile);
    myObject.vertexCount = 36;
    std::vector<GLfloat> vertices = {
        pos.x + sz, pos.y + sz, pos.z + sz, 1, 0,
        pos.x + sz, pos.y + sz, pos.z - sz, 1, 1,
        pos.x + sz, pos.y - sz, pos.z + sz, 0, 0,
        pos.x + sz, pos.y + sz, pos.z - sz, 1, 1,
        pos.x + sz, pos.y - sz, pos.z + sz, 0, 0,
        pos.x + sz, pos.y - sz, pos.z - sz, 0, 1,

        pos.x - sz, pos.y + sz, pos.z + sz, 1, 0,
        pos.x - sz, pos.y + sz, pos.z - sz, 1, 1,
        pos.x + sz, pos.y + sz, pos.z + sz, 0, 0,
        pos.x - sz, pos.y + sz, pos.z - sz, 1, 1,
        pos.x + sz, pos.y + sz, pos.z + sz, 0, 0,
        pos.x + sz, pos.y + sz, pos.z - sz, 0, 1,

        pos.x + sz, pos.y - sz, pos.z + sz, 1, 0,
        pos.x - sz, pos.y - sz, pos.z + sz, 1, 1,
        pos.x + sz, pos.y + sz, pos.z + sz, 0, 0,
        pos.x - sz, pos.y - sz, pos.z + sz, 1, 1,
        pos.x + sz, pos.y + sz, pos.z + sz, 0, 0,
        pos.x - sz, pos.y + sz, pos.z + sz, 0, 1,

        pos.x - sz, pos.y - sz, pos.z + sz, 1, 0,
        pos.x - sz, pos.y - sz, pos.z - sz, 1, 1,
        pos.x - sz, pos.y + sz, pos.z + sz, 0, 0,
        pos.x - sz, pos.y - sz, pos.z - sz, 1, 1,
        pos.x - sz, pos.y + sz, pos.z + sz, 0, 0,
        pos.x - sz, pos.y + sz, pos.z - sz, 0, 1,

        pos.x + sz, pos.y - sz, pos.z + sz, 1, 0,
        pos.x + sz, pos.y - sz, pos.z - sz, 1, 1,
        pos.x - sz, pos.y - sz, pos.z + sz, 0, 0,
        pos.x + sz, pos.y - sz, pos.z - sz, 1, 1,
        pos.x - sz, pos.y - sz, pos.z + sz, 0, 0,
        pos.x - sz, pos.y - sz, pos.z - sz, 0, 1,

        pos.x - sz, pos.y - sz, pos.z - sz, 1, 0,
        pos.x + sz, pos.y - sz, pos.z - sz, 1, 1,
        pos.x - sz, pos.y + sz, pos.z - sz, 0, 0,
        pos.x + sz, pos.y - sz, pos.z - sz, 1, 1,
        pos.x - sz, pos.y + sz, pos.z - sz, 0, 0,
        pos.x + sz, pos.y + sz, pos.z - sz, 0, 1
    };
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
    return myObject;
}

#endif // CREATE_OBJECTS_HPP