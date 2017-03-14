#include "interface/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QSurfaceFormat qsf;
    qsf.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
    qsf.setRenderableType(QSurfaceFormat::RenderableType::OpenGL);
    qsf.setVersion(4,3);
    QSurfaceFormat::setDefaultFormat(qsf);

    return a.exec();
}
