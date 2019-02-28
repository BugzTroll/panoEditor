#include "mainwindow.h"
#include <QDebug>
#include <QSignalMapper>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include "imagedisplay.h"
#include "ui_mainwindow.h"

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
    qDebug() << value;
     ui->sphereViewer->rotatePanorama(value, "y");
}

void MainWindow::verticalSliderMovedEvent(int value){
    qDebug() << value;
    ui->sphereViewer->rotatePanorama(value, "x");
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

