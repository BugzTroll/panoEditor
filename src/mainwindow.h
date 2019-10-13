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
    QImage m_image;
    QOpenGLTexture m_texture;
public slots:
    void loadButtonEvent();
    void saveButtonEvent();
    void verticalSliderReleasedEvent();
    void horizontalSliderReleasedEvent();

private:
    Ui::MainWindow *m_ui;
};

#endif // MAINWINDOW_H
