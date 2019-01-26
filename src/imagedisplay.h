#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H

#include<QOpenGLWidget>
#include<QOpenGLContext>
#include<QOpenGLFunctions>
#include<QPainter>
#include<QDebug>

class ImageDisplay : public QOpenGLWidget
{
public:
    QImage image;
    ImageDisplay(QWidget *parent) : QOpenGLWidget(parent) { }
    void paintEvent(QPaintEvent*) override;
    void display(const QImage& img);
    void paintGL() override;
    void resizeImageToFit(QImage& image);

protected:
    void initializeGL() override;
};

#endif // IMAGEDISPLAY_H
