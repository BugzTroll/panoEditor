#ifndef IMAGESPHEREVIEWER_H
#define IMAGESPHEREVIEWER_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

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
    GLuint m_matrixUniform;

    QOpenGLShaderProgram *m_program;
    int m_frame;
};

#endif // IMAGESPHEREVIEWER_H
