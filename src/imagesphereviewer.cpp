#include "imagesphereviewer.h"
#include "math.h"
#include <QVector3D>
#include <QtDebug>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

struct floatPixel {
  float r = 1.0;
  float g = 0.0;
  float b = 0.0;
  float a = 1.0;
  floatPixel(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha){}
  floatPixel(){}
};

ImageSphereViewer::ImageSphereViewer(QWidget *parent) :
    QOpenGLWidget(parent),
    m_projectionMatrix(),
    m_viewMatrix(),
    m_modelMatrix(),
    m_program(this),
    texture(nullptr),
    m_lastMouseWheelAngle(0){}

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

  return QMatrix4x4(float(aa + bb - cc - dd), float(2 * (bc + ad)), float(2 * (bd - ac)), 0,
                    float(2 * (bc - ad)), float(aa + cc - bb - dd), float(2 * (cd + ab)), 0,
                    float(2 * (bd + ac)), float(2 * (cd - ab)), float(aa + dd - bb - cc), 0,
                    0, 0, 0, 1);

}

QImage ImageSphereViewer::rotateImage(QVector3D axis, float angle){

   QMatrix4x4 rotationMatrix = getRotationMatrixFromV(axis, angle);

   m_program_compute.bind();

   //bind the variables in the shader
   m_program_compute.setUniformValue(m_angleUniform, angle);
   glUniformMatrix4fv(m_rotationMatrixUniform, 1, false, rotationMatrix.data());

   GLuint width = static_cast<GLuint>(texture->width());
   GLuint height = static_cast<GLuint>(texture->height());

   glBindImageTexture(0, m_texOutput, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
   glBindImageTexture(1, m_texInput, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
   
   glDispatchCompute(width, height, 1);
   glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

   //returning the rotated image
   std::vector<floatPixel> transformedImage(width * height);

   glBindTexture(GL_TEXTURE_2D, m_texOutput);
   glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, transformedImage.data());
   unsigned char* data = static_cast<unsigned char*> (malloc (width * height * 4));

    for (size_t i = 0; i < transformedImage.size(); i++){
       data[i*4] = static_cast<unsigned char>(transformedImage[i].b * 255);
       data[i*4 + 1] = static_cast<unsigned char>(transformedImage[i].g * 255);
       data[i*4 + 2] = static_cast<unsigned char>(transformedImage[i].r * 255);
       data[i*4 + 3] = static_cast<unsigned char>(transformedImage[i].a * 255);
   }

   QImage image(data, texture->width(), texture->height(), QImage::Format_RGB32);
   return image;
}

bool ImageSphereViewer::loadShaders(){

	std::ifstream f;

	//fragment shader for projection on cube
	f.open("C:/Users/Bugz/Documents/panoEditor/src/shaders/fragmentShader.txt");
	if (f.is_open()) {
		std::ostringstream ss;
		ss << f.rdbuf();
		m_projectionFragmentShader = ss.str();
		f.close();
		f.clear();
	} else {
		return false;
	}

	//vertex shader for cube
	f.open("C:/Users/Bugz/Documents/panoEditor/src/shaders/vertexShader.txt");
	if (f.is_open()) {
		std::ostringstream ss;
		ss << f.rdbuf();
		m_vertexShader = ss.str();
		f.close();
		f.clear();
	} else {
		return false;
	}

	//compute shader for panorama rotation
	f.open("C:/Users/Bugz/Documents/panoEditor/src/shaders/computeShader.txt");
	if (f.is_open()) {
		std::ostringstream ss;
		ss << f.rdbuf();
		m_rotationComputeShader = ss.str();
		f.close();
		f.clear();
	} else {
		return false;
	}

	return true;
}

void ImageSphereViewer::initializeGL()
{
	assert(loadShaders());

	initializeOpenGLFunctions();
	m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, m_vertexShader.c_str());
	m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, m_projectionFragmentShader.c_str());
	m_program_compute.addShaderFromSourceCode(QOpenGLShader::Compute, m_rotationComputeShader.c_str());
	m_program.link();
	m_program_compute.link();

	m_posAttr = GLuint(m_program.attributeLocation("posAttr"));
	m_projectionMatrixUniform = m_program.uniformLocation("m_projectionMatrix");
	m_viewMatrixUniform = m_program.uniformLocation("m_viewMatrix");
	m_modelMatrixUniform = m_program.uniformLocation("m_modelMatrix");

	m_angleUniform = m_program_compute.uniformLocation("angle");
	m_rotationMatrixUniform = m_program_compute.uniformLocation("rotation");

	float aspectRatio = float(this->rect().width())/float(this->rect().height());

	m_fov = 60.0f;
	m_projectionMatrix.perspective(m_fov, aspectRatio, 0.1f, 100.0f);
	cubeData = generateCube();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void ImageSphereViewer::setTexture(QOpenGLTexture *tex, QImage image)
{
    texture = tex;

    //create the read texture
    int tex_w = image.width(), tex_h = image.height();
    glGenTextures(1, &m_texInput);
    glBindTexture(GL_TEXTURE_2D, m_texInput);

    //create the imageData array manually because the shader needs float
    std::vector<floatPixel> imageData;
    for (int y = 0; y < tex_h; y++) {
      QRgb *rowData = (QRgb*)(image.scanLine(y));
      for (int x = 0; x < tex_w; x++) {
        QRgb color = rowData[x];
        double red = static_cast<double>(qRed(color));
        double green = static_cast<double>(qGreen(color));
        double blue = static_cast<double>(qBlue(color));
		floatPixel p(red / 255.0,green / 255.0,blue / 255.0, 1.0);
        imageData.push_back(p);
      }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, imageData.data());

    //create the write texture
    glGenTextures(1, &m_texOutput);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texOutput);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, imageData.data());
    glBindImageTexture(0, m_texOutput, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void ImageSphereViewer::paintGL() {


    if(texture) {

        //bind the vertex and fragment shader
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, width(), height());
        m_program.bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        m_program.setUniformValue(GLint(m_projectionMatrixUniform), m_projectionMatrix);
        m_program.setUniformValue(GLint(m_viewMatrixUniform), m_viewMatrix);
        m_program.setUniformValue(GLint(m_modelMatrixUniform), m_modelMatrix);
        glBindTexture(GL_TEXTURE_2D, m_texOutput);

        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, cubeData.data());

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, int(cubeData.size()));

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);

        m_program.release();

        ++m_frame;
    }

    update();
}

void ImageSphereViewer::mousePressEvent(QMouseEvent* event)
{
    m_initMouseX = float(event->screenPos().x());
    m_initMouseY = float(event->screenPos().y());
}

void ImageSphereViewer::wheelEvent(QWheelEvent *event){
    float mouseWheelAngle = event->angleDelta().ry();
    if( mouseWheelAngle >= m_lastMouseWheelAngle ){
        m_fov = fmax(m_minFov, m_fov - 10);
    } else {
        m_fov = fmin(m_maxFov, m_fov + 10);
    }

    float aspect = float( this->rect().width() )/float(this->rect().height());
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective(m_fov, aspect, 0.1f, 100.0f);
}

void ImageSphereViewer::mouseMoveEvent(QMouseEvent* event)
{
    float mouseX = float(event->screenPos().x());
    float mouseY = float(event->screenPos().y());
    float diffX = m_initMouseX - mouseX;
    float diffY = m_initMouseY - mouseY;

    float width = float(this->rect().width());
    float height = float(this->rect().height());

    float m_fovV = m_fov / width * height;

    float angleY = diffX/width * m_fov;
    float angleX = diffY/height * m_fovV;

    QVector3D y(0,1,0);
    QVector3D x(1,0,0);

    m_viewMatrix.rotate(angleY, y.x(), y.y(), y.z());
    QVector4D newX = x * m_viewMatrix;
    m_viewMatrix.rotate(angleX, newX.x(), newX.y(), newX.z());

    m_initMouseX = mouseX;
    m_initMouseY = mouseY;
}

void ImageSphereViewer::resizeGL(int w, int h)
{
      float aspect = float(w)/float(h);
      m_projectionMatrix.setToIdentity();
      m_projectionMatrix.perspective(m_fov, aspect, 0.1f, 100.0f);
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
