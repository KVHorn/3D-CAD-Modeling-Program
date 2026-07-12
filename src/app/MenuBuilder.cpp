#include "app/MenuBuilder.h"
#include "app/MainWindow.h"
#include "app/Document.h"
#include "app/FeatureTreeDock.h"
#include "app/RecentFiles.h"
#include "model/FeatureFactory.h"
#include "workspaces/WorkspaceType.h"

#include <QMenuBar>
#include <QMenu>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QUndoGroup>
#include <QKeySequence>

MenuBuilder::MenuBuilder(MainWindow* mainWindow)
    : QObject(mainWindow)
    , m_mainWindow(mainWindow)
{
    buildFileMenu();
    buildEditMenu();
    buildViewMenu();
    buildInsertMenu();
    buildWorkspaceMenu();
    buildWindowMenu();
    buildHelpMenu();
}

QAction* MenuBuilder::addPlanned(QMenu* menu, const QString& text, int phase)
{
    QAction* action = menu->addAction(text);
    action->setEnabled(false);
    action->setToolTip(QStringLiteral("Arrives in Phase %1").arg(phase));
    return action;
}

void MenuBuilder::buildFileMenu()
{
    QMenu* menu = m_mainWindow->menuBar()->addMenu(QStringLiteral("&File"));

    QAction* newAction = menu->addAction(QStringLiteral("&New Part"));
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, m_mainWindow, &MainWindow::newPart);

    QAction* openAction = menu->addAction(QStringLiteral("&Open..."));
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, m_mainWindow, &MainWindow::openFile);

    QAction* closeAction = menu->addAction(QStringLiteral("&Close"));
    closeAction->setShortcut(QKeySequence::Close);
    connect(closeAction, &QAction::triggered, m_mainWindow, &MainWindow::closeActiveDocument);

    menu->addSeparator();

    QAction* saveAction = menu->addAction(QStringLiteral("&Save"));
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, m_mainWindow, &MainWindow::saveActiveDocument);

    QAction* saveAsAction = menu->addAction(QStringLiteral("Save &As..."));
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, m_mainWindow, &MainWindow::saveActiveDocumentAs);

    QAction* saveCopyAction = menu->addAction(QStringLiteral("Save As Copy..."));
    connect(saveCopyAction, &QAction::triggered, m_mainWindow, &MainWindow::saveActiveDocumentAsCopy);

    QAction* saveAllAction = menu->addAction(QStringLiteral("Save A&ll"));
    connect(saveAllAction, &QAction::triggered, m_mainWindow, &MainWindow::saveAll);

    menu->addSeparator();

    QAction* exportStlAction = menu->addAction(QStringLiteral("&Export STL..."));
    connect(exportStlAction, &QAction::triggered, m_mainWindow, &MainWindow::exportStl);

    QAction* export3mfAction = menu->addAction(QStringLiteral("Export &3MF..."));
    connect(export3mfAction, &QAction::triggered, m_mainWindow, &MainWindow::export3mf);

    menu->addSeparator();

    addPlanned(menu, QStringLiteral("&Print..."), 12);
    addPlanned(menu, QStringLiteral("Printer Setup..."), 12);
    menu->addSeparator();

    QAction* docPropsAction = menu->addAction(QStringLiteral("Document Properties..."));
    connect(docPropsAction, &QAction::triggered, m_mainWindow, &MainWindow::showDocumentProperties);

    menu->addMenu(m_mainWindow->recentFiles()->menu());
    menu->addSeparator();

    QAction* exitAction = menu->addAction(QStringLiteral("E&xit"));
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, m_mainWindow, &QWidget::close);
}

void MenuBuilder::buildEditMenu()
{
    QMenu* menu = m_mainWindow->menuBar()->addMenu(QStringLiteral("&Edit"));

    // Undo/Redo route through the QUndoGroup to whichever document has
    // focus; the actions auto-enable and show the operation name.
    QAction* undoAction = m_mainWindow->undoGroup()->createUndoAction(this, QStringLiteral("&Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    menu->addAction(undoAction);

    QAction* redoAction = m_mainWindow->undoGroup()->createRedoAction(this, QStringLiteral("&Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    menu->addAction(redoAction);

    menu->addSeparator();

    QAction* updateAction = menu->addAction(QStringLiteral("Up&date"));
    updateAction->setToolTip(QStringLiteral("Force a full recompute of the active part"));
    connect(updateAction, &QAction::triggered, m_mainWindow, &MainWindow::forceUpdate);

    menu->addSeparator();
    addPlanned(menu, QStringLiteral("Cu&t"), 5);
    addPlanned(menu, QStringLiteral("&Copy"), 5);
    addPlanned(menu, QStringLiteral("&Paste"), 5);
    menu->addSeparator();

    QAction* deleteAction = menu->addAction(QStringLiteral("&Delete"));
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setToolTip(QStringLiteral("Delete the feature selected in the specification tree"));
    connect(deleteAction, &QAction::triggered, m_mainWindow, &MainWindow::deleteSelectedFeature);

    QAction* propertiesAction = menu->addAction(QStringLiteral("P&roperties"));
    propertiesAction->setToolTip(QStringLiteral("Edit the selected feature's definition"));
    connect(propertiesAction, &QAction::triggered, m_mainWindow, &MainWindow::editSelectedFeature);

    menu->addSeparator();

    QAction* optionsAction = menu->addAction(QStringLiteral("&Options..."));
    connect(optionsAction, &QAction::triggered, m_mainWindow, &MainWindow::showOptions);
}

void MenuBuilder::buildViewMenu()
{
    QMenu* menu = m_mainWindow->menuBar()->addMenu(QStringLiteral("&View"));

    addPlanned(menu, QStringLiteral("Toolbars..."), 12);
    menu->addSeparator();

    // The dock provides its own show/hide checkable action.
    QAction* treeAction = m_mainWindow->featureTree()->toggleViewAction();
    treeAction->setText(QStringLiteral("Specification Tree"));
    menu->addAction(treeAction);
    addPlanned(menu, QStringLiteral("View Cube"), 11);
    menu->addSeparator();

    QAction* fitAction = menu->addAction(QStringLiteral("&Fit All In"));
    connect(fitAction, &QAction::triggered, m_mainWindow, &MainWindow::fitAll);

    addPlanned(menu, QStringLiteral("Zoom Area"), 11);
    addPlanned(menu, QStringLiteral("Named Views"), 11);
    addPlanned(menu, QStringLiteral("Render Style"), 11);
    menu->addSeparator();

    QAction* fullScreenAction = menu->addAction(QStringLiteral("F&ull Screen"));
    fullScreenAction->setCheckable(true);
    fullScreenAction->setShortcut(QKeySequence::FullScreen);
    connect(fullScreenAction, &QAction::toggled, m_mainWindow, &MainWindow::setFullScreen);
}

void MenuBuilder::buildInsertMenu()
{
    QMenu* menu = m_mainWindow->menuBar()->addMenu(QStringLiteral("&Insert"));

    addPlanned(menu, QStringLiteral("&Sketch..."), 5);
    menu->addSeparator();

    QMenu* primitivesMenu = menu->addMenu(QStringLiteral("&Primitives"));
    for (const QString& typeName : FeatureFactory::primitiveTypes())
    {
        QAction* action = primitivesMenu->addAction(typeName);
        connect(action, &QAction::triggered, m_mainWindow,
                [this, typeName]() { m_mainWindow->insertFeature(typeName); });
    }

    QMenu* dressUpMenu = menu->addMenu(QStringLiteral("&Dress-Up Features"));
    for (const QString& typeName : FeatureFactory::dressUpTypes())
    {
        QAction* action = dressUpMenu->addAction(typeName);
        connect(action, &QAction::triggered, m_mainWindow,
                [this, typeName]() { m_mainWindow->insertFeature(typeName); });
    }
    addPlanned(dressUpMenu, QStringLiteral("Draft"), 4);
    addPlanned(dressUpMenu, QStringLiteral("Shell"), 4);
    addPlanned(dressUpMenu, QStringLiteral("Thickness"), 4);

    QMenu* sketchBasedMenu = menu->addMenu(QStringLiteral("Sketch-&Based Features"));
    addPlanned(sketchBasedMenu, QStringLiteral("Pad"), 8);
    addPlanned(sketchBasedMenu, QStringLiteral("Pocket"), 8);
    addPlanned(sketchBasedMenu, QStringLiteral("Shaft"), 8);
    addPlanned(sketchBasedMenu, QStringLiteral("Hole"), 8);
    addPlanned(sketchBasedMenu, QStringLiteral("Slot"), 9);
    addPlanned(sketchBasedMenu, QStringLiteral("Groove"), 9);

    QMenu* transformMenu = menu->addMenu(QStringLiteral("&Transformation Features"));
    addPlanned(transformMenu, QStringLiteral("Translation"), 9);
    addPlanned(transformMenu, QStringLiteral("Rotation"), 9);
    addPlanned(transformMenu, QStringLiteral("Mirror"), 9);
    addPlanned(transformMenu, QStringLiteral("Scale"), 9);
    addPlanned(transformMenu, QStringLiteral("Rectangular Pattern"), 9);
    addPlanned(transformMenu, QStringLiteral("Circular Pattern"), 9);

    QMenu* referenceMenu = menu->addMenu(QStringLiteral("&Reference Elements"));
    addPlanned(referenceMenu, QStringLiteral("Point"), 4);
    addPlanned(referenceMenu, QStringLiteral("Line"), 4);
    addPlanned(referenceMenu, QStringLiteral("Plane"), 4);
}

void MenuBuilder::buildWorkspaceMenu()
{
    QMenu* menu = m_mainWindow->menuBar()->addMenu(QStringLiteral("Wor&kspace"));

    QAction* partDesignAction = menu->addAction(QStringLiteral("&Part Design"));
    connect(partDesignAction, &QAction::triggered, m_mainWindow,
            [this]() { m_mainWindow->switchWorkspace(WorkspaceType::PartDesign); });

    QAction* sketcherAction = menu->addAction(QStringLiteral("&Sketcher"));
    connect(sketcherAction, &QAction::triggered, m_mainWindow,
            [this]() { m_mainWindow->switchWorkspace(WorkspaceType::Sketcher); });
}

void MenuBuilder::buildWindowMenu()
{
    QMenu* menu = m_mainWindow->menuBar()->addMenu(QStringLiteral("&Window"));

    // Rebuilt on every open so the document list below stays current.
    connect(menu, &QMenu::aboutToShow, this, [this, menu]()
    {
        menu->clear();

        QAction* tileHAction = menu->addAction(QStringLiteral("Tile &Horizontally"));
        connect(tileHAction, &QAction::triggered, m_mainWindow, &MainWindow::tileHorizontally);

        QAction* tileVAction = menu->addAction(QStringLiteral("Tile &Vertically"));
        connect(tileVAction, &QAction::triggered, m_mainWindow, &MainWindow::tileVertically);

        QAction* cascadeAction = menu->addAction(QStringLiteral("&Cascade"));
        connect(cascadeAction, &QAction::triggered,
                m_mainWindow->mdiArea(), &QMdiArea::cascadeSubWindows);

        const auto subWindows = m_mainWindow->mdiArea()->subWindowList();
        if (!subWindows.isEmpty())
            menu->addSeparator();

        // Open-documents list, inline in the dropdown (per SPEC.md).
        for (QMdiSubWindow* subWindow : subWindows)
        {
            QAction* action = menu->addAction(subWindow->windowTitle());
            action->setCheckable(true);
            action->setChecked(subWindow == m_mainWindow->mdiArea()->activeSubWindow());
            connect(action, &QAction::triggered, m_mainWindow, [this, subWindow]()
            {
                m_mainWindow->mdiArea()->setActiveSubWindow(subWindow);
            });
        }
    });
}

void MenuBuilder::buildHelpMenu()
{
    QMenu* menu = m_mainWindow->menuBar()->addMenu(QStringLiteral("&Help"));

    addPlanned(menu, QStringLiteral("Infinity Creator &Help"), 12);
    addPlanned(menu, QStringLiteral("Infinity Creator User &Companion"), 12);
    menu->addSeparator();

    QAction* aboutAction = menu->addAction(QStringLiteral("&About Infinity Creator"));
    connect(aboutAction, &QAction::triggered, m_mainWindow, &MainWindow::showAbout);
}
