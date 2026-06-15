// Workspace.h — the common interface every workspace implements.
// Inherits QObject so its button handlers can be slots / connect targets.

#pragma once

#include <QObject>
#include <QString>
#include <QWidget>


class Workspace : public QObject
{
    Q_OBJECT

public:
    explicit Workspace(QObject *parent = nullptr) : QObject(parent) {}
    ~Workspace() override = default;

    virtual QString name() const = 0;
    virtual void activate() = 0;   // show this workspace's toolbars
    virtual void deactivate() = 0; // hide them
};