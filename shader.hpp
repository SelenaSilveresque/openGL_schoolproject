#define GLEW_STATIC

#include <GL/glew.h>
#include <stdio.h>
#include <fstream>
#include <iostream>

#ifndef SHADER_HPP
#define SHADER_HPP

const char* loadShader(const char *shaderSource)
{
    std::string content;
    std::ifstream fileStream(shaderSource, std::ios::in);

    if (!fileStream.is_open()) {
        std::cerr << "Could not read " << shaderSource << std::endl;
        return "";
    };

    while (!fileStream.eof()) {
        std::string line = "";
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content.c_str();
}

GLuint createDefaultShaderProgram()
{
    const GLchar* defaultVertexShaderSource = loadShader("vertexShader.shd");
    GLuint defaultVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(defaultVertexShader, 1, &defaultVertexShaderSource, NULL);
    glCompileShader(defaultVertexShader);

    const GLchar* defaultFragmentShaderSource = loadShader("fragmentShader.shd");
    GLuint defaultFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(defaultFragmentShader, 1, &defaultFragmentShaderSource, NULL);
    glCompileShader(defaultFragmentShader);

    char vertexShaderBuffer[512];
    glGetShaderInfoLog(defaultVertexShader, 512, NULL, vertexShaderBuffer);

    char fragmentShaderBuffer[512];
    glGetShaderInfoLog(defaultFragmentShader, 512, NULL, fragmentShaderBuffer);

    printf("Vertex shader: %sFragment shader: %s", vertexShaderBuffer, fragmentShaderBuffer);

    GLuint defaultShaderProgram = glCreateProgram();
    glAttachShader(defaultShaderProgram, defaultVertexShader);
    glAttachShader(defaultShaderProgram, defaultFragmentShader);
    glBindFragDataLocation(defaultShaderProgram, 0, "outColor");
    glLinkProgram(defaultShaderProgram);

    return defaultShaderProgram;
}

#endif // SHADER_HPP
