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
    image = loadImage();
    ui->openGLWidget->display(image);
}

QImage MainWindow::loadImage() {
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

