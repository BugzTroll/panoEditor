#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QWindow>
#include <QOpenGLFunctions>

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

public slots:
    void loadButtonEvent();

private:
    Ui::MainWindow *ui;


};

#endif // MAINWINDOW_H
