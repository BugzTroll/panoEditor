#include "imagesphereviewer.h"
#include "math.h"

static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 projectionMatrix;\n"
    "uniform highp mat4 modelviewMatrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = projectionMatrix * modelviewMatrix * posAttr;\n"
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
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_projectionMatrixUniform = m_program->uniformLocation("projectionMatrix");
    m_modelviewMatrixUniform = m_program->uniformLocation("modelviewMatrix");

    projectionMatrix = QMatrix4x4();
    modelviewMatrix = QMatrix4x4();

    float aspectRatio = float(this->rect().width())/float(this->rect().height());

    fov = 60;
    //projectionMatrix.perspective(60.0f, aspectRatio, 0.1f, 100.0f);
    modelviewMatrix.translate(0, 0, -8);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void ImageSphereViewer::paintGL() {

     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glViewport(0, 0, width(), height());
     m_program->bind();

    //matrix.rotate(100.0f * m_frame / 30, 0, 1, 0);

    m_program->setUniformValue(m_projectionMatrixUniform, projectionMatrix);
    m_program->setUniformValue(m_modelviewMatrixUniform, modelviewMatrix);

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
    int angleY = 0;
    int angleX = 0;
    double mouseX = event->screenPos().x();
    double mouseY = event->screenPos().y();
    double diffX = initMouseX - mouseX;
    double diffY = initMouseY - mouseY;
    angleY += -diffX*2;
    angleX += diffY*2;

    modelviewMatrix.rotate(angleY, 0.0, 1.0, 0.0);

    initMouseX = mouseX;
    initMouseY = mouseY;
}

void ImageSphereViewer::resizeGL(int w, int h){
      float aspect = float(w)/float(h);
      projectionMatrix.setToIdentity();
      projectionMatrix.perspective(60.0f, aspect, 0.1f, 100.0f);
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
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
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
