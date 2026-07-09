#include <QApplication>
#include <QIcon>
#include <QActionEvent>
#include <QMessageBox>
#include "MainWindow.h"
#include "OcctViewport.h"

#include "Geometry.h"

#ifdef _DEBUG
#include <windows.h>
#include <cstdio>
#include <QDebug>
#endif

int main(int argc, char *argv[])
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    qInstallMessageHandler([](QtMsgType, const QMessageLogContext &, const QString &msg)
                           {
            fprintf(stdout, "%s\n", msg.toLocal8Bit().constData());
            fflush(stdout); });
    /*
    QApplication app(argc, argv)
    Enters the main event loop and waits until exit() is called, then returns the value that was set to exit(), which is 0 if exit is called via quit().
    */
    QApplication app(argc, argv);
    app.setApplicationName("Infinity Creator TEST");
    app.setWindowIcon(QIcon(":/infinity_creator.ico"));

    MainWindow window;
    window.setWindowTitle("Infinity Creator TEST");
    window.show();

    // --- TRACER BULLET ---------------------------------------------------
    // Build a 50 x 30 x 10 box by extruding a rectangle, then write it to STL.
    // Proves the whole pipeline: OCCT solid -> tessellate -> printable file.
    TopoDS_Shape solid = Geometry::extrudeRectangle(50.0, 30.0, 10.0);
    bool ok = Geometry::exportStl(solid, "output/infinity_test_box.stl");

    QMessageBox::information(&window, "Tracer Bullet",
                             ok ? "Exported a 50x30x10 box to output/infinity_test_box.stl\nOpen it in your slicer!"
                                : "STL export failed.");
    // ---------------------------------------------------------------------

    return app.exec();
}
