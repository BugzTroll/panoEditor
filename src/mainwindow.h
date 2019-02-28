#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QWindow>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

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
    void horizontalSliderMovedEvent(int value);
    void verticalSliderMovedEvent(int value);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
