#ifndef BULLET_HPP
#define BULLET_HPP

#define GLEW_STATIC

#include "object.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <ctime>
#include <list>
#include <chrono>

struct BulletArray : public ObjectArray
{
    BulletArray(){}

    float bullet_size = 0.1;
    void shoot_bullet(glm::vec3 cameraFront)
    {
        SimpleObject new_bullet;
            new_bullet.update_size(glm::vec3(bullet_size));
            new_bullet.update_angle(glm::vec3(0, 1, 0), cameraFront);
            new_bullet.direction = cameraFront;
            new_bullet.speed = speed;
        copies.insert(copies.end(), new_bullet);
    }

    int left_edge = -10,
        right_edge = 10,
        screen_height = 8;
    float distance_to_screen = 10.5;
    bool bulletHit = false;
    void update_time(int timeElapsed, ObjectArray& targets, int& score, int& life)
    {
        ObjectArray::update_time(timeElapsed);


        std::list<SimpleObject>::iterator cur = copies.begin();
        while (cur != copies.end())
        {
            std::list<SimpleObject>::iterator target = targets.copies.begin();
            while (target != targets.copies.end())
            {
                if (cur->position.y + 1 > distance_to_screen && pow(cur->position.x - target->position.x, 2) + pow(cur->position.z - target->position.z, 2) < 1) {
                    target = targets.copies.erase(target);
                    ++score;
                    bulletHit = true;
                }
                else {
                    ++target;
                }
            }

            if (cur->position.x < left_edge
             || cur->position.x > right_edge
             || cur->position.y > distance_to_screen
             || cur->position.z < -screen_height / 2
             || cur->position.z >  screen_height / 2) {
                if (!bulletHit) life--;
                cur = copies.erase(cur);
                bulletHit = false;
             }
            else ++cur;
        }
    }
};

#endif // BULLET_HPP
