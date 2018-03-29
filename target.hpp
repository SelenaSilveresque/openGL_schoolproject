#ifndef TARGET_HPP
#define TARGET_HPP

#define GLEW_STATIC

#include "object.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ctime>
#include <list>
#include <random>

std::mt19937 random_engine(time(nullptr));

struct TargetArray : public ObjectArray
{
    int timeToNextTarget, timeSinceCreation = 0;
    int interval = 500;
    TargetArray()
    {
        update_buffer(createSquareBuffer());
        update_texture("defaultTargetTexture.png");
        timeToNextTarget = random_engine() % interval + interval;
    }

    int left_edge = -10,
        right_edge = 10,
        distance_to_screen = 10,
        screen_height = 8;
    float target_size = 1;
    void update_time(int timeElapsed)
    {
        speed += timeElapsed * 5e-8;

        timeSinceCreation += timeElapsed;
        interval = 500 - timeSinceCreation / 1000;

        timeToNextTarget -= timeElapsed;
        if (timeToNextTarget <= 0)
        {
            SimpleObject new_target;
                new_target.update_size(glm::vec3(target_size));
                new_target.update_position(glm::vec3(left_edge, distance_to_screen, (float) (random_engine() % (screen_height * 1000)) / 1000 - screen_height / 2));
                new_target.direction = glm::vec3(1, 0, 0);
                new_target.speed = speed;
            copies.insert(copies.end(), new_target);
            timeToNextTarget = random_engine() % interval + interval;
        }

        ObjectArray::update_time(timeElapsed);

        std::list<SimpleObject>::iterator cur = copies.begin();
        while (cur != copies.end())
        {
            if (cur->position.x > right_edge)
                cur = copies.erase(cur);
            else ++cur;
        }
    }
};

#endif // TARGET_HPP
