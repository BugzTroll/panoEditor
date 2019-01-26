#include "imagedisplay.h"

void ImageDisplay::initializeGL(){
    // Set up the rendering context, load shaders and other resources, etc.:
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
}
void ImageDisplay::paintGL() {
    // Draw the scene
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT);
}

void ImageDisplay::display(const QImage& img){
    image = img;
    this->update();
}

void ImageDisplay::paintEvent(QPaintEvent*){
    QPainter painter(this);
    painter.drawImage(this->rect(),image);
}
