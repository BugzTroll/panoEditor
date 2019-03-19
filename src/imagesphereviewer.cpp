#include "imagesphereviewer.h"
#include "math.h"
#include <QVector3D>
#include <QtDebug>
#include <stdio.h>
#include <stdlib.h>

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

//float centeredX = 2 * (x + 0.5) / imageWidth - 1;
//float centeredY = 2 * (y + 0.5) / imageHeight - 1;
//
//float lat = 0.5 * M_PI * centeredY;
//float lon = M_PI * centeredX;
//
//float XSphere = cos(lat) * cos(lon);
//float YSphere = cos(lat) * sin(lon);
//float ZSphere = sin(lat);
//
//QVector4D point(XSphere, YSphere, ZSphere, 1);
//QVector4D rotatedPoint = rotationM * point;
//
//float D = sqrt(rotatedPoint.x() * rotatedPoint.x() + rotatedPoint.y() * rotatedPoint.y());
//float newLat = atan2(rotatedPoint.z(), D);
//float newLng = atan2(rotatedPoint.y(), rotatedPoint.x());
//
//float newX = newLng / M_PI;
//float newY = newLat / (M_PI * 0.5);
//
//int imgCoordX = (round(((0.5 * (newX + 1)) * imageWidth) - 0.5));
//int imgCoordY = round(((0.5 * (newY + 1)) * imageHeight) - 0.5);
//
//imgCoordX = qMax(0, imgCoordX);
//imgCoordY = qMax(0, imgCoordY);
//
//imgCoordX = qMin(static_cast<int>(imageWidth), imgCoordX);
//imgCoordY = qMin(static_cast<int>(imageHeight), imgCoordY);

static const char *computeShader =
    "  #version 430\n "
    "  #define PI 3.1415926535897932384626433832795\n"
    "  uniform float angle;\n"
    "  uniform highp mat4 rotationMatrix;\n"
    "  mat4 identity = mat4(vec4(1.0,0.0,0.0,0.0), vec4(0.0,1.0,0.0,0.0), vec4(0.0,0.0,1.0,0.0), vec4(0.0,0.0,0.0,1.0));\n"
    "  layout(local_size_x = 1, local_size_y = 1) in;\n"
    "  layout(rgba32f, binding = 0) uniform image2D img_output;\n"
    "  layout(rgba32f, binding = 1) uniform image2D texture;\n"
    "  uniform mat4 rotation;\n"
    "  void main() {\n"
    "     ivec2 imgSize = imageSize(texture);\n"
    "     float x = float(gl_GlobalInvocationID.x);\n"
    "     float y = float(gl_GlobalInvocationID.y);\n"
    "     float width = float(imgSize.x);\n"
    "     float height = float(imgSize.y);\n"
    "     float centeredX = 2.0 * (x + 0.5) / width - 1.0;\n"
    "     float centeredY = 2.0 * (y + 0.5) / height - 1.0;\n"
    "     float lat = 0.5 * PI * centeredY;\n" 
    "     float lon = PI * centeredX;\n"
    "     float XSphere = cos(lat) * cos(lon);\n"
    "     float YSphere = cos(lat) * sin(lon);\n"
    "     float ZSphere = sin(lat);\n"
    "     vec4 point = vec4(XSphere, YSphere, ZSphere, 1.0);\n"
    "     vec4 rotatedPoint = rotation * point;\n"
    "     float D = sqrt(rotatedPoint.x * rotatedPoint.x + rotatedPoint.y * rotatedPoint.y);\n"
    "     float newLat = atan(rotatedPoint.z, D);\n"
    "     float newLng = atan(rotatedPoint.y, rotatedPoint.x);\n"
    "     float newX = newLng / PI;\n"
    "     float newY = newLat / (PI * 0.5);\n"
    "     float imgCoordX = round(((0.5 * ( newX + 1.0 )) * width) - 0.5);\n"
    "     float imgCoordY = round(((0.5 * ( newY + 1.0 )) * height) - 0.5);\n"
    "     imgCoordX = min(imgSize.x - 1.0, max(0.0, imgCoordX));\n"
    "     imgCoordY = min(imgSize.y - 1.0, max(0.0, imgCoordY));\n"
    "     int i_imgCoordX = int(imgCoordX);\n"
    "     int i_imgCoordY = int(imgCoordY);\n"
    "     vec4 rotatedColor = imageLoad(texture, ivec2(i_imgCoordX , i_imgCoordY));\n"

    ""
    "     ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);\n"
    "     vec4 texColor = imageLoad(texture, pixel_coords);\n"
    "     imageStore(img_output, pixel_coords, rotatedColor);\n"
    "  }\n";

struct floatPixel {
  float r = 1.0;
  float g = 0.0;
  float b = 0.0;
  float a = 1.0;
  floatPixel(float red, float green, float blue, float alpha) :
    r(red), g(green), b(blue), a(alpha)
  {
  }
  floatPixel()
  {
  }
};

ImageSphereViewer::ImageSphereViewer(QWidget *parent) :
    QOpenGLWidget(parent),
    m_program(this),
    projectionMatrix(),
    viewMatrix(),
    modelMatrix(),
    texture(nullptr),
    lastMouseWheelAngle(0){}

ImageSphereViewer::~ImageSphereViewer(){
}

QMatrix4x4 ImageSphereViewer::getRotationMatrixFromV(QVector3D axis, float angle)
{
  float a = cos(angle / 2.0);
  float b = -axis.x() * sin(angle / 2.0);
  float c = -axis.y() * sin(angle / 2.0);
  float d = -axis.z() * sin(angle / 2.0);
  float aa = a * a;
  float bb = b * b;
  float cc = c * c;
  float dd = d * d;
  float bc = b * c;
  float ad = a * d;
  float ac = a * c;
  float ab = a * b;
  float bd = b * d;
  float cd = c * d;

  return QMatrix4x4(aa + bb - cc - dd, 2 * (bc + ad), 2 * (bd - ac), 0,
    2 * (bc - ad), aa + cc - bb - dd, 2 * (cd + ab), 0,
    2 * (bd + ac), 2 * (cd - ab), aa + dd - bb - cc, 0,
    0, 0, 0, 1);

}

void ImageSphereViewer::rotateImage(QVector3D axis, float angle){

  qDebug() << "Rotating";

   QMatrix4x4 rotationMatrix = getRotationMatrixFromV(axis, angle);
   //rotationMatrix.setToIdentity();

   qDebug() << rotationMatrix;

   m_program_compute.bind();

   //bind the variables in the shader
   m_program_compute.setUniformValue(GLint(m_angleUniform), angle);
   //m_program_compute.setUniformValue(GLint(m_rotationMatrixUniform), rotationMatrix);
   glUniformMatrix4fv(m_rotationMatrixUniform, 1, false, rotationMatrix.data());

   glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
   glBindImageTexture(1, tex_input, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
   
   glDispatchCompute((GLuint)texture->width(), (GLuint)texture->height(), 1);
   glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void ImageSphereViewer::initializeGL()
{
    initializeOpenGLFunctions();
    m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderProjection);
    m_program_compute.addShaderFromSourceCode(QOpenGLShader::Compute, computeShader);
    m_program.link();
    m_program_compute.link();

    m_posAttr = GLuint(m_program.attributeLocation("posAttr"));
    m_projectionMatrixUniform = GLuint(m_program.uniformLocation("projectionMatrix"));
    m_viewMatrixUniform = GLuint(m_program.uniformLocation("viewMatrix"));
    m_modelMatrixUniform = GLuint(m_program.uniformLocation("modelMatrix"));

    m_angleUniform = GLuint(m_program_compute.uniformLocation("angle"));
    m_rotationMatrixUniform = GLuint(m_program_compute.uniformLocation("rotation"));

    float aspectRatio = float(this->rect().width())/float(this->rect().height());

    fov = 60.0f;
    projectionMatrix.perspective(fov, aspectRatio, 0.1f, 100.0f);
    cubeData = generateCube();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void ImageSphereViewer::setTexture(QOpenGLTexture *tex, QImage image)
{
    texture = tex;

    //create the read texture
    int tex_w = image.width(), tex_h = image.height();
    glGenTextures(1, &tex_input);
    glBindTexture(GL_TEXTURE_2D, tex_input);

    //create the imageData array manually because the shader needs float
    std::vector<floatPixel> imageData;
    for (int y = 0; y < tex_h; y++) {
      QRgb *rowData = (QRgb*)image.scanLine(y);
      for (int x = 0; x < tex_w; x++) {
        QRgb color = rowData[x];
        int red = qRed(color);
        int green = qGreen(color);
        int blue = qBlue(color);
        floatPixel p(static_cast<float>(red) / 255.0, static_cast<float>(green) / 255.0, static_cast<float>(blue) / 255.0, 1.0);
        imageData.push_back(p);
      }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, imageData.data());

    //create the write texture
    glGenTextures(1, &tex_output);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_output);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT,NULL);
    glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void ImageSphereViewer::paintGL() {


    if(texture) {

        ////bind the compute shader
        //glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        //glBindImageTexture(1, tex_input, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        //m_program_compute.bind();
        //int tex_w = 512, tex_h = 512;
        //glDispatchCompute((GLuint)texture->width(), (GLuint)texture->height(), 1);
        //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        //bind the vertex and fragment shader
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, width(), height());
        m_program.bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        m_program.setUniformValue(GLint(m_projectionMatrixUniform), projectionMatrix);
        m_program.setUniformValue(GLint(m_viewMatrixUniform), viewMatrix);
        m_program.setUniformValue(GLint(m_modelMatrixUniform), modelMatrix);
        glBindTexture(GL_TEXTURE_2D, tex_output);

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

void ImageSphereViewer::rotatePanorama(int value, std::string axis){
    qDebug() << "rotate pliz";
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
