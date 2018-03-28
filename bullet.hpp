#ifndef BULLET_HPP
#define BULLET_HPP

#define GLEW_STATIC

#include "object.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ctime>
#include <list>

struct BulletArray : public ObjectArray
{
    BulletArray()
    {
        speed = 0.1;
        update_buffer(createSquareBuffer());
        update_texture("defaultBulletTexture.png");
    }

    const float bullet_size = 0.1;
    void shoot_bullet(glm::vec3 cameraFront)
    {
        SimpleObject new_bullet;
            new_bullet.update_size(glm::vec3(bullet_size));
            new_bullet.update_angle(glm::vec3(0, 1, 0), cameraFront);
            new_bullet.direction = cameraFront;
            new_bullet.speed = speed;
        copies.insert(copies.end(), new_bullet);
    }

    const int left_edge = -10,
              right_edge = 10,
              distance_to_screen = 10,
              screen_height = 8;
    void update_time(int timeElapsed)
    {
        ObjectArray::update_time(timeElapsed);

        std::list<SimpleObject>::iterator cur = copies.begin();
        while (cur != copies.end())
        {
            if (cur->position.x <  left_edge
             || cur->position.x > right_edge
             || cur->position.y > distance_to_screen
             || cur->position.z < -screen_height / 2
             || cur->position.z >  screen_height / 2)
                cur = copies.erase(cur);
            else ++cur;
        }
    }
};

#endif // BULLET_HPP
