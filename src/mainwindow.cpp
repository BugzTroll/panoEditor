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

    QObject::connect(ui->horizontalSlider, SIGNAL(sliderMoved(int)),
                     this, SLOT(horizontalSliderMovedEvent(int)));

    QObject::connect(ui->verticalSlider, SIGNAL(sliderMoved(int)),
                     this, SLOT(verticalSliderMovedEvent(int)));

    QObject::connect(ui->verticalSlider, SIGNAL(sliderReleased()),
                     this, SLOT(verticalSliderReleasedEvent()));

    QObject::connect(ui->horizontalSlider, SIGNAL(sliderReleased()),
                     this, SLOT(horizontalSliderReleasedEvent()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadButtonEvent()
{
    image = loadImage();
    texture.setData(image);
    ui->sphereViewer->setTexture(&texture);
    ui->openGLWidget->display(image);
}

void MainWindow::horizontalSliderMovedEvent(int value){
    //qDebug() << value;
    ui->sphereViewer->rotatePanorama(value, "y");
    QVector3D v(1, 0, 0);
    //rotatePanoramicImage(image, v, 0);
}

void MainWindow::verticalSliderMovedEvent(int value){
    //qDebug() << value;
    ui->sphereViewer->rotatePanorama(value, "x");
}

void MainWindow::verticalSliderReleasedEvent(){
    QVector3D v(0, 0, 1);
    qDebug() << "released";
    qDebug() << ui->verticalSlider->value();
    float theta = (ui->verticalSlider->value() * 2 * M_PI) / 100.0;
    rotatePanoramicImage(image, v, theta);
}

void MainWindow::horizontalSliderReleasedEvent(){
    QVector3D v(1, 0, 0);
    qDebug() << "released";
    qDebug() << ui->horizontalSlider->value();
    float theta = (ui->horizontalSlider->value() * 2 * M_PI) / 100.0;
    rotatePanoramicImage(image, v, theta);
}

QImage MainWindow::loadImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "",
        tr("All Files (*)"));

    if (fileName.isEmpty()){
        qDebug() << "Error, image is empty";
        return QImage();
    }
    else {
        QImage image = QImage(fileName);
        return image;
    }
}

QMatrix4x4 MainWindow::getRotationMatrixFromV(QVector3D axis, float angle)
{
   float a = cos(angle / 2.0);
   float b = -axis.x() * sin(angle / 2.0);
   float c = -axis.y() * sin(angle / 2.0);
   float d = -axis.z() * sin(angle / 2.0);
   float aa = a * a;
   float bb = b * b;
   float cc = c * c;
   float dd = d * d;
   float bc = b * c;
   float ad = a * d;
   float ac = a * c;
   float ab = a * b;
   float bd = b * d;
   float cd = c * d;

   return QMatrix4x4(aa + bb - cc -dd, 2 * (bc + ad), 2 * (bd - ac), 0,
                     2 * (bc - ad), aa + cc - bb - dd, 2 * (cd + ab), 0,
                     2 * (bd + ac), 2 * (cd - ab), aa + dd - bb - cc, 0,
                     0, 0, 0, 1 );

}

QImage MainWindow::rotatePanoramicImage(QImage image, QVector3D axis, float angle)
{

    QImage imageCopy = image.copy();
    float imageWidth = imageCopy.width();
    float imageHeight = imageCopy.height();
//    float imageWidth = 5;
//    float imageHeight = 5;
    QMatrix4x4 rotationM = getRotationMatrixFromV(axis, angle);

    for (float y = 0; y < imageHeight; y++) {
        QRgb *rowData = (QRgb*)imageCopy.scanLine(y);
        for (float x = 0; x < imageWidth; x++) {
            float centeredX = 2 * (x + 0.5) / imageWidth - 1;
            float centeredY = 2 * (y + 0.5) / imageHeight - 1;

            float lat = 0.5 * M_PI * centeredY;
            float lon = M_PI * centeredX;

            float XSphere = cos(lat) * cos(lon);
            float YSphere = cos(lat) * sin(lon);
            float ZSphere = sin(lat);

            QVector4D point(XSphere, YSphere, ZSphere, 1);
            QVector4D rotatedPoint = rotationM * point;

            float D = sqrt(rotatedPoint.x() * rotatedPoint.x() + rotatedPoint.y() * rotatedPoint.y());
            float newLat = atan2(rotatedPoint.z(), D);
            float newLng = atan2(rotatedPoint.y(), rotatedPoint.x());

            float newX = newLng / M_PI;
            float newY = newLat / (M_PI * 0.5);

            int imgCoordX = (round(((0.5 * (newX + 1)) * imageWidth) - 0.5));
            int imgCoordY = round(((0.5 * (newY + 1)) * imageHeight) - 0.5);

            imgCoordX = qMax(0, imgCoordX);
            imgCoordY = qMax(0, imgCoordY);

            imgCoordX = qMin(static_cast<int>(imageWidth), imgCoordX);
            imgCoordY = qMin(static_cast<int>(imageHeight), imgCoordY);
             QRgb *rowDataBase = (QRgb*)image.scanLine(imgCoordY);

            QRgb color = rowDataBase[imgCoordX];
            rowData[static_cast<int>(x)] = color;
        }
    }

    qDebug() << "DONE";

    ui->openGLWidget->display(imageCopy);

    return image;
}

