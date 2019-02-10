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

static const char *fragmentShaderColor =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
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

void ImageSphereViewer::initializeGL(){
    initializeOpenGLFunctions();
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderProjection);
    m_program->link();
    m_posAttr = GLuint(m_program->attributeLocation("posAttr"));
    m_projectionMatrixUniform = GLuint(m_program->uniformLocation("projectionMatrix"));
    m_viewMatrixUniform = GLuint(m_program->uniformLocation("viewMatrix"));
    m_modelMatrixUniform = GLuint(m_program->uniformLocation("modelMatrix"));

    projectionMatrix = QMatrix4x4();
    viewMatrix = QMatrix4x4();
    modelMatrix = QMatrix4x4();

    float aspectRatio = float(this->rect().width())/float(this->rect().height());

    fov = 60.0f;
    projectionMatrix.perspective(fov, aspectRatio, 0.1f, 100.0f);
    modelMatrix.scale(20);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    texture = nullptr;
}

void ImageSphereViewer::setTexture(QOpenGLTexture *tex){
    texture = tex;
}

void ImageSphereViewer::testShader(std::vector<GLfloat> points){

    for (size_t i = 0; i < points.size(); i+=3){
        float px = points[i];
        float py = points[i+1];
        float pz = points[i+2];

        QVector3D v(px, py, pz);
        v = v.normalized();

        float lon = atan2(v.x(), v.z());
        float lat = asin(v.y());

        float textCoordX = (lon + M_PI) / (2.0 * M_PI);
        float textCoordY = float(lat / float(M_PI));
        qDebug() << px << ", " << py <<", "<< pz;
        qDebug() << v.x() << ", " << v.y() <<", "<< v.z();
        qDebug() << "lon lat " << lon << " " << lat;
        qDebug() << textCoordX << "," << textCoordY;

        Q_ASSERT(textCoordX < 0);
        Q_ASSERT(textCoordX > 1);
        Q_ASSERT(textCoordY < 0);
        Q_ASSERT(textCoordY > 1);
    }
}

void ImageSphereViewer::paintGL() {

    if(texture) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, width(), height());
        m_program->bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        m_program->setUniformValue(GLint(m_projectionMatrixUniform), projectionMatrix);
        m_program->setUniformValue(GLint(m_viewMatrixUniform), viewMatrix);
        m_program->setUniformValue(GLint(m_modelMatrixUniform), modelMatrix);
        texture->bind();

        std::vector<std::vector<GLfloat>> cubeData = generateCube();

        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, cubeData[0].data());

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, 3*12);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);

        m_program->release();

        ++m_frame;
    }

    update();
}

void ImageSphereViewer::mousePressEvent(QMouseEvent* event){
    initMouseX = float(event->screenPos().x());
    initMouseY = float(event->screenPos().y());
}

void ImageSphereViewer::mouseReleaseEvent(QMouseEvent* event){
}

void ImageSphereViewer::mouseMoveEvent(QMouseEvent* event){
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

void ImageSphereViewer::resizeGL(int w, int h){
      float aspect = float(w)/float(h);
      projectionMatrix.setToIdentity();
      projectionMatrix.perspective(fov, aspect, 0.1f, 100.0f);
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
