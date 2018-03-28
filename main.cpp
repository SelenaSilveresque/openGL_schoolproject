#define GLEW_STATIC

#include "shader.hpp"
#include "object.hpp"
#include "target.hpp"
#include "bullet.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <chrono>
#include <iostream>

const int WinWidth  = 800;
const int WinHeight = 600;
const sf::Vector2i defaultMousePosition(WinWidth / 2, WinHeight / 2);

const float viewAngle = 45;
const float nearPlane = 0.1;
const float  farPlane = 100;

const float mouseSensitivity = 0.05;
glm::vec3 cameraPos  (0, 0, 0),
          cameraFront(0, 1, 0),
          cameraUp   (0, 0, 1);
float yaw = 0, pitch = 0;

glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.5f, 20.0f);
glm::mat4   _2d_view_matrix = glm::lookAt(glm::vec3(0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
glm::mat4   _3d_view_matrix = glm::lookAt(glm::vec3(0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));

std::chrono::steady_clock::time_point last_frame, current_frame;

int main()
{
    random_engine.seed(std::chrono::steady_clock::now().time_since_epoch().count());

    sf::RenderWindow window(sf::VideoMode(WinWidth, WinHeight), "SFML window", sf::Style::Default, sf::ContextSettings(24, 8));
    window.setMouseCursorGrabbed(true) ;
    window.setMouseCursorVisible(false);
    sf::Mouse::setPosition(defaultMousePosition, window);

    glewExperimental = GL_TRUE;
    glewInit();

    // glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint shaderProgram = createDefaultShaderProgram();
    glUseProgram(shaderProgram);
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    GLint uniView  = glGetUniformLocation(shaderProgram, "view" );
    GLint uniProj  = glGetUniformLocation(shaderProgram, "proj" );
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection_matrix));

    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);

    TargetArray targets;
    BulletArray bullets;

    DefaultObject background;
        background.update_size(glm::vec3(10));
        background.update_position(glm::vec3(0, 11, 0));
        background.update_buffer(createSquareBuffer());
        background.update_texture("backgroundTexture.png");
    DefaultObject foreground;
        foreground.update_size(glm::vec3(12));
        foreground.update_position(glm::vec3(0, 11, 0));
        foreground.update_buffer(createSquareBuffer());
        foreground.update_texture("foregroundTexture.png");
    DefaultObject crosshair;
        crosshair.update_size(glm::vec3(0.5));
        crosshair.update_position(glm::vec3(0, 10, 0));
        crosshair.update_buffer(createSquareBuffer());
        crosshair.update_texture("crosshair.png");

    bool running = true;
    last_frame = std::chrono::steady_clock::now();

    while (running)
    {
        current_frame = std::chrono::steady_clock::now();
        if (current_frame - last_frame >= std::chrono::milliseconds(10))
        {
            int timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_frame - last_frame).count();
            targets.update_time(timeElapsed);
            bullets.update_time(timeElapsed);
            last_frame = current_frame;

            sf::Event event;
            bool checkMouseMove = false;

            while (window.pollEvent(event))
                switch (event.type)
                {
                case sf::Event::Closed:
                    running = false;
                    break;
                case sf::Event::KeyPressed:
                    switch (event.key.code)
                    {
                    case sf::Keyboard::Escape:
                        running = false;
                        break;
                    default:
                        break;
                    }
                    break;
                case sf::Event::MouseMoved:
                    checkMouseMove = true;
                    break;
                case sf::Event::MouseButtonPressed:
                    switch (event.mouseButton.button)
                    {
                    case sf::Mouse::Left:
                        bullets.shoot_bullet(cameraFront);
                    default:
                        break;
                    }
                    break;
                default:
                    break;
                }

            if (checkMouseMove) {
                sf::Vector2i currentMousePosition = sf::Mouse::getPosition(window);
                yaw   += (currentMousePosition.x - defaultMousePosition.x) * mouseSensitivity;
                pitch += (defaultMousePosition.y - currentMousePosition.y) * mouseSensitivity;
                sf::Mouse::setPosition(defaultMousePosition, window);

                if (yaw >  45)
                    yaw =  45;
                if (yaw < -45)
                    yaw = -45;

                if (pitch >  25)
                    pitch =  25;
                if (pitch < -25)
                    pitch = -25;

                cameraFront.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront.y = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront.z = sin(glm::radians(pitch));

                _3d_view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            }

            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(_3d_view_matrix));
            background.draw(posAttrib, texAttrib, uniModel);
            targets   .draw(posAttrib, texAttrib, uniModel);
            bullets   .draw(posAttrib, texAttrib, uniModel);
            foreground.draw(posAttrib, texAttrib, uniModel);

            glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(_2d_view_matrix));
            crosshair .draw(posAttrib, texAttrib, uniModel);

            window.display();
        }
    }

    glDeleteVertexArrays(1, &vertexArray);

    window.close();
    return 0;
}
