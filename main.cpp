#define GLEW_STATIC

#include "shader.hpp"
#include "object.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

const int defaultWindowWidth  = 800;
const int defaultWindowHeight = 600;

const float viewAngle = 45;
const float nearPlane = 0.1;
const float  farPlane = 100;

const float mouseSensitivity = 0.05 ;
glm::vec3 cameraPos  (0, 0, 0),
          cameraFront(0, 1, 0),
          cameraUp   (0, 0, 1);
float yaw = 0, pitch = 0;

int main()
{
    sf::RenderWindow window(sf::VideoMode(defaultWindowWidth, defaultWindowHeight), "SFML window", sf::Style::Default, sf::ContextSettings(24, 8));
    window.setMouseCursorGrabbed(true) ;
    window.setMouseCursorVisible(false);
    sf::Vector2i defaultMousePosition(window.getSize().x / 2, window.getSize().y / 2);
    sf::Mouse::setPosition(defaultMousePosition, window);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);

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

    TexturedObject object[4];
    object[0] = createTexturedCube(glm::vec3(3, 3, 0), 1, "cube0.png");
    object[1] = createTexturedCube(glm::vec3(3, 7, 0), 1, "cube1.png");
    object[2] = createTexturedCube(glm::vec3(7, 3, 0), 1, "cube2.png");
    object[3] = createTexturedCube(glm::vec3(7, 7, 0), 1, "cube3.png");

    printf("%d\n", glGetError());

    bool running = true;
    while (running)
    {
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
            
            if (pitch >  89)
                pitch =  89;
            if (pitch < -89)
                pitch = -89;

            cameraFront.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront.y = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront.z = sin(glm::radians(pitch));
        }

        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(glm::perspective(glm::radians(viewAngle), (float)window.getSize().x / window.getSize().y, nearPlane, farPlane)));
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)));

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < 4; ++i)
        {
            glBindBuffer(GL_ARRAY_BUFFER, object[i].vertexBuffer);
            glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
            glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
            glBindTexture(GL_TEXTURE_2D, object[i].texture);
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(object[i].model));
            glDrawArrays(GL_TRIANGLES, 0, object[i].vertexCount);
        }

        window.display();
    }

    glDeleteVertexArrays(1, &vertexArray);

    window.close();
    return 0;
}
