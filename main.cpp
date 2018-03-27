#define GLEW_STATIC

#include "shader.hpp"
#include "object.hpp"
#include "target.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <chrono>

const int WinWidth  = 800;
const int WinHeight = 600;

const float viewAngle = 45;
const float nearPlane = 0.1;
const float  farPlane = 100;

const float mouseSensitivity = 0.02;
glm::vec3 cameraPos  (0, 0, 0),
          cameraFront(0, 1, 0),
          cameraUp   (0, 0, 1);
float yaw = 0, pitch = 0;

std::chrono::steady_clock::time_point last_frame, current_frame;

int main()
{
    random_engine.seed(std::chrono::steady_clock::now().time_since_epoch().count());

    sf::RenderWindow window(sf::VideoMode(WinWidth, WinHeight), "SFML window", sf::Style::Default, sf::ContextSettings(24, 8));
    window.setMouseCursorGrabbed(true) ;
    window.setMouseCursorVisible(false);
    sf::Vector2i defaultMousePosition(window.getSize().x / 2, window.getSize().y / 2);
    sf::Mouse::setPosition(defaultMousePosition, window);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint shaderProgram = createDefaultShaderProgram();
    glUseProgram(shaderProgram);
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    GLint uniView  = glGetUniformLocation(shaderProgram, "view" );
    GLint uniProj  = glGetUniformLocation(shaderProgram, "proj" );

    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);

    TargetArray targets;
    targets.object = createTexturedSquare(glm::vec3(0), 1, "defaultTargetTexture.png");

    TexturedObject background = createTexturedSquare(glm::vec3(0, 11, 0), 10, "backgroundTexture.png");
    TexturedObject foreground = createTexturedSquare(glm::vec3(0, 5.5, 0), 6, "foregroundTexture.png");
    TexturedObject crosshair = createTexturedSquare(glm::vec3(0, 0.1, 0), 0.1, "crosshair.png");

    bool running = true;
    last_frame = std::chrono::steady_clock::now();

    while (running)
    {
        current_frame = std::chrono::steady_clock::now();
        std::chrono::milliseconds timeSinceLastFrame = std::chrono::duration_cast<std::chrono::milliseconds>(current_frame - last_frame);
        last_frame = current_frame;

        if (timeSinceLastFrame.count() > 0)
            targets.update(timeSinceLastFrame);

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
            case sf::Event::MouseMoved:
                checkMouseMove = true;
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
        }

        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(glm::perspective(glm::radians(viewAngle), (float)window.getSize().x / window.getSize().y, nearPlane, farPlane)));
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)));

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        background.draw(posAttrib, texAttrib, uniModel);
        targets   .draw(posAttrib, texAttrib, uniModel);
        foreground.draw(posAttrib, texAttrib, uniModel);

        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(glm::perspective((float)glm::radians(45.0), (float)window.getSize().x / window.getSize().y, 1.0f, 10.0f)));
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(glm::lookAt(glm::vec3(0, -2, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 2))));

        crosshair.draw(posAttrib, texAttrib, uniModel);

        window.display();
    }

    glDeleteVertexArrays(1, &vertexArray);

    printf("%s\n", gluErrorString(glGetError()));

    window.close();
    return 0;
}
