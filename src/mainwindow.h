#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QWindow>
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QMatrix4x4>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QImage loadImage();
    QImage image;
    QOpenGLTexture texture;
public slots:
    void loadButtonEvent();
    void verticalSliderReleasedEvent();
    void horizontalSliderReleasedEvent();
    QImage rotatePanoramicImage(QImage image, QVector3D axis, float angle);
    QMatrix4x4 getRotationMatrixFromV(QVector3D axis, float angle);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
