#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include "workspaces/WorkspaceType.h"

class QMdiArea;
class QMdiSubWindow;
class QUndoGroup;
class QCloseEvent;
class Workspace;
class Document;
class FeatureTreeDock;
class MenuBuilder;
class RecentFiles;

// The application shell: MDI area, shared workbenches, specification tree,
// undo group, file operations, and the 7-tab menu system.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    QMdiArea* mdiArea() const { return m_mdiArea; }
    QUndoGroup* undoGroup() const { return m_undoGroup; }
    FeatureTreeDock* featureTree() const { return m_treeDock; }
    RecentFiles* recentFiles() const { return m_recentFiles; }
    Document* activeDocument() const;

public slots:
    // File
    void newPart();
    void openFile();
    void openPath(const QString& path);
    void closeActiveDocument();
    bool saveDocument(Document* document);   // false if cancelled/failed
    bool saveActiveDocument();
    bool saveActiveDocumentAs();
    void saveActiveDocumentAsCopy();
    void saveAll();
    void exportStl();
    void export3mf();
    void showDocumentProperties();
    // Edit
    void showOptions();
    void forceUpdate();
    void deleteSelectedFeature();
    void editSelectedFeature();
    // Insert / View / Window / Help
    void insertFeature(const QString& typeName);
    void switchWorkspace(WorkspaceType type);
    void fitAll();
    void setFullScreen(bool on);
    void tileHorizontally();
    void tileVertically();
    void showAbout();

protected:
    // Exit: closes every document, each prompting if dirty; any Cancel
    // vetoes the exit.
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onSubWindowActivated(QMdiSubWindow* window);

private:
    void createWorkspaces();
    void activateWorkspaceFor(Document* document);
    Document* createDocument(const QString& name);
    void updateSubWindowTitle(Document* document);
    bool saveDocumentAs(Document* document);
    QString defaultSaveName();

    QMdiArea* m_mdiArea;
    QMap<WorkspaceType, Workspace*> m_workspaces;
    Workspace* m_activeWorkspace;
    QUndoGroup* m_undoGroup;
    FeatureTreeDock* m_treeDock;
    MenuBuilder* m_menuBuilder;
    RecentFiles* m_recentFiles;
    int m_untitledCounter;
};

#endif // MAINWINDOW_H
