#pragma once

#include "openglwidget.h"

#include <QElapsedTimer>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QWindow>
#include <QSlider>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QtMath>
#include <cmath>

class Window : public OpenGLWidget
{
    Q_OBJECT

public:
    Window();
    ~Window();

    void onInit() override;
    void onRender() override;
    void onResize(size_t width, size_t height) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void computeXRange(float& xL, float& xR) const;

private:
    QElapsedTimer timer_;
    GLint colorUniform_ = -1;
    float hue_ = 0;

    GLint mvpUniform_ = -1;

    QMatrix4x4 model_;
    QMatrix4x4 view_;
    QMatrix4x4 projection_;

    QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
    QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
    QOpenGLVertexArrayObject vao_;

    std::unique_ptr<QOpenGLTexture> texture_;
    std::unique_ptr<QOpenGLShaderProgram> program_;

    bool animated_ = true;
    bool colorAnimated_ = animated_;

    float camX_ = 0.0f;
    float camY_ = 0.0f;
    float camZ_ = 4.0f;
    float fov_ = 60.0f;
    float aspect_ = 1.0f;

    QPoint lastMousePos_;
    bool   panning_ = false;

    void rebuildProjection();
    void rebuildCamera();
};
