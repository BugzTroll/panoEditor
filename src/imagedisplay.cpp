#include "imagedisplay.h"

void ImageDisplay::initializeGL(){
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
void ImageDisplay::paintGL() {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT);
}

void ImageDisplay::display(const QImage& img){
    image = img;
    this->update();
}

void ImageDisplay::resizeImageToFit(QImage& image){
    int viewerWidth = this->rect().width();
    int viewerHeight = this->rect().height();

    image = image.scaled(viewerWidth, viewerHeight,Qt::KeepAspectRatio);
}

void ImageDisplay::paintEvent(QPaintEvent*){
    QPainter painter(this);

    //clear color
    painter.fillRect(this->rect(), Qt::black);

    //Draw the image in the center
    QRect rect(image.rect());
    QRect devRect(0, 0, painter.device()->width(), painter.device()->height());
    rect.moveCenter(devRect.center());

    qDebug() << this->rect().width() << this->rect().height();
    painter.drawImage(rect.topLeft(),image);
}
