#ifndef IMAGESPHEREVIEWER_H
#define IMAGESPHEREVIEWER_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QOpenGLTexture>
#include <vector>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions_4_3_Core>

class ImageSphereViewer : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
public:
    explicit ImageSphereViewer(QWidget *parent);
    ~ImageSphereViewer() override;
    void paintGL() override;
    void setTexture(QOpenGLTexture *texture, QImage image);

protected:
    void initializeGL() override;
private:
    GLint m_projectionMatrixUniform;
    GLint m_viewMatrixUniform;
    GLint m_modelMatrixUniform;
    GLint m_frameUniform;
    GLint m_angleUniform;
    GLint m_rotationMatrixUniform;

    GLuint m_posAttr;
    GLuint m_texOutput;
    GLuint m_texInput;

    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_modelMatrix;

    QOpenGLShaderProgram m_program;
    QOpenGLShaderProgram m_program_compute;
    int m_frame;
    QOpenGLTexture *texture;
    std::vector<GLfloat> cubeData;
    float m_fov;
    float const m_maxFov = 130;
    float const m_minFov = 30;
    float m_initMouseX;
    float m_initMouseY;
    float m_lastMouseWheelAngle;

	std::string m_projectionFragmentShader;
	std::string m_vertexShader;
	std::string m_rotationComputeShader;

    void mousePressEvent (QMouseEvent* event) override;
    void mouseMoveEvent (QMouseEvent* event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeGL (int w, int h) override;
    std::vector<GLfloat> generateCube();
	bool loadShaders();

public:
    QImage rotateImage(QVector3D axis, float angle);
    QMatrix4x4 getRotationMatrixFromV(QVector3D axis, float angle);
};

#endif // IMAGESPHEREVIEWER_H
