#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <memory>

class VideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget();

    void updateFrame(int offset, int bytesUsed);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void setupShaders();

    std::unique_ptr<QOpenGLShaderProgram> shaderProgram;
    GLuint textureId{0};
    GLuint vao{0};
    GLuint vbo{0};

    int frameWidth{1920};
    int frameHeight{1080};
};

#endif // VIDEOWIDGET_H
