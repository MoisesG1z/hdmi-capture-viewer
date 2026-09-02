#include "videowidget.h"

#include <QOpenGLShader>
#include <iostream>

// Shader para convertir YUYV a RGB
static const char *vertexShaderSource = R"(
    #version 120
    void main()
    {
        gl_Position = gl_Vertex;
        gl_TexCoord[0] = gl_MultiTexCoord0;
    }
)";

static const char *fragmentShaderSource = R"(
    #version 120
    uniform sampler2D texture1;
    
    void main()
    {
        vec4 color = texture2D(texture1, gl_TexCoord[0].st);
        
        // Conversión YUYV a RGB
        float y = color.r;
        float u = color.g;
        float v = color.b;
        
        float r = y + 1.402 * (v - 0.5);
        float g = y - 0.344136 * (u - 0.5) - 0.714136 * (v - 0.5);
        float b = y + 1.772 * (u - 0.5);
        
        gl_FragColor = vec4(r, g, b, 1.0);
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
    
    // Crear textura
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Crear textura de prueba (color negro)
    int texWidth = 1920;
    int texHeight = 1080;
    unsigned char *data = new unsigned char[texWidth * texHeight * 4];
    memset(data, 0, texWidth * texHeight * 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    delete[] data;
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void VideoWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void VideoWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (shaderProgram) {
        shaderProgram->bind();
        
        glBindTexture(GL_TEXTURE_2D, textureId);
        
        // Dibujar quad
        glBegin(GL_QUADS);
        {
            glTexCoord2f(0, 0); glVertex2f(0, 0);
            glTexCoord2f(1, 0); glVertex2f(width(), 0);
            glTexCoord2f(1, 1); glVertex2f(width(), height());
            glTexCoord2f(0, 1); glVertex2f(0, height());
        }
        glEnd();
        
        glBindTexture(GL_TEXTURE_2D, 0);
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
