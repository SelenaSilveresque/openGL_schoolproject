#ifndef CREATE_OBJECTS_HPP
#define CREATE_OBJECTS_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENALBLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>
#include <SOIL.h>

#include <list>
#include <vector>

struct SimpleObject
{
    glm::mat4 trans = glm::mat4(1),
              rot   = glm::mat4(1),
              scale = glm::mat4(1);

    glm::vec3 position = glm::vec3(0);
    void update_position(const glm::vec3 &t)
    {
        trans = glm::translate(trans, t);
        position += t;
    }

    void update_angle(const glm::vec3 &a, const glm::vec3 &b)
    {
        rot = glm::rotate(rot, glm::orientedAngle(a, b, glm::cross(a, b)), glm::cross(a, b));
    }

    void update_size(const glm::vec3 &s)
    {
        scale = glm::scale(scale, s);
    }

    glm::vec3 direction;
    float speed;
    void update_time(int timeElapsed)
    {
        update_position(direction * speed * static_cast<float>(timeElapsed));
    }
};

struct DefaultObject : public SimpleObject
{
    GLuint vertexBuffer;
    int vertexCount;
    void update_buffer(std::vector<GLfloat> vertices)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        vertexCount = vertices.size() / 5;
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
    }

    GLuint texture;
    void update_texture(const char* texturefile)
    {
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
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(trans * rot * scale));
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

    DefaultObject() {
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
};

struct ObjectArray : public DefaultObject
{
    std::list<SimpleObject> copies;
    void update_time(int timeElapsed)
    {
        for (SimpleObject& copy : copies)
        {
            copy.update_time(timeElapsed);
        }
    }

    void draw(GLint posAttrib, GLint texAttrib, GLint uniModel)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), 0);
            glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
        glBindTexture(GL_TEXTURE_2D, texture);

        for (SimpleObject& copy : copies)
        {
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(copy.trans * copy.rot * copy.scale));
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        }
    }
};

struct numberObject : public DefaultObject
{
    void draw(GLint posAttrib, GLint texAttrib, GLint uniModel, int score)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), 0);
            glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
        glBindTexture(GL_TEXTURE_2D, texture);

        std::vector<int> number;
        while (score > 0) {
            number.push_back(score % 10);
            score /= 10;
        }

        position = glm::vec3(8.5, 25, 9);
        trans = glm::translate(glm::vec3(8.5, 25, 9));
        for (int i = number.size() - 1; i >= 0; i--) {
            update_position(glm::vec3(2, 0 ,0));
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(trans * rot * scale));
            glDrawArrays(GL_TRIANGLES, number[i] * 6, 6);
        }
    }
};

std::vector<GLfloat> createSquareBuffer() /** perpendicular to y-axis **/
{
    std::vector<GLfloat> result = {
        -1, 0,  1, 0, 0,
        -1, 0, -1, 0, 1,
         1, 0,  1, 1, 0,
        -1, 0, -1, 0, 1,
         1, 0,  1, 1, 0,
         1, 0, -1, 1, 1
    };
    return result;
}

std::vector<GLfloat> createNumberBuffer()
{
    std::vector<GLfloat> result;

    for (float j = 0; j < 2; j++) {
        float top = j / 2;
        for (int i = 0; i < 5; i++) {
            std::vector<GLfloat> numCoor = {
                -1, 0,  1, 0.2 * i,       top,
                 1, 0,  1, 0.2 * (i + 1), top,
                -1, 0, -1, 0.2 * i,       top + 0.5,
                 1, 0,  1, 0.2 * (i + 1), top,
                 1, 0, -1, 0.2 * (i + 1), top + 0.5,
                -1, 0, -1, 0.2 * i,       top + 0.5
            };
            result.insert(result.end(), numCoor.begin(), numCoor.end());
        }
    }
    return result;
}

std::vector<GLfloat> createCubeBuffer()
{
    std::vector<GLfloat> result = {
         1,  1,  1, 1, 0,
         1,  1, -1, 1, 1,
         1, -1,  1, 0, 0,
         1,  1, -1, 1, 1,
         1, -1,  1, 0, 0,
         1, -1, -1, 0, 1,

        -1,  1,  1, 1, 0,
        -1,  1, -1, 1, 1,
         1,  1,  1, 0, 0,
        -1,  1, -1, 1, 1,
         1,  1,  1, 0, 0,
         1,  1, -1, 0, 1,

         1, -1,  1, 1, 0,
        -1, -1,  1, 1, 1,
         1,  1,  1, 0, 0,
        -1, -1,  1, 1, 1,
         1,  1,  1, 0, 0,
        -1,  1,  1, 0, 1,

        -1, -1,  1, 1, 0,
        -1, -1, -1, 1, 1,
        -1,  1,  1, 0, 0,
        -1, -1, -1, 1, 1,
        -1,  1,  1, 0, 0,
        -1,  1, -1, 0, 1,

         1, -1,  1, 1, 0,
         1, -1, -1, 1, 1,
        -1, -1,  1, 0, 0,
         1, -1, -1, 1, 1,
        -1, -1,  1, 0, 0,
        -1, -1, -1, 0, 1,

        -1, -1, -1, 1, 0,
         1, -1, -1, 1, 1,
        -1,  1, -1, 0, 0,
         1, -1, -1, 1, 1,
        -1,  1, -1, 0, 0,
         1,  1, -1, 0, 1
    };
    return result;
}

#endif // CREATE_OBJECTS_HPP
