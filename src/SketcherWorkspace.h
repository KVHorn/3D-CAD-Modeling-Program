// Workspace.h — the common interface every workspace implements.
// Inherits QObject so its button handlers can be slots / connect targets.
#pragma once

#include <QObject>
#include <QString>
#include <QWidget>

class SketcherWorkspace : public QObject
{
    Q_OBJECT

public:
    explicit SketcherWorkspace(QObject *parent = nullptr) : QObject(parent) {}
    ~SketcherWorkspace() override = default;

    virtual QString name() const = 0;
    virtual void activate() = 0;   // show this workspace's toolbars
    virtual void deactivate() = 0; // hide them
};