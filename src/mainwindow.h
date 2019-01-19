#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void clickingButtonEvent ();

private:
    Ui::MainWindow *ui;

private slots:
    void onAddNew();
};

#endif // MAINWINDOW_H
