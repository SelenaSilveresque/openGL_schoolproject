#define GLEW_STATIC

#include <GL/glew.h>

#ifndef SHADER_HPP
#define SHADER_HPP

GLuint createDefaultShaderProgram()
{
    const GLchar* defaultVertexShaderSource = R"glsl(
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
    GLuint defaultVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(defaultVertexShader, 1, &defaultVertexShaderSource, NULL);
    glCompileShader(defaultVertexShader);

    const GLchar* defaultFragmentShaderSource = R"glsl(
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
    GLuint defaultFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(defaultFragmentShader, 1, &defaultFragmentShaderSource, NULL);
    glCompileShader(defaultFragmentShader);

    GLuint defaultShaderProgram = glCreateProgram();
    glAttachShader(defaultShaderProgram, defaultVertexShader);
    glAttachShader(defaultShaderProgram, defaultFragmentShader);
    glLinkProgram(defaultShaderProgram);

    return defaultShaderProgram;
}

#endif // SHADER_HPP
