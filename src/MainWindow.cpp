#include "MainWindow.h"
#include "OcctViewport.h"
#include "Workspace.h"
#include "SketcherWorkspace.h"
#include "PartDesignWorkspace.h"


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

    createMenus(); // conttains file, edit, help, etc.
    sketcherWorkspaceLineDrawingToolBar();
    sketcherWorkspaceCircleToolBar();
    sketcherWorkspacConstraintsToolBar();
    // createToolBar(); // contains the sketcher tools when a sketch is open and the create feature tools when just viewing the body/object in 3D.
    createStatusBar();
}

void MainWindow::createMenus()
{
    /*******************************************************************************************
     * Create Group Box that contains all of the menu tabs like: file, edit, settings, help, etc.
     *******************************************************************************************/
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

void MainWindow::sketcherWorkspaceLineDrawingToolBar()
{
    /*
    SKETCHER WORKSPACE toolbar
    This widget is a moveable toolbar button that the user can click and drag to different places aroun the screen
    */
    QToolBar *toolBar = addToolBar("Main");
    toolBar->setWindowTitle("Line Creator");
    toolBar->addAction("Line");
    toolBar->addAction("Infinite\nLine");
    toolBar->addSeparator();
    toolBar->addAction("Bi-Tangent\nLine");
    toolBar->addSeparator();
    toolBar->addAction("Bisecting\nLine");
    toolBar->addSeparator();
    toolBar->addAction("Line Normal\nto Curve");
}

void MainWindow::sketcherWorkspaceCircleToolBar()
{
    /*
    SKETCHER WORKSPACE toolbar
    This toolbar is for the group of tools under the "Circle" category.
    */
    QToolBar *toolBar = addToolBar("Main");
    toolBar->setWindowTitle("Circles");
    toolBar->addAction("Circle");
    toolBar->addAction("Arc");
    toolBar->addAction("Three-point\nCircle");
    toolBar->addAction("Circle\nusing Coords");
    toolBar->addAction("Tri-tangent\nCircle");
    toolBar->addAction("Three-point arc");
    toolBar->addAction("Three-point arc\nw/starting limits");
}

void MainWindow::sketcherWorkspacConstraintsToolBar()
{
    /*
    SKETCHER WORKSPACE toolbar
    This widget is a moveable toolbar button that the user can click and drag to different places aroun the screen
    */
    QToolBar *toolBar = addToolBar("Main");
    toolBar->setWindowTitle("Constraints");
    toolBar->addAction("Constraint"); // Defined by dialog box popup.
    toolBar->addAction("Fix\nTogether");
    toolBar->addAction("Animate\nConstraint");
    toolBar->addAction("Edit\nMulti-Constraint");
    toolBar->addAction("Length");
    toolBar->addAction("Offset");
    toolBar->addAction("Radius");
    toolBar->addAction("Angle");
    toolBar->addAction("Diam/Rad");
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage("Ready");
}

// void MainWindow::setWorkspace(Workspace *ws)
// {
//     if (m_active)
//         m_active->deactivate();
//     m_active = ws;
//     m_active->activate();
// }