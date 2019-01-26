#include "imagedisplay.h"
#include<QPainter>
#include<QDebug>

void ImageDisplay::initializeGL(){
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
void ImageDisplay::paintGL() {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT);
}

void ImageDisplay::display(const QImage& image){
    this->image = image;
    this->update();
}

QImage ImageDisplay::resizeImageToFit(const QImage& image){
    int viewerWidth = this->rect().width();
    int viewerHeight = this->rect().height();

    QImage scaledImage = image.scaled(viewerWidth, viewerHeight,Qt::KeepAspectRatio);
    return scaledImage;
}

void ImageDisplay::paintEvent(QPaintEvent*){
    if (!image.isNull()) {
        QPainter painter(this);

        QImage resizedImage = resizeImageToFit(image);

        //clear color
        painter.fillRect(rect(), Qt::black);

        //Draw the image in the center
        QRect imgRect(resizedImage.rect());
        QRect devRect(0, 0, painter.device()->width(), painter.device()->height());
        imgRect.moveCenter(devRect.center());

        painter.drawImage(imgRect.topLeft(),resizedImage);
    }
}
