#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SFML/Window.hpp>

#ifndef CAMERA_HPP
#define CAMERA_HPP

struct Camera
{
    static glm::vec3 defaultPos, defaultFront, defaultUp;
    glm::vec3 Pos, Front, Up;
    static float speed, mouseSensitivity;
    float yaw = 0, pitch = 0;

    Camera(glm::vec3 pos = defaultPos, glm::vec3 front = defaultFront, glm::vec3 up = defaultUp)
    {
        Pos   = pos;
        Front = front;
        Up    = up;
    }

    void rotate(sf::Vector2i pos1, sf::Vector2i pos2)
    {
        yaw   += (pos2.x - pos1.x) * mouseSensitivity;
        pitch += (pos1.y - pos2.y) * mouseSensitivity;

        if (pitch >  89)
            pitch =  89;
        if (pitch < -89)
            pitch = -89;

        Front.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        Front.y = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        Front.z = sin(glm::radians(pitch));
    }

    void move()
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            Pos += glm::normalize(glm::cross(glm::cross(Up, Front), Up)) * speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            Pos -= glm::normalize(glm::cross(glm::cross(Up, Front), Up)) * speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            Pos += glm::normalize(glm::cross(Up, Front)) * speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            Pos -= glm::normalize(glm::cross(Up, Front)) * speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            Pos += Up * speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            Pos -= Up * speed;
    }

    glm::mat4 display()
    {
        return glm::lookAt(Pos, Pos + Front, Up);
    }
};

#endif // CAMERA_HPP
