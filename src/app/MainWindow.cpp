#include "app/MainWindow.h"
#include "app/Document.h"
#include "app/FeatureTreeDock.h"
#include "app/MenuBuilder.h"
#include "app/RecentFiles.h"
#include "workspaces/Workspace.h"
#include "workspaces/PartDesignWorkspace.h"
#include "workspaces/SketcherWorkspace.h"
#include "viewport/OcctViewport.h"
#include "model/PartModel.h"
#include "model/Feature.h"
#include "io/InfinityFormat.h"
#include "ui/OptionsDialog.h"
#include "ui/DocumentPropertiesDialog.h"

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QUndoGroup>
#include <QUndoStack>
#include <QStatusBar>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_mdiArea(new QMdiArea(this))
    , m_activeWorkspace(nullptr)
    , m_undoGroup(new QUndoGroup(this))
    , m_treeDock(new FeatureTreeDock(this))
    , m_recentFiles(new RecentFiles(this))
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
    connect(m_recentFiles, &RecentFiles::openRequested,
            this, &MainWindow::openPath);

    createWorkspaces();
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

Document* MainWindow::createDocument(const QString& name)
{
    auto* document = new Document(name);
    m_undoGroup->addStack(document->undoStack());
    document->setSaveHandler([this](Document* d) { return saveDocument(d); });

    QMdiSubWindow* subWindow = m_mdiArea->addSubWindow(document);
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    subWindow->resize(640, 480);

    connect(document, &Document::dirtyStateChanged, this,
            [this, document]() { updateSubWindowTitle(document); });

    updateSubWindowTitle(document);
    subWindow->show();
    return document;
}

void MainWindow::updateSubWindowTitle(Document* document)
{
    if (auto* subWindow = qobject_cast<QMdiSubWindow*>(document->parentWidget()))
        subWindow->setWindowTitle(document->name()
            + (document->isDirty() ? QStringLiteral(" *") : QString()));
}

// ---- File operations ----

void MainWindow::newPart()
{
    ++m_untitledCounter;
    createDocument(QStringLiteral("Part%1").arg(m_untitledCounter));
    statusBar()->showMessage(QStringLiteral("Created new part"), 3000);
}

void MainWindow::openFile()
{
    const QString path = QFileDialog::getOpenFileName(
        this, QStringLiteral("Open"), QString(),
        QStringLiteral("Infinity Creator part (*.infinity)"));
    if (!path.isEmpty())
        openPath(path);
}

void MainWindow::openPath(const QString& path)
{
    // Already open? Just focus it.
    for (QMdiSubWindow* window : m_mdiArea->subWindowList())
    {
        auto* existing = qobject_cast<Document*>(window->widget());
        if (existing && existing->filePath() == path)
        {
            m_mdiArea->setActiveSubWindow(window);
            return;
        }
    }

    Document* document = createDocument(QFileInfo(path).completeBaseName());

    QString error;
    if (!InfinityFormat::load(*document->model(), document->settings(), path, &error))
    {
        if (auto* subWindow = qobject_cast<QMdiSubWindow*>(document->parentWidget()))
            subWindow->close();
        QMessageBox::warning(this, QStringLiteral("Open failed"), error);
        return;
    }

    document->setFilePath(path);
    document->markSaved();
    updateSubWindowTitle(document);
    m_recentFiles->add(path);
    statusBar()->showMessage(QStringLiteral("Opened %1").arg(path), 5000);
}

void MainWindow::closeActiveDocument()
{
    if (QMdiSubWindow* window = m_mdiArea->activeSubWindow())
        window->close();  // Document::closeEvent handles dirty prompting
}

QString MainWindow::defaultSaveName()
{
    // Per SPEC: unnamed objects suggest Obj1.infinity, Obj2... on save.
    static int objCounter = 0;
    ++objCounter;
    return QStringLiteral("Obj%1.infinity").arg(objCounter);
}

bool MainWindow::saveDocument(Document* document)
{
    if (!document)
        return false;
    if (document->filePath().isEmpty())
        return saveDocumentAs(document);

    QString error;
    if (!InfinityFormat::save(*document->model(), document->settings(),
                              document->filePath(), &error))
    {
        QMessageBox::warning(this, QStringLiteral("Save failed"), error);
        return false;
    }

    document->markSaved();
    updateSubWindowTitle(document);
    m_recentFiles->add(document->filePath());
    statusBar()->showMessage(QStringLiteral("Saved %1").arg(document->filePath()), 5000);
    return true;
}

bool MainWindow::saveDocumentAs(Document* document)
{
    const QString suggested = document->filePath().isEmpty()
        ? (document->name().startsWith(QStringLiteral("Part"))
               ? defaultSaveName()
               : document->name() + QStringLiteral(".infinity"))
        : document->filePath();

    const QString path = QFileDialog::getSaveFileName(
        this, QStringLiteral("Save As"), suggested,
        QStringLiteral("Infinity Creator part (*.infinity)"));
    if (path.isEmpty())
        return false;

    document->setFilePath(path);
    document->setName(QFileInfo(path).completeBaseName());
    m_treeDock->rebuild();  // root node shows the (possibly new) name
    return saveDocument(document);
}

bool MainWindow::saveActiveDocument()
{
    return saveDocument(activeDocument());
}

bool MainWindow::saveActiveDocumentAs()
{
    Document* document = activeDocument();
    return document ? saveDocumentAs(document) : false;
}

void MainWindow::saveActiveDocumentAsCopy()
{
    Document* document = activeDocument();
    if (!document)
        return;

    const QString path = QFileDialog::getSaveFileName(
        this, QStringLiteral("Save As Copy"),
        document->name() + QStringLiteral("-copy.infinity"),
        QStringLiteral("Infinity Creator part (*.infinity)"));
    if (path.isEmpty())
        return;

    // A copy: written to disk, but this document keeps its own path and
    // dirty state untouched.
    QString error;
    if (!InfinityFormat::save(*document->model(), document->settings(), path, &error))
        QMessageBox::warning(this, QStringLiteral("Save As Copy failed"), error);
    else
        statusBar()->showMessage(QStringLiteral("Copy saved to %1").arg(path), 5000);
}

void MainWindow::saveAll()
{
    for (QMdiSubWindow* window : m_mdiArea->subWindowList())
    {
        auto* document = qobject_cast<Document*>(window->widget());
        if (document && document->isDirty())
        {
            if (!saveDocument(document))
                return;  // user cancelled a Save As - stop the sweep
        }
    }
    statusBar()->showMessage(QStringLiteral("All documents saved"), 3000);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // Close every document; each dirty one prompts via Document::closeEvent.
    // Any Cancel leaves that window open, which vetoes the exit.
    m_mdiArea->closeAllSubWindows();
    if (!m_mdiArea->subWindowList().isEmpty())
        event->ignore();
    else
        event->accept();
}

void MainWindow::exportStl()
{
    Document* document = activeDocument();
    if (!document || !document->hasShape())
    {
        QMessageBox::information(this, QStringLiteral("Export STL"),
            QStringLiteral("Create a part with geometry first."));
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

void MainWindow::export3mf()
{
    Document* document = activeDocument();
    if (!document || !document->hasShape())
    {
        QMessageBox::information(this, QStringLiteral("Export 3MF"),
            QStringLiteral("Create a part with geometry first."));
        return;
    }

    const QString filePath = QFileDialog::getSaveFileName(
        this, QStringLiteral("Export 3MF"),
        document->name() + QStringLiteral(".3mf"),
        QStringLiteral("3MF files (*.3mf)"));
    if (filePath.isEmpty())
        return;

    if (document->export3mf(filePath))
        statusBar()->showMessage(QStringLiteral("Exported %1").arg(filePath), 5000);
    else
        QMessageBox::warning(this, QStringLiteral("Export 3MF"),
            QStringLiteral("Export failed - see debug.log for details."));
}

void MainWindow::showDocumentProperties()
{
    Document* document = activeDocument();
    if (!document)
    {
        statusBar()->showMessage(QStringLiteral("Open a document first"), 3000);
        return;
    }

    DocumentPropertiesDialog dialog(&document->settings(), document->name(), this);
    if (dialog.exec() == QDialog::Accepted)
    {
        document->markSettingsDirty();
        updateSubWindowTitle(document);
    }
}

// ---- Edit operations ----

void MainWindow::showOptions()
{
    OptionsDialog dialog(this);
    dialog.exec();  // settings apply on next dialog/export; nothing recomputes
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

// ---- Insert / View / Window / Help ----

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
        QStringLiteral("<b>Infinity Creator</b> 0.3.0 (Phase 3)<br><br>"
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
