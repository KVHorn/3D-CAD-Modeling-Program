#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>
#include <QString>
#include <functional>

#include "workspaces/WorkspaceType.h"
#include "model/DocumentSettings.h"

class OcctViewport;
class PartModel;
class Feature;
class QUndoStack;

// One open part. Owns its OcctViewport, PartModel, undo stack, per-file
// settings, and (once saved) its .infinity path. Lives inside a
// QMdiSubWindow managed by MainWindow.
class Document : public QWidget
{
    Q_OBJECT

public:
    explicit Document(const QString& name, QWidget* parent = nullptr);

    const QString& name() const { return m_name; }
    void setName(const QString& name);

    OcctViewport* viewport() const { return m_viewport; }
    PartModel* model() const { return m_model; }
    QUndoStack* undoStack() const { return m_undoStack; }

    DocumentSettings& settings() { return m_settings; }
    const DocumentSettings& settings() const { return m_settings; }

    // Empty until first Save/Save As, or set by Open.
    const QString& filePath() const { return m_filePath; }
    void setFilePath(const QString& path) { m_filePath = path; }

    // Dirty = unsaved model changes (tracked by the undo stack's clean
    // state) or unsaved settings changes.
    bool isDirty() const;
    void markSettingsDirty();
    void markSaved();

    WorkspaceType currentWorkspace() const { return m_currentWorkspace; }
    void setCurrentWorkspace(WorkspaceType type) { m_currentWorkspace = type; }

    bool isEmpty() const;
    bool hasShape() const;

    // Interactive flows (definition dialogs in the document's units).
    void insertFeatureInteractive(const QString& typeName, QWidget* dialogParent);
    void editFeature(Feature* feature);
    void toggleSuppress(Feature* feature);
    void deleteFeature(Feature* feature);

    bool exportStl(const QString& filePath) const;
    bool export3mf(const QString& filePath) const;

    // MainWindow installs this so close-with-unsaved-changes can trigger a
    // save; returns false if the user cancelled the save dialog.
    void setSaveHandler(std::function<bool(Document*)> handler);

signals:
    void dirtyStateChanged();

protected:
    // Prompts Save / Discard / Cancel when closing dirty.
    void closeEvent(QCloseEvent* event) override;

private slots:
    void refreshDisplay();

private:
    QString m_name;
    QString m_filePath;
    OcctViewport* m_viewport;
    PartModel* m_model;
    QUndoStack* m_undoStack;
    DocumentSettings m_settings;
    bool m_settingsDirty = false;
    WorkspaceType m_currentWorkspace;
    std::function<bool(Document*)> m_saveHandler;
};

#endif // DOCUMENT_H
