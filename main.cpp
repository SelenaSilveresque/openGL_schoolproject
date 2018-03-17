#define GLEW_STATIC

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <iostream>

void createShaderProgram(GLuint &vertexShader, GLuint &fragmentShader, GLuint &shaderProgram)
{
    const GLchar* vertexSource = R"glsl(
        #version 150 core
        in vec3 position;
        in vec2 texcoord;
        out vec4 Color;
        out vec2 Texcoord;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 proj;
        void main()
        {
            Color = vec4(1, 1, 1, 1);
            Texcoord = texcoord;
            gl_Position = proj * view * model * vec4(position, 1);
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(
        #version 150 core
        in vec4 Color;
        in vec2 Texcoord;
        out vec4 outColor;
        uniform sampler2D tex;
        void main()
        {
            outColor = Color * texture(tex, Texcoord);
        }
    )glsl";

    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    /** for debugging

    GLint status0, status1;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status0);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status1);
    if (status0 == GL_TRUE && status1 == GL_TRUE)
        std::cout << "shader compiled" << std::endl;

    // **/

    glBindFragDataLocation(shaderProgram, 0, "outColor");

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
}

GLuint loadTexture(const char* file)
{
    GLuint result;
    glGenTextures(1, &result);

    int width, height;
    unsigned char* image = SOIL_load_image(file, &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return result;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window", sf::Style::Default, sf::ContextSettings(24, 8));
    window.setMouseCursorGrabbed(true);
    window.setMouseCursorVisible(false);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    GLfloat vertices[] = {
        -1,  3,  1, 0.0, 0.0,
        -1,  3, -1, 0.0, 1.0,
         1,  3,  0, 1.0, 0.5,
         3,  1,  1, 0.0, 0.0,
         3,  1, -1, 0.0, 1.0,
         3, -1,  0, 1.0, 0.5,
         1, -3,  1, 0.0, 0.0,
         1, -3, -1, 0.0, 1.0,
        -1, -3,  0, 1.0, 0.5,
        -3, -1,  1, 0.0, 0.0,
        -3, -1, -1, 0.0, 1.0,
        -3,  1,  0, 1.0, 0.5
    };
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint   vertexShader  = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader  = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint   shaderProgram = glCreateProgram();
    createShaderProgram(vertexShader, fragmentShader, shaderProgram);

    GLuint texture = loadTexture("example.png");
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex"), texture);

    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    GLint uniView  = glGetUniformLocation(shaderProgram, "view");
    GLint uniProj  = glGetUniformLocation(shaderProgram, "proj");

    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    glm::vec3 cameraPos  (0, 0, 0);
    glm::vec3 cameraFront(0, 1, 0);
    glm::vec3 cameraUp   (0, 0, 1);
    float yaw = 0, pitch = 0;

    const int mouseX = 400, mouseY = 300;
    sf::Mouse::setPosition(sf::Vector2i(mouseX, mouseY), window);
    const float sensitivity = 0.05;

    bool running = true;
    while (running)
    {
        sf::Event event;
        while (window.pollEvent(event))
            switch (event.type)
            {
            case sf::Event::Closed:
                running = false;
                break;
            case sf::Event::MouseMoved:
                yaw   += (event.mouseMove.x - mouseX) * sensitivity;
                pitch += (mouseY - event.mouseMove.y) * sensitivity;
                sf::Mouse::setPosition(sf::Vector2i(mouseX, mouseY), window);

                if (pitch >  89)
                    pitch =  89;
                if (pitch < -89)
                    pitch = -89;

                cameraFront.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront.y = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront.z = sin(glm::radians(pitch));
                break; // **/
            }

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

        glDrawArrays(GL_TRIANGLES, 0, 12);
        window.display();
    }

    glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    window.close();

    return 0;
}
