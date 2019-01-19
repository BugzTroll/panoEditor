#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSignalMapper>

#include "window.h"
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //ui->setupUi(this);

//    QObject::connect(ui->myFirstButton, SIGNAL(clicked()),
//                     this, SLOT(clickingButtonEvent()));

    QMenuBar *menuBar = new QMenuBar;
    QMenu *menuWindow = menuBar->addMenu(tr("&Window"));
    QAction *addNew = new QAction(menuWindow);
    addNew->setText(tr("Add new"));
    menuWindow->addAction(addNew);
    connect(addNew, &QAction::triggered, this, &MainWindow::onAddNew);
    setMenuBar(menuBar);

    onAddNew();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clickingButtonEvent(){
    qDebug() << "clicked";
}

void MainWindow::onAddNew()
{
    if (!centralWidget())
        setCentralWidget(new Window(this));
    else
        QMessageBox::information(nullptr, tr("Cannot add new window"), tr("Already occupied. Undock first."));
    qDebug() << "OPEN FILE";

}

