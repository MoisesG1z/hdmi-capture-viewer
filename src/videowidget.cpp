#include "videowidget.h"

#include <QOpenGLShader>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <iostream>

// Shader para convertir YUYV a RGB
static const char *vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec3 position;
    layout(location = 1) in vec2 texCoord;
    
    out vec2 TexCoord;
    
    void main()
    {
        gl_Position = vec4(position, 1.0);
        TexCoord = texCoord;
    }
)";

static const char *fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;
    
    uniform sampler2D texture1;
    
    void main()
    {
        vec4 color = texture(texture1, TexCoord);
        
        // Conversión YUYV a RGB
        float y = color.r;
        float u = color.g;
        float v = color.b;
        
        float r = y + 1.402 * (v - 0.5);
        float g = y - 0.344136 * (u - 0.5) - 0.714136 * (v - 0.5);
        float b = y + 1.772 * (u - 0.5);
        
        FragColor = vec4(r, g, b, 1.0);
    }
)";

VideoWidget::VideoWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

VideoWidget::~VideoWidget()
{
    makeCurrent();
    if (textureId) {
        glDeleteTextures(1, &textureId);
    }
    if (vao) {
        glDeleteVertexArrays(1, &vao);
    }
    if (vbo) {
        glDeleteBuffers(1, &vbo);
    }
    doneCurrent();
}

void VideoWidget::updateFrame(int offset, int bytesUsed)
{
    // Actualizar textura con los nuevos datos
    update();
}

void VideoWidget::initializeGL()
{
    initializeOpenGLFunctions();
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    
    setupShaders();
    
    // Crear VAO y VBO
    float vertices[] = {
        -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 0.0f,
    };
    
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // TexCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Crear textura
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void VideoWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void VideoWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (shaderProgram) {
        shaderProgram->bind();
        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        shaderProgram->release();
    }
}

void VideoWidget::setupShaders()
{
    shaderProgram = std::make_unique<QOpenGLShaderProgram>();
    
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)) {
        std::cerr << "Error compilando vertex shader: " 
                  << shaderProgram->log().toStdString() << std::endl;
    }
    
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource)) {
        std::cerr << "Error compilando fragment shader: " 
                  << shaderProgram->log().toStdString() << std::endl;
    }
    
    if (!shaderProgram->link()) {
        std::cerr << "Error enlazando programa: " 
                  << shaderProgram->log().toStdString() << std::endl;
    }
}
