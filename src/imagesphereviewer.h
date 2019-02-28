#ifndef IMAGESPHEREVIEWER_H
#define IMAGESPHEREVIEWER_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QOpenGLTexture>
#include <vector>

class ImageSphereViewer : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    explicit ImageSphereViewer(QWidget *parent);
   // ~ImageSphereViewer();
    void paintGL() override;
    void setTexture(QOpenGLTexture *texture);

protected:
    void initializeGL() override;
private:
    QImage image;
    GLuint m_posAttr;
    GLuint m_projectionMatrixUniform;
    GLuint m_viewMatrixUniform;
    GLuint m_modelMatrixUniform;
    float initMouseX;
    float initMouseY;
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 modelMatrix;
    float fov;
    QOpenGLShaderProgram m_program;
    int m_frame;
    QOpenGLTexture *texture;
    std::vector<GLfloat> cubeData;
    float const maxFov = 130;
    float const minFov = 30;
    float lastMouseWheelAngle;

    void mousePressEvent (QMouseEvent* event) override;
    void mouseReleaseEvent (QMouseEvent* event) override;
    void mouseMoveEvent (QMouseEvent* event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeGL (int w, int h) override;
    std::vector<GLfloat> generateCube();
    void testShader(std::vector<GLfloat> points);

public:
    void rotatePanorama(int value, std::string axis);
};

#endif // IMAGESPHEREVIEWER_H
