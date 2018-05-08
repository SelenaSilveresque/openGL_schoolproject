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
#include <string>

const int WinWidth  = 800;
const int WinHeight = 600;
const sf::Vector2i defaultMousePosition(WinWidth / 2, WinHeight / 2);

const float viewAngle = 45;
const float nearPlane = 1;
const float  farPlane = 100;
const float sensitivity = 0.5;

const float mouseSensitivity = 0.05;
glm::vec3 cameraPos  (0, 0, 0),
          cameraFront(0, 1, 0),
          cameraUp   (0, 0, 1);
float yaw = 0, pitch = 0;

glm::mat4 projection_matrix = glm::perspective(glm::radians(viewAngle), static_cast<float>(WinWidth) / WinHeight, nearPlane, farPlane);
glm::mat4   _2d_view_matrix = glm::lookAt(glm::vec3(0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
glm::mat4   _3d_view_matrix = glm::lookAt(glm::vec3(0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));

std::chrono::steady_clock::time_point last_frame, current_frame;

const int   targets_interval = 1300;
const float targets_speed    = 0.005;
const float bullets_speed    = 0.02;

bool program_began = false;
void start_new_game(TargetArray& targets, int& score, int& life, bool& moveMode)
{
    if (!program_began)
    {
        targets.update_buffer(createSquareBuffer());
        targets.update_texture("texture/defaultTargetTexture.png");
        program_began = true;
    }
    targets.copies.clear();
    targets.interval = targets_interval;
    targets.speed    = targets_speed;
    score = 0;
    life = 3;
    moveMode = false;
    cameraPos = glm::vec3(0, 0, 0);
}

int main()
{
    random_engine.seed(std::chrono::steady_clock::now().time_since_epoch().count());

    sf::RenderWindow window(sf::VideoMode(WinWidth, WinHeight), "SFML window", sf::Style::Default, sf::ContextSettings(24, 8));
    window.setMouseCursorGrabbed(true) ;
    window.setMouseCursorVisible(false);
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
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection_matrix));

    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);


    DefaultObject background;
        background.update_size(glm::vec3(10));
        background.update_position(glm::vec3(0, 11, 0));
        background.update_buffer(createSquareBuffer());
        background.update_texture("texture/backgroundTexture.png");
    DefaultObject foreground;
        foreground.update_size(glm::vec3(11));
        foreground.update_position(glm::vec3(0, 10, 0));
        foreground.update_buffer(createSquareBuffer());
        foreground.update_texture("texture/foregroundTexture.png");
    DefaultObject crosshair;
        crosshair.update_size(glm::vec3(0.25));
        crosshair.update_position(glm::vec3(0, 5, 0));
        crosshair.update_buffer(createSquareBuffer());
        crosshair.update_texture("texture/crosshair.png");
    DefaultObject ground;
        ground.update_size(glm::vec3(11));
        ground.update_angle(glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));
        ground.update_position(glm::vec3(0, 0, -8));
        ground.update_buffer(createSquareBuffer());
        ground.update_texture("texture/groundTexture.png");
    DefaultObject roof;
        roof.update_size(glm::vec3(11));
        roof.update_angle(glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));
        roof.update_position(glm::vec3(0, 0, 8));
        roof.update_buffer(createSquareBuffer());
        roof.update_texture("texture/roofTexture.png");
    DefaultObject leftWall;
        leftWall.update_size(glm::vec3(11));
        leftWall.update_angle(glm::vec3(0, 1, 0), glm::vec3(-1, 0, 0));
        leftWall.update_position(glm::vec3(-15, 4, 0));
        leftWall.update_buffer(createSquareBuffer());
        leftWall.update_texture("texture/leftWallTexture.png");
    DefaultObject rightWall;
        rightWall.update_size(glm::vec3(11));
        rightWall.update_angle(glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
        rightWall.update_position(glm::vec3(15, 4, 0));
        rightWall.update_buffer(createSquareBuffer());
        rightWall.update_texture("texture/rightWallTexture.png");
    DefaultObject scoreText;
        scoreText.update_size(glm::vec3(0.045));
        scoreText.update_position(glm::vec3(0.32, 1, 0.29));
        scoreText.update_buffer(createSquareBuffer());
        scoreText.update_texture("texture/scoreText.png");
    numberObject numText;
        numText.update_size(glm::vec3(0.045));
        numText.update_buffer(createNumberBuffer());
        numText.update_texture("texture/numbers.png");
    lifeObject lifeText;
        lifeText.update_size(glm::vec3(0.019));
        lifeText.update_buffer(createSquareBuffer());
        lifeText.update_texture("texture/lifeTexture.png");
    BulletArray bullets;
        bullets.update_buffer(createSquareBuffer());
        bullets.update_texture("texture/defaultBulletTexture.png");
        bullets.speed = bullets_speed;
    DefaultObject gameoverText;
        gameoverText.update_size(glm::vec3(0.5));
        gameoverText.update_position(glm::vec3(0, 1, 0));
        gameoverText.update_buffer(createSquareBuffer());
        gameoverText.update_texture("texture/gameoverTexture.png");

    last_frame = std::chrono::steady_clock::now();
    bool running = true;

    TargetArray targets;
    int score = 0,
        endPressedTime = 0,
        life = 3;
    bool moveMode = false;
    start_new_game(targets, score, life, moveMode);

    while (running)
    {
        current_frame = std::chrono::steady_clock::now();
        if (current_frame - last_frame >= std::chrono::milliseconds(10))
        {
            int timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_frame - last_frame).count();
            if (life > 0) {
                targets.update_time(timeElapsed, life);
                bullets.update_time(timeElapsed, targets, score);
            }

            last_frame = current_frame;

            sf::Event event;
            bool checkMouseMove = false;

            while (window.pollEvent(event))
                switch (event.type) {
                case sf::Event::Closed:
                    running = false;
                    break;
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                    case sf::Keyboard::Escape:
                        running = false;
                        break;
                    case sf::Keyboard::F2:
                        start_new_game(targets, score, life, moveMode);
                        break;
                    case sf::Keyboard::W:
                        break;
                    case sf::Keyboard::A:
                        break;
                    case sf::Keyboard::S:
                        break;
                    case sf::Keyboard::D:
                        break;
                    case sf::Keyboard::Space:
                        if (moveMode && life > 0) cameraPos.z += sensitivity;
                        break;
                    case sf::Keyboard::LShift:
                        if (moveMode && life > 0) cameraPos.z -= sensitivity;
                        break;
                    case sf::Keyboard::End:
                        endPressedTime++;
                        break;
                    }
                    break;
                case sf::Event::MouseMoved:
                    if (life > 0) checkMouseMove = true;
                    break;
                case sf::Event::MouseButtonPressed:
                    if (life > 0)
                        switch (event.mouseButton.button)
                        {
                        case sf::Mouse::Left:
                            bullets.shoot_bullet(cameraFront);
                            break;
                        default:
                            break;
                        }
                    break;
                }

            if (endPressedTime == 5) {
                endPressedTime = 0;
                if (moveMode) cameraPos = glm::vec3(0, 0, 0);
                moveMode = !moveMode;
            }

            if (checkMouseMove) {
                sf::Vector2i currentMousePosition = sf::Mouse::getPosition(window);
                yaw   += (currentMousePosition.x - defaultMousePosition.x) * mouseSensitivity;
                pitch += (defaultMousePosition.y - currentMousePosition.y) * mouseSensitivity;
                sf::Mouse::setPosition(defaultMousePosition, window);

                if (!moveMode) {
                    if (yaw >  45)
                        yaw =  45;
                    if (yaw < -45)
                        yaw = -45;
                }

                if (pitch >  89)
                    pitch =  89;
                if (pitch < -89)
                    pitch = -89;

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
            foreground.draw(posAttrib, texAttrib, uniModel);
            bullets   .draw(posAttrib, texAttrib, uniModel);
            ground    .draw(posAttrib, texAttrib, uniModel);
            roof      .draw(posAttrib, texAttrib, uniModel);
            leftWall  .draw(posAttrib, texAttrib, uniModel);
            rightWall .draw(posAttrib, texAttrib, uniModel);

            glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(_2d_view_matrix));
            crosshair .draw(posAttrib, texAttrib, uniModel);
            scoreText .draw(posAttrib, texAttrib, uniModel);
            numText   .draw(posAttrib, texAttrib, uniModel, score);
            lifeText  .draw(posAttrib, texAttrib, uniModel, life);
            if (life == 0) gameoverText.draw(posAttrib, texAttrib, uniModel);

            window.display();
        }
    }

    glDeleteVertexArrays(1, &vertexArray);

    printf("%s\n", gluErrorString(glGetError()));

    window.close();
    return 0;
}
