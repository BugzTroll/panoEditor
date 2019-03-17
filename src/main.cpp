
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {

    QSurfaceFormat glFormat;
    glFormat.setVersion(4, 6);
    glFormat.setRenderableType(QSurfaceFormat::OpenGL);
    glFormat.setProfile(QSurfaceFormat::CoreProfile); // Requires >=Qt-4.8.0

    QApplication app(argc, argv);

    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
