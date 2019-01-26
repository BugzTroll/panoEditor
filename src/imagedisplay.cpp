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

QImage ImageDisplay::resizeImageToFit(const QImage& image){
    int viewerWidth = this->rect().width();
    int viewerHeight = this->rect().height();

    QImage scaledImage = image.scaled(viewerWidth, viewerHeight,Qt::KeepAspectRatio);
    return scaledImage;
}

void ImageDisplay::paintEvent(QPaintEvent*){

    qDebug() << "painting";
    QPainter painter(this);
    QImage resizedImage = this->resizeImageToFit(image);

    //clear color
    painter.fillRect(this->rect(), Qt::black);

    //Draw the image in the center
    QRect rect(resizedImage.rect());
    QRect devRect(0, 0, painter.device()->width(), painter.device()->height());
    rect.moveCenter(devRect.center());

    qDebug() << this->rect().width() << this->rect().height();
    painter.drawImage(rect.topLeft(),resizedImage);
}
