#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H

#include<QOpenGLWidget>
#include<QOpenGLContext>
#include<QOpenGLFunctions>

class ImageDisplay : public QOpenGLWidget
{
public:
    QImage image;
    ImageDisplay(QWidget *parent) : QOpenGLWidget(parent) { }
    void paintEvent(QPaintEvent*) override;
    void display(const QImage& img);
    void paintGL() override;
protected:
    void initializeGL() override;
private:
    QImage resizeImageToFit(const QImage& image);

};

#endif // IMAGEDISPLAY_H
