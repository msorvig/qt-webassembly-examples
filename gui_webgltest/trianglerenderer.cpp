#include "trianglerenderer.h"
#include <QGuiApplication>
#include <QDebug>

TriangleRenderer::TriangleRenderer(std::function<void ()>  requestUpdate)
{
    this->requestUpdate = requestUpdate;

    // Run animation at fixed 60 fps
    rotationTimer = new QTimer(this);
    connect(rotationTimer, &QTimer::timeout, this, &TriangleRenderer::updateRotation);
    rotationTimer->start(16);
}

void TriangleRenderer::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Create and compile the vertex shader
    const char *vShaderSource = R"(#version 300 es
        layout (location = 0) in vec3 aPos;
        uniform mat4 model; 
        void main()
        {
            gl_Position = model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
        })";

    vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    vertexShader->compileSourceCode(vShaderSource);

    // Create and compile the fragment shader
    const char *fShaderSource = R"(#version 300 es
        out highp vec4 FragColor;
        void main()
        {
            FragColor = vec4(0.2, 0.5, 0.7, 1.0);
        })";

    fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    fragmentShader->compileSourceCode(fShaderSource);

    // Create and link the shader program
    shaderProgram = new QOpenGLShaderProgram;
    shaderProgram->addShader(vertexShader);
    shaderProgram->addShader(fragmentShader);
    shaderProgram->link();

    // Define the vertices of the triangle
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    // Create a vertex buffer object (VBO) and a vertex array object (VAO)
    vao = new QOpenGLVertexArrayObject;
    vao->create();
    vao->bind();

    vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo->create();
    vbo->bind();
    vbo->allocate(vertices, sizeof(vertices));

    // Configure vertex attribute pointers
    shaderProgram->bind();
    int vertexLocation = shaderProgram->attributeLocation("aPos");
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(vertexLocation);

    vbo->release();
    vao->release();
    shaderProgram->release();
}

void TriangleRenderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Update the model matrix with rotation
    QMatrix4x4 model;
    model.rotate(rotationAngle, QVector3D(0.0f, 0.0f, 1.0f));

    // Use the shader program and draw the triangle
    shaderProgram->bind();
    shaderProgram->setUniformValue("model", model);
    vao->bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    vao->release();
    shaderProgram->release();
    
    requestUpdate(); // paint at max fps
}

void TriangleRenderer::updateRotation()
{
    rotationAngle += 1.0f;
    if (rotationAngle >= 360.0f)
        rotationAngle -= 360.0f;
    requestUpdate();
}


TriangleWindow::TriangleWindow()
{
    renderer = new TriangleRenderer([this](){
        this->requestUpdate();
    });
}

void TriangleWindow::initializeGL()
{
    renderer->initializeGL();
}

void TriangleWindow::paintGL()
{
    renderer->paintGL();
}

TriangleWidget::TriangleWidget()
{
    renderer = new TriangleRenderer([this](){
        this->update();
    });
}

void TriangleWidget::initializeGL()
{
    renderer->initializeGL();
}

void TriangleWidget::paintGL()
{
    renderer->paintGL();
}


