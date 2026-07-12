#include "app/MainWindow.h"
#include "app/Document.h"
#include "app/FeatureTreeDock.h"
#include "app/MenuBuilder.h"
#include "workspaces/Workspace.h"
#include "workspaces/PartDesignWorkspace.h"
#include "workspaces/SketcherWorkspace.h"
#include "viewport/OcctViewport.h"
#include "model/PartModel.h"
#include "model/Feature.h"

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QUndoGroup>
#include <QUndoStack>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_mdiArea(new QMdiArea(this))
    , m_activeWorkspace(nullptr)
    , m_undoGroup(new QUndoGroup(this))
    , m_treeDock(new FeatureTreeDock(this))
    , m_untitledCounter(0)
{
    setWindowTitle(QStringLiteral("Infinity Creator"));
    resize(1280, 800);

    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(m_mdiArea);

    addDockWidget(Qt::LeftDockWidgetArea, m_treeDock);

    connect(m_mdiArea, &QMdiArea::subWindowActivated,
            this, &MainWindow::onSubWindowActivated);

    createWorkspaces();

    // Menus last: MenuBuilder needs the undo group and tree dock to exist.
    m_menuBuilder = new MenuBuilder(this);

    statusBar()->showMessage(QStringLiteral("Ready - File > New Part to open a document"));
}

void MainWindow::createWorkspaces()
{
    m_workspaces[WorkspaceType::PartDesign] = new PartDesignWorkspace(this);
    m_workspaces[WorkspaceType::Sketcher] = new SketcherWorkspace(this);
}

Document* MainWindow::activeDocument() const
{
    QMdiSubWindow* window = m_mdiArea->activeSubWindow();
    if (!window)
        return nullptr;
    return qobject_cast<Document*>(window->widget());
}

void MainWindow::newPart()
{
    ++m_untitledCounter;
    const QString name = QStringLiteral("Part%1").arg(m_untitledCounter);

    auto* document = new Document(name);
    m_undoGroup->addStack(document->undoStack());

    QMdiSubWindow* subWindow = m_mdiArea->addSubWindow(document);
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    subWindow->setWindowTitle(name);
    subWindow->resize(640, 480);
    subWindow->show();

    statusBar()->showMessage(QStringLiteral("Created %1").arg(name), 3000);
}

void MainWindow::exportStl()
{
    Document* document = activeDocument();
    if (!document)
    {
        QMessageBox::information(this, QStringLiteral("Export STL"),
            QStringLiteral("Open or create a part first (File > New Part)."));
        return;
    }
    if (!document->hasShape())
    {
        QMessageBox::information(this, QStringLiteral("Export STL"),
            QStringLiteral("This part has no geometry yet. Insert a feature in Part Design."));
        return;
    }

    const QString filePath = QFileDialog::getSaveFileName(
        this, QStringLiteral("Export STL"),
        document->name() + QStringLiteral(".stl"),
        QStringLiteral("STL files (*.stl)"));
    if (filePath.isEmpty())
        return;

    if (document->exportStl(filePath))
        statusBar()->showMessage(QStringLiteral("Exported %1").arg(filePath), 5000);
    else
        QMessageBox::warning(this, QStringLiteral("Export STL"),
            QStringLiteral("Export failed - see debug.log for details."));
}

void MainWindow::forceUpdate()
{
    if (Document* document = activeDocument())
    {
        document->model()->recompute();
        statusBar()->showMessage(QStringLiteral("Updated"), 2000);
    }
}

void MainWindow::deleteSelectedFeature()
{
    Document* document = activeDocument();
    Feature* feature = m_treeDock->selectedFeature();
    if (document && feature)
        document->deleteFeature(feature);
    else
        statusBar()->showMessage(
            QStringLiteral("Select a feature in the specification tree first"), 3000);
}

void MainWindow::editSelectedFeature()
{
    Document* document = activeDocument();
    Feature* feature = m_treeDock->selectedFeature();
    if (document && feature)
        document->editFeature(feature);
    else
        statusBar()->showMessage(
            QStringLiteral("Select a feature in the specification tree first"), 3000);
}

void MainWindow::insertFeature(const QString& typeName)
{
    Document* document = activeDocument();
    if (!document)
    {
        statusBar()->showMessage(
            QStringLiteral("Open a document before inserting features"), 3000);
        return;
    }
    document->insertFeatureInteractive(typeName, this);
}

void MainWindow::fitAll()
{
    Document* document = activeDocument();
    if (document && !document->viewport()->view().IsNull())
    {
        document->viewport()->view()->FitAll();
        document->viewport()->view()->Redraw();
    }
}

void MainWindow::setFullScreen(bool on)
{
    if (on)
        showFullScreen();
    else
        showNormal();
}

void MainWindow::tileHorizontally()
{
    // Side-by-side columns spanning full height.
    const auto windows = m_mdiArea->subWindowList();
    if (windows.isEmpty())
        return;

    const int width = m_mdiArea->width() / windows.size();
    int x = 0;
    for (QMdiSubWindow* window : windows)
    {
        window->showNormal();
        window->setGeometry(x, 0, width, m_mdiArea->height());
        x += width;
    }
}

void MainWindow::tileVertically()
{
    // Stacked rows spanning full width.
    const auto windows = m_mdiArea->subWindowList();
    if (windows.isEmpty())
        return;

    const int height = m_mdiArea->height() / windows.size();
    int y = 0;
    for (QMdiSubWindow* window : windows)
    {
        window->showNormal();
        window->setGeometry(0, y, m_mdiArea->width(), height);
        y += height;
    }
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, QStringLiteral("About Infinity Creator"),
        QStringLiteral("<b>Infinity Creator</b> 0.2.0 (Phase 2)<br><br>"
                       "A parametric 3D CAD modeler for individual part design "
                       "and 3D-printable output.<br><br>"
                       "Built with Qt and OpenCASCADE Technology."));
}

void MainWindow::onSubWindowActivated(QMdiSubWindow* window)
{
    if (!window)
    {
        if (m_activeWorkspace)
        {
            m_activeWorkspace->deactivate();
            m_activeWorkspace = nullptr;
        }
        m_treeDock->setDocument(nullptr);
        return;
    }

    Document* document = qobject_cast<Document*>(window->widget());
    if (document)
    {
        activateWorkspaceFor(document);
        m_treeDock->setDocument(document);
        m_undoGroup->setActiveStack(document->undoStack());
    }
}

void MainWindow::activateWorkspaceFor(Document* document)
{
    Workspace* target = m_workspaces.value(document->currentWorkspace(), nullptr);
    if (!target)
    {
        qWarning() << "No workspace registered for document" << document->name();
        return;
    }

    if (m_activeWorkspace && m_activeWorkspace != target)
        m_activeWorkspace->deactivate();

    m_activeWorkspace = target;
    target->activate(document);

    statusBar()->showMessage(QStringLiteral("%1 - %2")
        .arg(document->name(), target->name()), 3000);
}

void MainWindow::switchWorkspace(WorkspaceType type)
{
    Document* document = activeDocument();
    if (!document)
    {
        statusBar()->showMessage(
            QStringLiteral("Open a document before switching workspaces"), 3000);
        return;
    }

    document->setCurrentWorkspace(type);
    activateWorkspaceFor(document);
}
