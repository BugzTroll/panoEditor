#include "imagesphereviewer.h"
#include "math.h"

static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 projectionMatrix;\n"
    "uniform highp mat4 modelMatrix;\n"
    "uniform highp mat4 viewMatrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";

void ImageSphereViewer::initializeGL(){
    initializeOpenGLFunctions();
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = GLuint(m_program->attributeLocation("posAttr"));
    m_colAttr = GLuint(m_program->attributeLocation("colAttr"));
    m_projectionMatrixUniform = GLuint(m_program->uniformLocation("projectionMatrix"));
    m_viewMatrixUniform = GLuint(m_program->uniformLocation("viewMatrix"));
    m_modelMatrixUniform = GLuint(m_program->uniformLocation("modelMatrix"));

    projectionMatrix = QMatrix4x4();
    viewMatrix = QMatrix4x4();
    modelMatrix = QMatrix4x4();

    float aspectRatio = float(this->rect().width())/float(this->rect().height());

    fov = 60;
    projectionMatrix.perspective(70.0f, aspectRatio, 0.1f, 100.0f);
    //modelMatrix.translate(0, 0, -8);
    modelMatrix.scale(20);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void ImageSphereViewer::paintGL() {

     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glViewport(0, 0, width(), height());
     m_program->bind();

    //matrix.rotate(100.0f * m_frame / 30, 0, 1, 0);

    m_program->setUniformValue(GLint(m_projectionMatrixUniform), projectionMatrix);
    m_program->setUniformValue(GLint(m_viewMatrixUniform), viewMatrix);
    m_program->setUniformValue(GLint(m_modelMatrixUniform), modelMatrix);

    std::vector<std::vector<GLfloat>> cubeData = generateCube();

    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, cubeData[0].data());
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, cubeData[1].data());

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 3*12);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    m_program->release();

    ++m_frame;

    update();
}

void ImageSphereViewer::mousePressEvent(QMouseEvent* event){
    initMouseX = event->screenPos().x();
    initMouseY = event->screenPos().y();
}

void ImageSphereViewer::mouseReleaseEvent(QMouseEvent* event){
}

void ImageSphereViewer::mouseMoveEvent(QMouseEvent* event){
//    int angleY = 0;
//    int angleX = 0;
//    double mouseX = event->screenPos().x();
//    double mouseY = event->screenPos().y();
//    double diffX = initMouseX - mouseX;
//    double diffY = initMouseY - mouseY;
//    angleY += -diffX*2;
//    angleX += diffY*2;

//    QVector3D y(0,1,0);
//    QVector3D x(1,0,0);

//    modelMatrix.rotate(angleY, y.x(), y.y(), y.z());
//    //viewMatrix.rotate(angleY, x.x(), x.y(), x.z());

//    initMouseX = mouseX;
//    initMouseY = mouseY;

    int angleY = 0;
    int angleX = 0;
    double mouseX = event->screenPos().x();
    double mouseY = event->screenPos().y();
    double diffX = initMouseX - mouseX;
    double diffY = initMouseY - mouseY;
    angleY += -diffX;
    angleX += diffY;

    QVector3D y(0,1,0);
    QVector3D x(1,0,0);

    QVector4D newY = y * viewMatrix;
    QVector4D newX = x * viewMatrix;


    //viewMatrix.setToIdentity();
    viewMatrix.rotate(-angleY, newY.x(), newY.y(), newY.z());
    viewMatrix.rotate(angleX, newX.x(), newX.y(), newX.z());

    initMouseX = mouseX;
    initMouseY = mouseY;
}

void ImageSphereViewer::resizeGL(int w, int h){
      float aspect = float(w)/float(h);
      projectionMatrix.setToIdentity();
      projectionMatrix.perspective(70.0f, aspect, 0.1f, 100.0f);
}

std::vector<std::vector<GLfloat>> ImageSphereViewer::generateCube(){

    std::vector<std::vector<GLfloat>> cubeData;
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

    cubeData.push_back(g_vertex_buffer_data);

    std::vector<GLfloat> g_color_buffer_data = {
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.5f,  1.0f,
        0.0f,  0.5f,  1.0f,
        0.0f,  0.5f,  1.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        0.0f,  1.0f,  1.0f,
        0.0f,  1.0f,  1.0f,
        0.0f,  1.0f,  1.0f,
        0.0f,  1.0f,  1.0f,
        0.0f,  1.0f,  1.0f,
        0.0f,  1.0f,  1.0f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };

     cubeData.push_back(g_color_buffer_data);

     return cubeData;
}
