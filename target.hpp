#ifndef TARGET_HPP
#define TARGET_HPP

#define GLEW_STATIC

#include "object.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <ctime>
#include <list>
#include <random>

std::mt19937 random_engine(time(nullptr));

struct TargetArray
{
    TexturedObject object;
    std::list<glm::vec3> copies;
    std::chrono::milliseconds timeToNextTarget;

    TargetArray()
    {
        timeToNextTarget = std::chrono::milliseconds(random_engine() % 4000 + 1000);
    }

    void update(std::chrono::milliseconds timeElapsed)
    {
        timeToNextTarget -= timeElapsed;
        if (timeToNextTarget < std::chrono::steady_clock::duration::zero())
        {
            timeToNextTarget = std::chrono::milliseconds(random_engine() % 500 + 500);
            copies.insert(copies.end(), glm::vec3(-7, 10, 0));
        }

        std::list<glm::vec3>::iterator cur = copies.begin();
        while (cur != copies.end())
        {
            *cur += glm::vec3(0.02, 0, 0) * (float) timeElapsed.count();
            if (cur->x > 7)
                cur = copies.erase(cur);
            else ++cur;
        }
    }

    void draw(GLint posAttrib, GLint texAttrib, GLint uniModel)
    {
        glBindBuffer(GL_ARRAY_BUFFER, object.vertexBuffer);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), 0);
        glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
        glBindTexture(GL_TEXTURE_2D, object.texture);

        std::list<glm::vec3>::iterator cur = copies.begin();
        while (cur != copies.end())
        {
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(glm::translate(object.model, *cur)));
            glDrawArrays(GL_TRIANGLES, 0, object.vertexCount);
            ++cur;
        }
    }
};

#endif // TARGET_HPP
