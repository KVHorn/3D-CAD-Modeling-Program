#include <QApplication>
#include <QIcon>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Infinity Creator");
    app.setWindowIcon(QIcon(":/infinity_creator.ico"));   // title bar, taskbar, Alt+Tab

    MainWindow window;
    window.setWindowTitle("Infinity Creator");
    window.show();
    return app.exec();
}