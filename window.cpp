#include "window.h"

#include <array>
#include <QLabel>

float a = 1.4f;
float b = -2.3f;
float c = 2.4f;
float d = -2.1f;
int N = 100000;

std::vector<float> PeterDeJongFractal(float a, float b, float c, float d, int numPoints) {
    std::vector<float> points;
    points.reserve(numPoints * 3);
    float x = 0.1f;
    float y = 0.1f;
    for (int i = 0; i < numPoints; ++i) {
        float nx = std::sin(a * y) - std::cos(b * x);
        float ny = std::sin(c * x) - std::cos(d * y);
        float dx = nx - x;
        float dy = ny - y;
        float speed = std::sqrt(dx * dx + dy * dy);

        points.push_back(nx);
        points.push_back(ny);
        points.push_back(speed);
        x = nx;
        y = ny;
    }
    return points;
}

Window::Window()
{
    // Add label for FPS
    auto* fpsLabel = new QLabel("FPS: 0", this);
    fpsLabel->setStyleSheet("QLabel { color: white; }");
    fpsLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    fpsLabel->adjustSize();
    fpsLabel->move(0, 0);
    fpsLabel->raise();

    connect(this, &Window::fpsUpdated, [fpsLabel](double fps) {
        fpsLabel->setText(QString("FPS: %1").arg(fps, 0, 'f', 0));
        fpsLabel->adjustSize();
    });

    auto makeSlider = [this](int x, int y, int w, int minV, int maxV, int initV,
                             const QString& name, auto onChanged) {
        auto* lbl = new QLabel(QString("%1: %2").arg(name).arg(initV), this);
        lbl->setStyleSheet("QLabel { color: white; background: rgba(0,0,0,128); }");
        lbl->move(x, y);
        lbl->adjustSize();

        auto* s = new QSlider(Qt::Horizontal, this);
        s->setRange(minV, maxV);
        s->setValue(initV);
        s->setGeometry(x, y + 20, w, 20);
        s->setStyleSheet("QSlider { background: rgba(0,0,0,64); }");

        connect(s, &QSlider::valueChanged, this,
                [lbl, name, onChanged](int v) {
                    lbl->setText(QString("%1: %2").arg(name).arg(v));
                    lbl->adjustSize();
                    onChanged(v);
                });
        return s;
    };
    makeSlider(10, 40, 200, 0, 400, static_cast<int>(a * 100), "a",
        [this](int v) {
        a = v / 100.0f;
        auto points = PeterDeJongFractal(a,b,c,d,N);
        vbo_.bind();
        vbo_.write(0,points.data(), static_cast<int>(points.size() * sizeof(GLfloat)));
        vbo_.release();
        update();
    });

    makeSlider(10, 90, 200, -400, 400, static_cast<int>(b * 100), "b",
        [this](int v) {
        b = v / 100.0f;
        auto points = PeterDeJongFractal(a,b,c,d,N);
        vbo_.bind();
        vbo_.write(0,points.data(), static_cast<int>(points.size() * sizeof(GLfloat)));
        vbo_.release();
        update();
    });

    makeSlider(10, 140, 200, -400, 400, static_cast<int>(c * 100), "c",
        [this](int v) {
        c = v / 100.0f;
        auto points = PeterDeJongFractal(a,b,c,d,N);
        vbo_.bind();
        vbo_.write(0,points.data(), static_cast<int>(points.size() * sizeof(GLfloat)));
        vbo_.release();
        update();
    });

    makeSlider(10, 190, 200, -400, 400, static_cast<int>(d * 100), "d",
        [this](int v) {
        d = v / 100.0f;
        auto points = PeterDeJongFractal(a,b,c,d,N);
        vbo_.bind();
        vbo_.write(0,points.data(), static_cast<int>(points.size() * sizeof(GLfloat)));
        vbo_.release();
        update();
    });

    makeSlider(10, 240, 200, 4, 6, 5, "N=10^k",
        [this](int v) {
            N = static_cast<int>(std::pow(10.0f, v));
            auto points = PeterDeJongFractal(a,b,c,d,N);
            vbo_.bind();
            vbo_.allocate(points.data(), static_cast<int>(points.size() * sizeof(GLfloat)));
            vbo_.release();
            update();
        });
}

Window::~Window()
{
    texture_.reset();
    program_.reset();
}

void Window::onInit()
{
    // Configure shaders
    program_ = std::make_unique<QOpenGLShaderProgram>(this);
    program_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/template.vs");
    program_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/template.fs");
    if (!program_->link()) {
        qDebug() << "Ошибка линковки:" << program_->log();
    }
    program_->bind();

    // Configure textures
    texture_ = std::make_unique<QOpenGLTexture>(QImage(":/textures/cat.jpg"));
    texture_->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    texture_->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
    program_->setUniformValue("tex_2d", 0);

    // Configure uniforms
    mvpUniform_ = program_->uniformLocation("mvp");
    colorUniform_ = program_->uniformLocation("color");
    program_->setUniformValue(colorUniform_, QColor::fromHsv(0, 0, 255));

    // Create VAO object
    vao_.create();
    vao_.bind();

    // Create VBO
    auto points = PeterDeJongFractal(a,b,c,d,N);
    vbo_.create();
    vbo_.bind();
    vbo_.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    vbo_.allocate(points.data(), static_cast<int>(points.size() * sizeof(GLfloat)));

    // Create IBO
    ibo_.create();
    ibo_.bind();
    ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
//    ibo_.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(GLuint)));

    // Bind attributes
    program_->enableAttributeArray(0);
    program_->setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(3 * sizeof(GLfloat)));

    program_->enableAttributeArray(1);
    program_->setAttributeBuffer(1,
                                 GL_FLOAT,
                                 static_cast<int>(2 * sizeof(GLfloat)),
                                 1,
                                 static_cast<int>(3 * sizeof(GLfloat)));

    // Release all
    program_->release();

    rebuildProjection();
    rebuildCamera();

    vao_.release();
    vbo_.release();
    ibo_.release();

    // Start timer for animation (if needed)
    timer_.start();

    // Еnable depth test and face culling
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Clear all FBO buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::onRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate MVP matrix
//    model_.setToIdentity();
//    model_.translate(0, 0, -1.5);
//    view_.setToIdentity();
    const auto mvp = projection_ * view_;

    // Bind shader program
    program_->bind();

    // Update uniform values
    program_->setUniformValue(mvpUniform_, mvp);

    if (animated_ && colorAnimated_) {
        const float dt = timer_.restart() / 1000.0f;
        hue_ = std::fmod(hue_ + dt * (60.0f / 360.0f), 1.0f);
        program_->setUniformValue(colorUniform_, QColor::fromHsvF(hue_, 1.0, 1.0));
    }

    // Bind VAO
    vao_.bind();

    glDrawArrays(GL_POINTS, 0, N);

    // Release all
    vao_.release();
    program_->release();

    if (animated_)
        update();
}

void Window::onResize(const size_t width, const size_t height)
{
    glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));
    aspect_ = static_cast<float>(width) / static_cast<float>(height ? height : 1);

    rebuildProjection();
    rebuildCamera();
}

void Window::rebuildCamera()
{
    view_.setToIdentity();
    view_.lookAt(QVector3D(camX_, camY_, camZ_), QVector3D(camX_, camY_, 0.0f), QVector3D(0, 1, 0)
    );
}

void Window::rebuildProjection()
{
    projection_.setToIdentity();
    projection_.perspective(fov_, aspect_, 0.1f, 100.0f);
}

void Window::wheelEvent(QWheelEvent* e)
{
    const float factor = std::pow(0.9f, e->angleDelta().y() / 120.0f);
    const float hxOld = camZ_ * std::tan(qDegreesToRadians(fov_ * 0.5f)) * aspect_;
    const float hyOld = camZ_ * std::tan(qDegreesToRadians(fov_ * 0.5f));
    const float ndcX = 2.0f * e->position().x() / width()  - 1.0f;
    const float ndcY = 1.0f - 2.0f * e->position().y() / height();
    const float worldX_before = camX_ + ndcX * hxOld;
    const float worldY_before = camY_ + ndcY * hyOld;

    fov_ = std::clamp(fov_ * factor, 5.0f, 150.0f);

    const float hxNew = camZ_ * std::tan(qDegreesToRadians(fov_ * 0.5f)) * aspect_;
    const float hyNew = camZ_ * std::tan(qDegreesToRadians(fov_ * 0.5f));

    camX_ = worldX_before - ndcX * hxNew;
    camY_ = worldY_before - ndcY * hyNew;

    rebuildProjection();
    rebuildCamera();
    update();
    e->accept();
}

void Window::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        lastMousePos_ = e->pos();
        panning_ = true;
        setCursor(Qt::ClosedHandCursor);
        e->accept();
    }
}

void Window::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        panning_ = false;
        unsetCursor();
        e->accept();
    }
}

void Window::mouseMoveEvent(QMouseEvent* e)
{
    if (!panning_) return;

    const QPoint delta = e->pos() - lastMousePos_;
    lastMousePos_ = e->pos();

    const float hy = camZ_ * std::tan(qDegreesToRadians(fov_ * 0.5f));
    const float hx = hy * aspect_;
    const float worldPerPxX = (2.0f * hx) / width();
    const float worldPerPxY = (2.0f * hy) / height();

    camX_ -= delta.x() * worldPerPxX;
    camY_ += delta.y() * worldPerPxY;

    rebuildCamera();
    update();
    e->accept();
}
