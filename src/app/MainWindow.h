#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include "workspaces/WorkspaceType.h"

class QMdiArea;
class QMdiSubWindow;
class QUndoGroup;
class Workspace;
class Document;
class FeatureTreeDock;
class MenuBuilder;

// The application shell: MDI area, shared workbenches, specification tree
// dock, undo group, and the 7-tab menu system (built by MenuBuilder).
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    // Accessors used by MenuBuilder.
    QMdiArea* mdiArea() const { return m_mdiArea; }
    QUndoGroup* undoGroup() const { return m_undoGroup; }
    FeatureTreeDock* featureTree() const { return m_treeDock; }
    Document* activeDocument() const;

public slots:
    void newPart();
    void exportStl();
    void forceUpdate();
    void deleteSelectedFeature();
    void editSelectedFeature();
    void insertFeature(const QString& typeName);
    void switchWorkspace(WorkspaceType type);
    void fitAll();
    void setFullScreen(bool on);
    void tileHorizontally();
    void tileVertically();
    void showAbout();

private slots:
    void onSubWindowActivated(QMdiSubWindow* window);

private:
    void createWorkspaces();
    void activateWorkspaceFor(Document* document);

    QMdiArea* m_mdiArea;
    QMap<WorkspaceType, Workspace*> m_workspaces;
    Workspace* m_activeWorkspace;
    QUndoGroup* m_undoGroup;
    FeatureTreeDock* m_treeDock;
    MenuBuilder* m_menuBuilder;
    int m_untitledCounter;
};

#endif // MAINWINDOW_H
