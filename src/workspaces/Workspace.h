#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QObject>
#include <QString>

#include "WorkspaceType.h"

class QToolBar;
class QMainWindow;
class Document;

// The shared "tool board on the workshop wall". MainWindow owns exactly one
// instance of each concrete workspace; documents don't own workspaces, they
// only remember which one they're in. activate(doc) points the tools at a
// specific document's viewport and shows the toolbar; deactivate() puts the
// tools away.
class Workspace : public QObject
{
    Q_OBJECT

public:
    Workspace(const QString& name, QMainWindow* mainWindow);
    ~Workspace() override = default;

    virtual WorkspaceType type() const = 0;

    const QString& name() const { return m_name; }
    QToolBar* toolbar() const { return m_toolbar; }
    Document* activeDocument() const { return m_activeDocument; }

    void activate(Document* document);
    void deactivate();

protected:
    // Derived classes fill their toolbar with actions in their constructor.
    QToolBar* m_toolbar;
    Document* m_activeDocument;

private:
    QString m_name;
};

#endif // WORKSPACE_H
