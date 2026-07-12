#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>
#include <QString>

#include "workspaces/WorkspaceType.h"

class OcctViewport;
class PartModel;
class Feature;
class QUndoStack;

// One open part. Owns its OcctViewport, its PartModel (the feature
// history), and its undo stack, and remembers which workbench it is in.
// Lives inside a QMdiSubWindow managed by MainWindow.
class Document : public QWidget
{
    Q_OBJECT

public:
    explicit Document(const QString& name, QWidget* parent = nullptr);

    const QString& name() const { return m_name; }
    OcctViewport* viewport() const { return m_viewport; }
    PartModel* model() const { return m_model; }
    QUndoStack* undoStack() const { return m_undoStack; }

    WorkspaceType currentWorkspace() const { return m_currentWorkspace; }
    void setCurrentWorkspace(WorkspaceType type) { m_currentWorkspace = type; }

    bool isEmpty() const;
    bool hasShape() const;

    // The full interactive creation flow: factory -> definition dialog ->
    // undoable add. Used by both workbench toolbars and the Insert menu.
    void insertFeatureInteractive(const QString& typeName, QWidget* dialogParent);

    // Undoable operations on existing features (spec tree, Edit menu).
    void editFeature(Feature* feature);
    void toggleSuppress(Feature* feature);
    void deleteFeature(Feature* feature);

    bool exportStl(const QString& filePath) const;

private slots:
    void refreshDisplay();

private:
    QString m_name;
    OcctViewport* m_viewport;
    PartModel* m_model;
    QUndoStack* m_undoStack;
    WorkspaceType m_currentWorkspace;
};

#endif // DOCUMENT_H
