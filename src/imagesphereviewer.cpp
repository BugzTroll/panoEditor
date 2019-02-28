#include "imagesphereviewer.h"
#include "math.h"
#include <QVector3D>

static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "uniform highp mat4 projectionMatrix;\n"
    "uniform highp mat4 modelMatrix;\n"
    "uniform highp mat4 viewMatrix;\n"
    "varying highp vec3 pos;\n"
    "void main() {\n"
    "  vec3 pos3 = posAttr.xyz;\n"
    "  pos = pos3;\n"
    "  gl_Position = projectionMatrix * viewMatrix * modelMatrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderProjection =
    "#define PI 3.1415926535897932384626433832795\n"
    "#define PIDIV 1.57\n"
    "#define DOUBLEPI 6.28318530717959\n"
    "varying highp vec3 pos;\n"
    "uniform sampler2D texture;\n"
    "void main() {\n"
    "   float lon = atan(pos.z, pos.x) + PI;\n"
    "   float r = length(vec2(pos.x, pos.z));\n"
    "   float lat = PI / 2.0 - atan(pos.y, r);\n"
    "   float x = 1.0f / DOUBLEPI;\n"
    "   float y = 1.0f / PI;\n"
    "   vec2 rads = vec2(x, y);\n"
    "   vec2 sphereCoords = vec2(lon, lat) * rads;\n"
    "   gl_FragColor = texture2D(texture, sphereCoords);\n"
    "}\n";

ImageSphereViewer::ImageSphereViewer(QWidget *parent) :
    QOpenGLWidget(parent),
    m_program(this),
    projectionMatrix(),
    viewMatrix(),
    modelMatrix(),
    texture(nullptr),
    lastMouseWheelAngle(0){}

void ImageSphereViewer::initializeGL()
{
    initializeOpenGLFunctions();
    m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderProjection);
    m_program.link();

    m_posAttr = GLuint(m_program.attributeLocation("posAttr"));
    m_projectionMatrixUniform = GLuint(m_program.uniformLocation("projectionMatrix"));
    m_viewMatrixUniform = GLuint(m_program.uniformLocation("viewMatrix"));
    m_modelMatrixUniform = GLuint(m_program.uniformLocation("modelMatrix"));

    float aspectRatio = float(this->rect().width())/float(this->rect().height());

    fov = 60.0f;
    projectionMatrix.perspective(fov, aspectRatio, 0.1f, 100.0f);
    cubeData = generateCube();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void ImageSphereViewer::setTexture(QOpenGLTexture *tex)
{
    texture = tex;
}

void ImageSphereViewer::paintGL()
{
    if(texture) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, width(), height());
        m_program.bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        m_program.setUniformValue(GLint(m_projectionMatrixUniform), projectionMatrix);
        m_program.setUniformValue(GLint(m_viewMatrixUniform), viewMatrix);
        m_program.setUniformValue(GLint(m_modelMatrixUniform), modelMatrix);
        texture->bind();

        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, cubeData.data());

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, cubeData.size());

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);

        m_program.release();

        ++m_frame;
    }

    update();
}

void ImageSphereViewer::mousePressEvent(QMouseEvent* event)
{
    initMouseX = float(event->screenPos().x());
    initMouseY = float(event->screenPos().y());
}

void ImageSphereViewer::mouseReleaseEvent(QMouseEvent* event)
{
}

void ImageSphereViewer::wheelEvent(QWheelEvent *event){
    float mouseWheelAngle = event->angleDelta().ry();
    if( mouseWheelAngle >= lastMouseWheelAngle ){
        fov = fmax(minFov, fov - 10);
    } else {
        fov = fmin(maxFov, fov + 10);
    }

    float aspect = float( this->rect().width() )/float(this->rect().height());
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(fov, aspect, 0.1f, 100.0f);
}

void ImageSphereViewer::mouseMoveEvent(QMouseEvent* event)
{
    float mouseX = float(event->screenPos().x());
    float mouseY = float(event->screenPos().y());
    float diffX = initMouseX - mouseX;
    float diffY = initMouseY - mouseY;

    float width = float(this->rect().width());
    float height = float(this->rect().height());

    float fovV = fov / width * height;

    float angleY = diffX/width * fov;
    float angleX = diffY/height * fovV;

    QVector3D y(0,1,0);
    QVector3D x(1,0,0);

    viewMatrix.rotate(angleY, y.x(), y.y(), y.z());
    QVector4D newX = x * viewMatrix;
    viewMatrix.rotate(angleX, newX.x(), newX.y(), newX.z());

    initMouseX = mouseX;
    initMouseY = mouseY;
}

void ImageSphereViewer::resizeGL(int w, int h)
{
      float aspect = float(w)/float(h);
      projectionMatrix.setToIdentity();
      projectionMatrix.perspective(fov, aspect, 0.1f, 100.0f);
}

std::vector<GLfloat> ImageSphereViewer::generateCube(){

    std::vector<GLfloat> g_vertex_buffer_data = {
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };

     return g_vertex_buffer_data;
}
