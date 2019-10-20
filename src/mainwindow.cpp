#define _USE_MATH_DEFINES
#include <cmath>
#include "mainwindow.h"
#include <QDebug>
#include <QSignalMapper>
#include <QMessageBox>
#include <QFileDialog>
#include "imagedisplay.h"
#include "ui_mainwindow.h"
#include <QtGlobal>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_texture(QOpenGLTexture::Target2D),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    QObject::connect(m_ui->loadButton, SIGNAL(clicked()),
                     this, SLOT(loadButtonEvent()));

    QObject::connect(m_ui->saveButton, SIGNAL(clicked()),
                     this, SLOT(saveButtonEvent()));

    QObject::connect(m_ui->verticalSlider, SIGNAL(sliderReleased()),
                     this, SLOT(verticalSliderReleasedEvent()));

    QObject::connect(m_ui->horizontalSlider, SIGNAL(sliderReleased()),
                     this, SLOT(horizontalSliderReleasedEvent()));
}

MainWindow::~MainWindow()
{
    m_ui->sphereViewer->makeCurrent();
    m_texture.destroy();
    delete m_ui;
}

void MainWindow::loadButtonEvent()
{
    m_texture.destroy();
    m_texture.create();
    m_image = loadImage();
	if(!m_image.isNull()){
		m_texture.setData(m_image);

		m_ui->sphereViewer->setTexture(&m_texture, m_image);
		m_ui->openGLWidget->display(m_image);
	}
}

void MainWindow::saveButtonEvent(){

    QString fileName = QFileDialog::getSaveFileName(
		this,
        tr("Save Image"), "",
        tr("JPEG (*.jpg *.jpeg);;PNG (*.png)")
	);

    if (fileName.isEmpty())
        return;
    else {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                file.errorString());
            return;
        }

        m_image.save(fileName);
    }
}

void MainWindow::verticalSliderReleasedEvent()
{
	m_ui->verticalLabel->setText(QString::number(m_ui->verticalSlider->value()) + "°");
    QVector3D v(0, 0, 1);
	rotateImage(m_ui->verticalSlider, v);
}

void MainWindow::horizontalSliderReleasedEvent()
{
	m_ui->horizontalLabel->setText(QString::number(m_ui->horizontalSlider->value()) + "°");
	QVector3D v(1, 0, 0);
	rotateImage(m_ui->horizontalSlider, v);
}

void MainWindow::rotateImage(QSlider *slider, const QVector3D &vector){
	float theta = (slider->value() * M_PI) / 180.0;
	QImage rotatedImage = m_ui->sphereViewer->rotateImage(vector, theta);
	m_ui->openGLWidget->display(rotatedImage);
	m_image = rotatedImage;
}

QImage MainWindow::loadImage()
{
    QString fileName = QFileDialog::getOpenFileName(
		this,
        tr("Open File"), "",
        tr("JPEG (*.jpg *.jpeg);;PNG (*.png)")
	);

    if (fileName.isEmpty()){
        qDebug() << "Error, image is empty";
        return QImage();
    } else {
        return QImage(fileName);
    }

  //temp quick loading
  //QImage image = QImage("C:\\Users\\Bugz\\Documents\\panoEditor\\data\\jail.jpg");

  return m_image;
}

