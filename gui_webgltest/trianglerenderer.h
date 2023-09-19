#ifndef TRIANGLERENDERER_H
#define TRIANGLERENDERER_H

#include <QtOpenGL>
#include <QtOpenGLWidgets>

class TriangleRenderer: public QObject, protected QOpenGLFunctions
{
public:
    TriangleRenderer(std::function<void ()> requestUpdate);

    void initializeGL();
    void paintGL();
    void updateRotation();

private:
    std::function<void ()> requestUpdate;
    QOpenGLShader *vertexShader = nullptr;
    QOpenGLShader *fragmentShader = nullptr;
    QOpenGLShaderProgram *shaderProgram = nullptr;
    QOpenGLBuffer *vbo = nullptr;
    QOpenGLVertexArrayObject *vao = nullptr;
    QTimer *rotationTimer = nullptr;
    qreal rotationAngle = 0;
};

class TriangleWindow : public QOpenGLWindow
{
public:
    TriangleWindow();

protected:
    void initializeGL() override;
    void paintGL() override;
    void updateRotation();

private:
    TriangleRenderer *renderer = nullptr;
};

class TriangleWidget : public QOpenGLWidget
{
public:
    TriangleWidget();

protected:
    void initializeGL() override;
    void paintGL() override;
    void updateRotation();

private:
    TriangleRenderer *renderer = nullptr;
};


#endif // TRIANGLEWINDOW_H
