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
    qDebug() << "click";
    loadFile();
}

void MainWindow::loadFile() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "",
        tr("All Files (*)"));

     qDebug() << "loading image" << fileName;

    if (fileName.isEmpty())
        return;
    else {
        QImage *image = new QImage(fileName);
        ui->openGLWidget->display(*image);
    }
}

