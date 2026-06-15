#include "MainWindow.h"
#include "OcctViewport.h"

#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QGridLayout>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Infinity Creator");
    resize(1200, 800);
    setCentralWidget(new OcctViewport(this));

    createMenus();   // conttains file, edit, help, etc.
    createToolBar(); // contains the sketcher tools when a sketch is open and the create feature tools when just viewing the body/object in 3D.
    createStatusBar();
}

void MainWindow::createMenus()
{

    /*******************************************************************************************
     *
     * Create Group Box that contains all of the menu tabs like: file, edit, settings, help, etc.
     *
     *
     *
     *******************************************************************************************/
    QGroupBox *groupBox = new QGroupBox(tr("Group Box with Layout"));

    // Create the Start menu tab button in the groupbox
    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&New...");
    fileMenu->addAction("&New From...");
    fileMenu->addAction("&Open...");
    fileMenu->addAction("&Close..."); // Used to close the current file, not the entire program.
    fileMenu->addAction("&Save...");
    fileMenu->addAction("&Save As...");
    fileMenu->addAction("&Exit", this, &QWidget::close);

    // Create the Edit menu tab button in the groupbox
    QMenu *editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction("&Undo...");
    editMenu->addAction("&Redo...");
    editMenu->addAction("&Settings");

    // // Create the Edit menu tab button in the groupbox
    // QMenu *fileMenu = menuBar()->addMenu("&View");
    // fileMenu->addAction("&Settings");

    // // Create the Edit menu tab button in the groupbox
    // QMenu *fileMenu = menuBar()->addMenu("&Window");
    // fileMenu->addAction("&View Settings");

    // // Create the Edit menu tab button in the groupbox
    // QMenu *fileMenu = menuBar()->addMenu("&Help");
    // fileMenu->addAction("&Settings");
}

void MainWindow::createToolBar()
{
    // QGroupBox *groupBox = new QGroupBox();

    QToolBar *toolBar = addToolBar("Main");
    /*******************************************************************************************
     *
     * Create the toolbar. This will be the groupBox containing different sets of tools based on
     *      which workbench is currently open.
     *
     * 
     *
     *
     *
     *
     *
     *
     *
     *******************************************************************************************/
    toolBar->addAction("New Sketch");
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage("Ready");
}
