#define _USE_MATH_DEFINES
#include <cmath>

#include "mainwindow.h"
#include <QDebug>
#include <QSignalMapper>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include "imagedisplay.h"
#include "ui_mainwindow.h"
#include <QtGlobal>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    texture(QOpenGLTexture::Target2D)
{
    ui->setupUi(this);
    QObject::connect(ui->loadButton, SIGNAL(clicked()),
                     this, SLOT(loadButtonEvent()));

    QObject::connect(ui->saveButton, SIGNAL(clicked()),
                     this, SLOT(saveButtonEvent()));

    QObject::connect(ui->verticalSlider, SIGNAL(sliderReleased()),
                     this, SLOT(verticalSliderReleasedEvent()));

    QObject::connect(ui->horizontalSlider, SIGNAL(sliderReleased()),
                     this, SLOT(horizontalSliderReleasedEvent()));

}

MainWindow::~MainWindow()
{
    ui->sphereViewer->makeCurrent();
    texture.destroy();
    delete ui;
}

void MainWindow::loadButtonEvent()
{

    texture.destroy();
    texture.create();
    image = loadImage();
    texture.setData(image);
    ui->sphereViewer->setTexture(&texture, image);
    ui->openGLWidget->display(image);
}

void MainWindow::saveButtonEvent(){

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Image"), "",
        tr("JPEG (*.jpg *.jpeg);;PNG (*.png)"));

    if (fileName.isEmpty())
        return;
    else {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                file.errorString());
            return;
        }

        image.save(fileName);
    }
}

void MainWindow::verticalSliderReleasedEvent()
{
    QVector3D v(0, 0, 1);
    float theta = (ui->verticalSlider->value() * M_PI) / 180.0;
    ui->verticalLabel->setText(QString::number(ui->verticalSlider->value()) + "°");
    QImage rotatedImage = ui->sphereViewer->rotateImage(v, theta);
    ui->openGLWidget->display(rotatedImage);
    image = rotatedImage;
}

void MainWindow::horizontalSliderReleasedEvent()
{
    QVector3D v(1, 0, 0);
    float theta = (ui->horizontalSlider->value() * M_PI) / 180.0;
    ui->horizontalLabel->setText(QString::number(ui->horizontalSlider->value()) + "°");
    QImage rotatedImage = ui->sphereViewer->rotateImage(v, theta);
    ui->openGLWidget->display(rotatedImage);
    image = rotatedImage;
}

QImage MainWindow::loadImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "",
        tr("JPEG (*.jpg *.jpeg);;PNG (*.png)"));

    if (fileName.isEmpty()){
        qDebug() << "Error, image is empty";
        return QImage();
    }
    else {
        QImage image = QImage(fileName);
        return image;
    }

  //temp quick loading
  //QImage image = QImage("C:\\Users\\Bugz\\Documents\\panoEditor\\data\\jail.jpg");
  return image;
}

