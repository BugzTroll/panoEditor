#ifndef IMAGESPHEREVIEWER_H
#define IMAGESPHEREVIEWER_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <vector>

class ImageSphereViewer : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    ImageSphereViewer(QWidget *parent) : QOpenGLWidget(parent) { }
    void paintGL() override;
protected:
    void initializeGL() override;
private:
    QImage image;
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_projectionMatrixUniform;
    GLuint m_modelviewMatrixUniform;
    double initMouseX;
    double initMouseY;
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 modelviewMatrix;
    float fov;
    void mousePressEvent (QMouseEvent* event) override;
    void mouseReleaseEvent (QMouseEvent* event) override;
    void mouseMoveEvent (QMouseEvent* event) override;
    void resizeGL (int w, int h) override;
    std::vector<std::vector<GLfloat>> generateCube();


    QOpenGLShaderProgram *m_program;
    int m_frame;
};

#endif // IMAGESPHEREVIEWER_H
