#include "MainWindow.h"

#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Infinity Creator");
    resize(1200, 800);

    createMenus();
    createToolBar();
    createStatusBar();
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&New");
    fileMenu->addAction("&Open...");
    fileMenu->addAction("&Save");
    fileMenu->addSeparator();
    fileMenu->addAction("&Exit", this, &QWidget::close);

    QMenu *sketchMenu = menuBar()->addMenu("&Sketch");
    sketchMenu->addAction("New Sketch");

    QMenu *featuresMenu = menuBar()->addMenu("Fea&tures");
    featuresMenu->addAction("Extrude");
    featuresMenu->addAction("Pocket");

    //Create an additional tab, labeled Options. Doing this to test and learn Qt.
  
}

void MainWindow::createToolBar()
{
    QToolBar *toolBar = addToolBar("Main");
    toolBar->addAction("New Sketch");
    toolBar->addAction("Extrude");
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage("Ready");
}





