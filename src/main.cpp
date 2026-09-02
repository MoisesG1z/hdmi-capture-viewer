#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    MainWindow window;
    window.setWindowTitle("HDMI Capture Viewer");
    window.resize(1280, 720);
    window.show();
    
    return app.exec();
}
