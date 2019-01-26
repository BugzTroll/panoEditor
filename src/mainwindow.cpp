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
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->loadButton, SIGNAL(clicked()),
                     this, SLOT(loadButtonEvent()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadButtonEvent(){
    image = this->loadFile();
    ui->openGLWidget->resizeImageToFit(*image);
    ui->openGLWidget->display(*image);
}

QImage* MainWindow::loadFile() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "",
        tr("All Files (*)"));

     qDebug() << "loading image" << fileName;

    if (fileName.isEmpty()){
        qDebug() << "Error, image is empty";
        return new QImage();
    }
    else {
        QImage *image = new QImage(fileName);
        return image;
    }
}

