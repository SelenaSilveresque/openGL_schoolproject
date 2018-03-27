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
    std::chrono::milliseconds timeToNextTarget, timeSinceCreation;
    float speed = 0.005;
    int interval = 500;

    TargetArray()
    {
        timeToNextTarget = std::chrono::milliseconds(random_engine() % interval + interval);
        timeSinceCreation = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::duration::zero());
    }

    void update(std::chrono::milliseconds timeElapsed)
    {
        timeToNextTarget -= timeElapsed;
        timeSinceCreation += timeElapsed;
        speed = 0.005 + timeSinceCreation.count() * 5e-7;
        interval = 500 - std::chrono::duration_cast<std::chrono::seconds>(timeSinceCreation).count();
        if (timeToNextTarget < std::chrono::steady_clock::duration::zero())
        {
            timeToNextTarget = std::chrono::milliseconds(random_engine() % interval + interval);
            copies.insert(copies.end(), glm::vec3(-10, 10, (float) (random_engine() % 6000) / 1000 - 3));
        }

        std::list<glm::vec3>::iterator cur = copies.begin();
        while (cur != copies.end())
        {
            *cur += glm::vec3(speed, 0, 0) * (float) timeElapsed.count();
            if (cur->x > 10)
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
