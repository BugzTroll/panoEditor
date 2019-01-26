#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H

#include<QOpenGLWidget>
#include<QOpenGLContext>
#include<QOpenGLFunctions>
#include<QPainter>

class ImageDisplay : public QOpenGLWidget
{
public:
    ImageDisplay(QWidget *parent) : QOpenGLWidget(parent) { }
    QImage image;
    void paintEvent(QPaintEvent*) override;
    void display(const QImage& img);
    void paintGL() override;

protected:
    void initializeGL() override;
};

#endif // IMAGEDISPLAY_H
