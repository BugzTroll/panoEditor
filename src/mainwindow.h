#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QWindow>
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QSlider>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void loadButtonEvent();
    void saveButtonEvent();
    void verticalSliderReleasedEvent();
    void horizontalSliderReleasedEvent();

private:
	QImage loadImage();
	
	QImage m_image;
	QOpenGLTexture m_texture;
    Ui::MainWindow *m_ui;
	void MainWindow::rotateImage(QSlider *slider, const QVector3D &vector);
};

#endif // MAINWINDOW_H
